#ifndef Global_hpp
#define Global_hpp

#include <csignal>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <exception>
#include <cstdlib>

#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <execinfo.h>

#include "Client.hpp"
#include "Debug.cpp"

using namespace std;


typedef unsigned int Filedescriptor_t;

typedef uint16_t HTTPVersionType_t;
typedef uint16_t HTTPMethodType_t;
typedef uint16_t HTTPReqNrType_t;
typedef uint32_t HTTPPayloadLength_t;
typedef int16_t ASIndex_t;
typedef uint16_t RequestNr_t;
typedef uint32_t RequestType_t;

enum HTTPType_t {
    HTTP1_1 = 1,
    HTTP1_2 = 2
};

//- Requests
typedef struct {
    RequestType_t RequestType;
    HTTPVersionType_t HTTPVersion;
    HTTPMethodType_t HTTPMethod;
    ClientFD_t ClientFD;
    ClientFD_t ClientFDShared;
    string HTTPPayload;
    HTTPPayloadLength_t PayloadLength;
    ASIndex_t ASIndex;
} RequestProps_t;

typedef vector<RequestProps_t> RequestPropsList_t;


class Signal {

public:

    // disable unwanted signal handlers (SIGINT, SIGPIPE)
    static void disableSignals()
    {
        signal(SIGINT,  SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
    }

};


class Syscall {

public:

    // unix domain socket for FD passing - server side (parent process)
    static int createFDPassingServer(const char* socket_path)
    {
        int server_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (server_fd < 0) {
            return -1;
        }

        // remove any existing socket file
        unlink(socket_path);

        struct sockaddr_un server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            close(server_fd);
            return -1;
        }

        if (listen(server_fd, 50) < 0) {
            close(server_fd);
            return -1;
        }

        return server_fd;
    }

    // unix domain socket for FD passing - client side (child process)
    static int connectFDPassingClient(const char* socket_path)
    {
        int client_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (client_fd < 0) {
            return -1;
        }

        struct sockaddr_un server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

        if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            close(client_fd);
            return -1;
        }

        return client_fd;
    }

    // send a file descriptor over Unix domain socket
    static int sendFD(int socket_fd, int fd_to_send)
    {
        struct msghdr msg;
        struct iovec iov[1];
        char ctrl_buf[CMSG_SPACE(sizeof(int))];
        char data[1] = {0};

        memset(&msg, 0, sizeof(msg));
        memset(ctrl_buf, 0, sizeof(ctrl_buf));

        // Setup iovec for dummy data
        iov[0].iov_base = data;
        iov[0].iov_len = sizeof(data);

        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_control = ctrl_buf;
        msg.msg_controllen = sizeof(ctrl_buf);

        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        *((int*)CMSG_DATA(cmsg)) = fd_to_send;

        if (sendmsg(socket_fd, &msg, 0) < 0) {
            return -1;
        }

        return 0;
    }

    // receive a file descriptor over Unix domain socket
    static int recvFD(int socket_fd)
    {
        struct msghdr msg;
        struct iovec iov[1];
        char ctrl_buf[CMSG_SPACE(sizeof(int))];
        char data[1];

        memset(&msg, 0, sizeof(msg));
        memset(ctrl_buf, 0, sizeof(ctrl_buf));

        iov[0].iov_base = data;
        iov[0].iov_len = sizeof(data);

        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_control = ctrl_buf;
        msg.msg_controllen = sizeof(ctrl_buf);

        if (recvmsg(socket_fd, &msg, 0) < 0) {
            return -1;
        }

        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg == NULL || cmsg->cmsg_type != SCM_RIGHTS) {
            return -1;
        }

        return *((int*)CMSG_DATA(cmsg));
    }

};


class Permission {

public:

    static void dropPrivileges(uint GroupID, uint UserID)
    {
        // in case of being root, drop privileges
        if (getuid() == 0) {

            if (setgid(GroupID) != 0) {
                DBG(10, "Failed setting groupid to:" << GroupID);
                exit(EXIT_FAILURE);
            }

            if (setuid(UserID) != 0) {
                DBG(10, "Failed setting userid to:" << UserID);
                exit(EXIT_FAILURE);
            }

        }
        else {
            DBG(10, "Error. Starting server as root user is recommended.");
            exit(EXIT_FAILURE);
        }
    }
};


class SigHandler {

public:

    #if defined(DEBUG_BUILD)
    static void myterminate()
    {
        try {
            // re-throw currently active exception
            throw;
        }
        catch (const std::exception &e) {
            std::cerr << __FUNCTION__ << " caught unhandled exception. what(): " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << __FUNCTION__ << " caught unknown/unhandled exception." << std::endl;
        }

        void * array[50];
        int size = backtrace(array, 50);

        std::cerr << __FUNCTION__ << " backtrace returned " << size << " frames\n\n";

        char ** messages = backtrace_symbols(array, size);

        for (int i = 0; i < size && messages != NULL; ++i) {
            std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
        }
        std::cerr << std::endl;

        free(messages);

        abort();
    }
    #endif
};

#endif
