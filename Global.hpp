#ifndef Global_hpp
#define Global_hpp

#include <csignal>
#include <string>
#include <unordered_map>

#include <sys/syscall.h>
#include <unistd.h>

using namespace std;

typedef unsigned int Filedescriptor_t;
typedef unsigned int pidfd_t;


class Signal {

public:

    //- disable unwanted signal handlers (SIGINT, SIGPIPE)
    static void disableSignals()
    {
        signal(SIGINT,  SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
    }

};


class Syscall {

public:

    //- wrapper function for pidfd_open
    static int pidfd_open(pid_t pid, unsigned int flags)
    {
        return syscall(SYS_pidfd_open, pid, flags);
    }

    //- wrapper function for pidfd_getfd
    static int pidfd_getfd(int pidfd, int targetfd, unsigned int flags)
    {
        return syscall(SYS_pidfd_getfd, pidfd, targetfd, flags);
    }

};

#endif
