#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "../../Helper.hpp"

using namespace std;


BOOST_AUTO_TEST_CASE( test_performance1 )
{
    string payload {"GET /bootsrap.css HTTP/1.2\r\nConnection: keep-alive\r\nHost: testapp1.local\r\nUser-Agent: http1.2-test\r\n\r\n"};

    cout << payload.c_str() << endl;

    int PortNr = 80;
    int Socket = socket(AF_INET, SOCK_STREAM, 0);

    //- set socket TCP_NODELAY for sending packets
    const int sockopt = 1;
    setsockopt(Socket, IPPROTO_TCP, TCP_NODELAY, &sockopt, sizeof(int));

    //- make socket nonblocking
    Socket::makeNonblocking(Socket);

    //struct hostent *ServerAddr = gethostbyname("testapp1.local");

    struct sockaddr_in ServerIP;
    ServerIP.sin_family = AF_INET;
    ServerIP.sin_port = htons(PortNr);
    ServerIP.sin_addr.s_addr = inet_addr("127.0.0.1");

    int rc = connect(Socket, (struct sockaddr*)&ServerIP, sizeof(ServerIP));
    if (rc < 0) { cout << strerror(errno) << endl; }

    struct timespec StartNanosec;
    struct timespec EndNanosec;

    clock_gettime(CLOCK_REALTIME, &StartNanosec);

    rc = write(Socket, payload.c_str(), strlen(payload.c_str()));
    cout << "Write rc:" << rc << endl;

    char Buffer[4096];
    string Result;

    int ReadBytes;
    while (1) {
        ReadBytes = read(Socket, Buffer, 4096);
        cout << "ReadBytes:" << ReadBytes << endl;
        if (ReadBytes < 0)  { cout << strerror(errno) << endl; }
        if (ReadBytes == 0) { break; }
        if (ReadBytes > 0)  { Result.append(*Buffer, ReadBytes); }
    }

    //cout << "Finish read()." << endl;

    shutdown(Socket, 1);
    read(Socket, Buffer, 4096);
    close(Socket);

    clock_gettime(CLOCK_REALTIME, &EndNanosec);

    cout << "Send/receive time in nanoseconds:" << (EndNanosec.tv_nsec-StartNanosec.tv_nsec) << endl;
}
