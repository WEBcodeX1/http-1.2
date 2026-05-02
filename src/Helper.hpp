#pragma once

#include "Debug.cpp"

#include <boost/filesystem.hpp>
#include <string.h>
#include <iomanip>

#include "fcntl.h"


using namespace std;

class Socket {

public:

    static void makeNonblocking(int fd)
    {
        int flags = fcntl(fd, F_GETFL, 0);

        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK ) < 0) {
            ERR("Error setting Socket flag O_NONBLOCKING.");
            exit(EXIT_FAILURE);
        }
    }

};


using namespace boost::filesystem;

class FilesystemHelper {

public:

    static void GetDirListingByFiletype(vector<string>& FileListRef, const string Path, const string FileType)
    {
        recursive_directory_iterator rdi(Path);
        recursive_directory_iterator end_rdi;

        for (; rdi != end_rdi; rdi++)
        {
            if (FileType.compare((*rdi).path().extension().string()) == 0)
            {
                FileListRef.push_back((*rdi).path().string());
                DBG(210, (*rdi).path().string());
            }
        }
    }

};


class String {

public:

    static void hexout(string& String)
    {
        for (auto i = String.begin(); i != String.end(); ++i) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*i) << ' ';
        }
        std::cout << endl;
    }
};
