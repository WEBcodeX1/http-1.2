#ifndef Filesystem_hpp
#define Filesystem_hpp

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <vector>

#include "Debug.cpp"
#include "Helper.hpp"
#include "Global.hpp"


typedef struct {
    Filedescriptor_t Filedescriptor;
    unsigned int FileSize;
    string FileName;
    string FileExtension;
    string MimeType;
    string ETag;
    string LastModifiedString;
    string LastModifiedSeconds;
} FileProperties_t;

typedef unordered_map<string, const string> MimetypeRelations_t;

typedef unsigned int Filedescriptor_t;
typedef vector<string> FilelistPlain_t;
typedef pair<string, FileProperties_t> FileListExtendedPair_t;
typedef unordered_map<string, const FileProperties_t> FileListExtended_t;

static const MimetypeRelations_t MimeRelations{
    { "html", "text/html" },
    { "js", "text/javascript" },
    { "json", "application/json" },
    { "css", "text/css" },
    { "png", "image/png" },
    { "svg", "image/svg+xml" },
    { "woff", "font/woff" },
    { "woff2", "font/woff2" }
};


class Filesystem
{

public:

    Filesystem();
    ~Filesystem();

    void initFiles();
    void processFileProperties();
    FileProperties_t getFilePropertiesByFile(string File);

    string Hostname;
    string BasePath;
    string Path;

    vector<string> Mimetypes;

private:

    FilelistPlain_t _Files;
    FileListExtended_t _FilesExtended;
    string _CompletePath;
};

#endif
