#ifndef Global_hpp
#define Global_hpp

#include <csignal>
#include <string>
#include <unordered_map>
#include <vector>
#include <exception>
#include <cstdlib>

#include <sys/syscall.h>
#include <unistd.h>
#include <execinfo.h>

#include "Client.hpp"
#include "Debug.cpp"

using namespace std;


typedef unsigned int Filedescriptor_t;
typedef unsigned int pidfd_t;

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


class Permission {

public:

    static void dropPrivileges(uint GroupID, uint UserID)
    {
        //- in case of being root, drop privileges
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
        //static bool tried_throw = false;

        try {
            // try once to re-throw currently active exception
            //if (!tried_throw++) throw;
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
