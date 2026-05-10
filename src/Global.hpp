#pragma once

#include <csignal>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <exception>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <errno.h>

#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <execinfo.h>

#include "Client.hpp"
#include "Debug.cpp"

using namespace std;


typedef int Filedescriptor_t;


class Signal {

public:

    // disable unwanted signal handlers (SIGINT, SIGPIPE)
    static void disableSignals()
    {
        signal(SIGINT,  SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
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
