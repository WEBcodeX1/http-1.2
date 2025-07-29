#ifndef Helper_hpp
#define Helper_hpp

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

    //- TODO: make erase optional, ugly
    static void split(string& String, const string Delimiter, vector<string>& ResultRef)
    {
        size_t FindPos = 0;
        string Token;
        while ((FindPos = String.find(Delimiter)) != String.npos) {
            Token = String.substr(0, FindPos);
            ResultRef.push_back(Token);
            String.erase(0, FindPos + Delimiter.length());
        }
        DBG(200, "String:'" << String << "'");
    }

    //- TODO: ugly, refactor (lambda?)
    static void rsplit(string& String, size_t StartPos, const string Delimiter, vector<string>& ResultRef)
    {
        size_t FindPos = 0;
        size_t FindPosLast = 0;
        string Token;
        StartPos -= Delimiter.length();
        while ((FindPos = String.rfind(Delimiter, StartPos)) != String.npos) {
            DBG(200, "FindPos:" << FindPos << " StartPos:" << StartPos);
            Token = String.substr(FindPos+Delimiter.length(), (StartPos-FindPos));
            DBG(200, "Token:" << Token);
            ResultRef.push_back(Token);
            StartPos = FindPos - Delimiter.length();
            FindPosLast = FindPos;
        }
        DBG(200, "End FindPos:" << FindPosLast);
        Token = String.substr(0, FindPosLast);
        ResultRef.push_back(Token);
    }

    static void hexout(string& String)
    {
        for (auto i = String.begin(); i != String.end(); ++i) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*i) << ' ';
        }
        std::cout << endl;
    }
};

#endif
