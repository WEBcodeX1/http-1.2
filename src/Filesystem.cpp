#include "Filesystem.hpp"

using namespace std;


Filesystem::Filesystem()
{
    DBG(120, "Constructor");
}

Filesystem::~Filesystem()
{
    DBG(120, "Destructor");
}

void Filesystem::initFiles()
{
    _CompletePath = BasePath + Path + STATICFS_SUBDIR + "/";

    DBG(120, "Filesystem Host:" << Hostname << " Path:" << _CompletePath);

    for (const auto &Mimetype:Mimetypes) {
        DBG(120, "Filesystem processing Mimetype:" << Mimetype);
    }

    for (const auto &Mimetype:Mimetypes) {
        FilelistPlain_t TmpFiles;
        FilesystemHelper::GetDirListingByFiletype(
            TmpFiles,
            _CompletePath,
            "." + Mimetype
        );

        _Files.insert(
            _Files.end(),
            make_move_iterator(TmpFiles.begin()),
            make_move_iterator(TmpFiles.end())
        );
    }
}

void Filesystem::processFileProperties()
{
    for (const auto &File:_Files) {

        const char* Filename = File.c_str();
        struct stat Filestat;

        int fd = open(Filename, O_RDONLY, 0440);

        if (fd > 0) {
            fstat(fd, &Filestat);

            auto FileSize = Filestat.st_size;

            if (FileSize > 300000) {
                DBG(140, "Map File with MAP_HUGETLB set and madvise().");
                auto FileMemPtr = mmap(NULL, FileSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_HUGETLB, fd, 0);
                madvise(FileMemPtr, FileSize, MADV_HUGEPAGE);
            }
            else {
                mmap(NULL, FileSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);
            }

            size_t FindPos = File.rfind("/");
            string FileName = File.substr(FindPos+1, File.length()-FindPos);
            string FilePath = File.substr(0, FindPos);

            FindPos = FileName.rfind(".");
            string FileExtension = FileName.substr(FindPos+1, File.length()-FindPos);
            string MimeType = MimeRelations.at(FileExtension);
            string ReplacePath = BasePath + Path + "/static";
            string RelPath = FilePath.substr(ReplacePath.length(), FilePath.length()-ReplacePath.length());
            string FileListKey = RelPath + "/" + FileName;

            DBG(210, "FilePath:" << FilePath << " ReplacePath:" << ReplacePath << " FileListKey:" << FileListKey);
            DBG(210, "FileName:" << FileName << " FD:" << fd << " RelPath:" << RelPath << " Extension:" << FileExtension << " Mimetype:" << MimeType);

            FileProperties_t FileProps;
            FileProps.Filedescriptor = fd;
            FileProps.FileSize = FileSize;
            FileProps.FileName = FileName;
            FileProps.FileExtension = FileExtension;
            FileProps.MimeType = MimeType;
            FileProps.ETag = this->getFileEtag(File);

            _FilesExtended.insert(
                FileListExtendedPair_t(FileListKey, FileProps)
            );
        }
    }
}

bool Filesystem::checkFileExists(const string &File)
{
    if (_FilesExtended.find(File) == _FilesExtended.end()) {
        return false;
    }
    return true;
}

FileProperties_t Filesystem::getFilePropertiesByFile(const string &File)
{
    return _FilesExtended.at(File);
}

string Filesystem::getFileEtag(const string &File) {

    streampos FileSize;
    char* FileBuffer;
    size_t FileHashInt = 0;

    try {
        std::ifstream FStream(File, ios::in | ios::binary | ios::ate);

        FileSize = FStream.tellg();
        FileBuffer = new char[FileSize];
        FStream.seekg (0, ios::beg);
        FStream.read (FileBuffer, FileSize);
        FStream.close();

        FileHashInt = hash<string>{}(string(FileBuffer, FileSize));
        delete[] FileBuffer;
    }
    catch(const char* msg) {
        ERR("Etag generation error:" << msg);
        exit(1);
    }

    stringstream FileHash;
    FileHash << std::hex << FileHashInt;
    return FileHash.str();
}
