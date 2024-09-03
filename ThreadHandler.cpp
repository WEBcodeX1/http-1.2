#include "ThreadHandler.hpp"

using namespace std;

ThreadHandler::ThreadHandler()
{
    DBG(120, "Constructor");
}

ThreadHandler::~ThreadHandler()
{
    DBG(120, "Destructor");
}

void ThreadHandler::_setGlobalData(pidfd_t ParentPidFD, Namespaces_t Namespaces)
{
    _Globals.ParentPidFD = ParentPidFD;
    _Globals.Namespaces = Namespaces;
}

void ThreadHandler::_addClient(ClientRequestData_t ReqData)
{
    //- if clientfd key exists, push to req vector
    if (_ClientRequests.contains(ReqData.ClientFD)) {
        _ClientRequests.at(ReqData.ClientFD).push_back(ReqData);
    }
    else {
        vector<ClientRequestData_t> ReqDataV{ReqData};
        _ClientRequests.insert(
             ClientRequestDataPair_t(ReqData.ClientFD, ReqDataV)
        );
    }
}

void ThreadHandler::_checkProcessed()
{
    //- check thread(s) are joinable
    for (auto it=_ClientThreads.cbegin(); it!=_ClientThreads.cend();) {
        if (it->second->join()) {
            DBG(120, "Joined Thread for ClientFD:" << it->first << ", removing from Map");
            _ClientRequests.erase(it->first);
            _ClientThreads.erase(it++);
        }
        else {++it;};
    }
}

void ThreadHandler::_startProcessingThreads()
{
    for (const auto& [ClientFD, ReqData] : _ClientRequests) {

        //- new requests could be passed while thread still active
        //- check if fd already exists, yes: return
        if (_ClientThreads.contains(ClientFD)) {
            DBG(120, "_ClientThreads contain ClientFD:" << ClientFD);
            return;
        }

        ClientThreadObjRef_t ClientThreadObj(new ClientThread(ReqData, _Globals));

        _ClientThreads.insert(
            ClientListPair_t(ClientFD, std::move(ClientThreadObj))
        );

        _ClientRequests.at(ClientFD).clear();

        ClientThreadObj->startPocessingThread();
    }
}


ClientThread::ClientThread(ClientRequestDataVec_t Requests, ThreadHandlerGlobals_t Globals) :
    HTTPParser(Requests[0].ClientFD),
    _ClientRequests(Requests),
    _Globals(Globals)
{
    DBG(120, "Constructor");
}

ClientThread::~ClientThread()
{
    DBG(120, "Destructor");
}

bool ClientThread::join()
{
    if (_ThreadRef.joinable()) {
        _ThreadRef.join();
        return true;
    }
    else {
        return false;
    }
}

void ClientThread::startPocessingThread()
{
    DBG(80, "Start proccessing thread");
    _ThreadRef = thread(&ClientThread::processRequests, this);
}

void ClientThread::processRequests()
{
    for (unsigned int i=0; i<_ClientRequests.size(); ++i) {
        this->processRequest(i);
    }
}

void ClientThread::processRequest(unsigned int Index)
{
    struct timespec StartNanoseconds;
    clock_gettime(CLOCK_REALTIME, &StartNanoseconds);

    DBG(80, "Processing Request inside Thread with Index:'" << Index << "' StartNanoseconds:" << StartNanoseconds.tv_nsec);
    BasePropsResult_t BaseProps;
    _parseBasePropsRV(_ClientRequests[Index].HTTPMessage, BaseProps);

    DBG(120, "RequestType:'" << BaseProps.at(2) << "'");
    DBG(120, "RequestPath:'" << BaseProps.at(1) << "'");
    DBG(120, "HTTPVersion:'" << BaseProps.at(0) << "'");

    RequestHeaderResult_t Headers;
    _parseHeadersRV(_ClientRequests[Index].HTTPMessage, Headers);

    const string NamespaceID = Headers.at("Host");
    DBG(120, "NamespaceID:'" << NamespaceID << "'");

    FileProperties_t FileProps;
    NamespaceProps_t NamespaceProps = _Globals.Namespaces.at(NamespaceID);

    DBG(120, "NamespacePath:'" << NamespaceProps.FilesystemRef->Path << "'");
    DBG(120, "NamespaceBasePath:'" << NamespaceProps.FilesystemRef->BasePath << "'");

    stringstream current_date;
    std::time_t tt = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now());
    struct std::tm * ptm = std::localtime(&tt);
    current_date << std::put_time(ptm, "%a, %d %b %Y %T") << '\n';

    try {
        FileProps = NamespaceProps.FilesystemRef->getFilePropertiesByFile(BaseProps.at(1));

        DBG(80, "ParentPidFD:" << _Globals.ParentPidFD);
        DBG(80, "ClientFileDescriptor:" << _ClientRequests[Index].ClientFD);
        DBG(80, "FileDescriptor:" << FileProps.Filedescriptor);
        DBG(80, "FileSize:" << FileProps.FileSize);
        DBG(80, "MimeType:" << FileProps.MimeType);

        string Response = "HTTP/1.1 200 OK\n";
        Response.append("Date: ");
        Response.append(current_date.str());
        Response.append(" GMT\n");
        Response.append("Server: falcon/0.1\n");
        Response.append("Content-Length: " + to_string(FileProps.FileSize) + "\n");
        Response.append("Content-Type: " + FileProps.MimeType + "\n\n");

        const char* send_buf = Response.c_str();

        DBG(120, "ClientFD:" << _ClientRequests[Index].ClientFD << " ClientFDShared:" << _ClientRequests[Index].ClientFDShared << " StaticFileFD:" << FileProps.Filedescriptor);

        int res1 = write(_ClientRequests[Index].ClientFDShared, send_buf, strlen(send_buf));

        if (res1 < 0) { DBG(50, "write() err:" << strerror(errno)); }

        lseek(FileProps.Filedescriptor, 0, SEEK_SET);

        int SentBytes = 0;
        unsigned int SumBytes = 0;

        while (1) {
            SentBytes = sendfile(_ClientRequests[Index].ClientFDShared, FileProps.Filedescriptor, 0, FileProps.FileSize);
            if (SentBytes < 0) {
                uint8_t errsv = errno;
                DBG(50, "sendfile() err:" << strerror(errsv));
                if (errsv != EAGAIN) { break; }
            }
            else {
                SumBytes += SentBytes;
            }
            if (SumBytes == FileProps.FileSize) { break; }
        }
        DBG(50, "sendfile() wrote bytes:" << SumBytes);
    }
    catch (exception& e) {
        string Response = "HTTP/1.1 404 Not Found\n";
        Response.append("Date: ");
        Response.append(current_date.str());
        Response.append(" GMT\n");
        Response.append("Server: falcon/0.1\n");
        Response.append("Content-Length: 26\n");
        Response.append("Content-Type: text/html\n\n");
        Response.append("<html>404 Not Found</html>");

        const char* send_buf = Response.c_str();

        DBG(120, "ClientFD:" << _ClientRequests[Index].ClientFD << " ClientFDShared:" << _ClientRequests[Index].ClientFDShared << " StaticFileFD:" << FileProps.Filedescriptor);

        int res1 = write(_ClientRequests[Index].ClientFDShared, send_buf, strlen(send_buf));
    }

    struct timespec EndNanoseconds;
    clock_gettime(CLOCK_REALTIME, &EndNanoseconds);

    uint64_t DurationNanoseconds = EndNanoseconds.tv_nsec - StartNanoseconds.tv_nsec;
    DBG(80, "Thread processing took Nanoseconds:" << DurationNanoseconds);

}
