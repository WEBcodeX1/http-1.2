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

void ThreadHandler::_addRequests(
    ClientRequestDataVec_t Requests
)
{
    _RequestsSorted.clear();

    for (const auto& RequestProps : Requests) {
        if (_RequestsSorted.contains(RequestProps.ClientFD)) {
            _RequestsSorted.at(RequestProps.ClientFD).push_back(std::move(RequestProps));
        }
        else {
            ClientRequestDataVec_t ReqPropsVec;
            ReqPropsVec.push_back(std::move(RequestProps));
            _RequestsSorted.emplace(
                RequestProps.ClientFD, std::move(ReqPropsVec)
            );
        }
    }

    for (const auto& RequestPropsPair : _RequestsSorted) {
        _ProcessRequests.push_back(std::move(RequestPropsPair));
    }
}

void ThreadHandler::_checkProcessed()
{
    //- check thread(s) are joinable
    for (auto it=_ClientThreads.cbegin(); it!=_ClientThreads.cend();) {
        if (it->second->join()) {
            DBG(120, "Joined Thread for ClientFD:" << it->first << ", removing from Map");

            _ProcessRequests.erase(
                _ProcessRequests.begin()+_ProcessRequestsIndex.at(it->first)
            );

            _ProcessRequestsIndex.erase(it->first);
            _ClientThreads.erase(it++);
        }
        else {++it;};
    }
}

void ThreadHandler::_processThreads()
{
    PocessRequestsIndex_t i = 0;
    for (auto& [ClientFD, RequestProps] : _ProcessRequests) {

        //- if thread still in progress, do nothing
        if (_ClientThreads.contains(ClientFD)) {
            DBG(200, "_ClientThreads contain ClientFD:" << ClientFD);
            continue;
        }

        ClientThreadObjRef_t ClientThreadObj(
            new ClientThread(ClientFD, _Globals.Namespaces, std::move(RequestProps))
        );

        _ProcessRequestsIndex.emplace(
            ClientFD, i
        );

        _ClientThreads.emplace(
            ClientFD, std::move(ClientThreadObj)
        );
        ClientThreadObj->startThread();
        ++i;
    }
    _checkProcessed();
}


ClientThread::ClientThread(
    ClientFD_t ClientFD,
    Namespaces_t &Namespaces,
    ClientRequestDataVec_t Requests
) :
    HTTPParser(ClientFD, Namespaces),
    _ClientFD(ClientFD),
    _Namespaces(Namespaces),
    _ClientRequests(Requests)
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

void ClientThread::startThread()
{
    DBG(80, "Start proccessing thread");
    _ThreadRef = thread(&ClientThread::processRequests, this);
}

void ClientThread::processRequests()
{
    for (unsigned long i=0; i<_ClientRequests.size(); ++i) {

        if (_ClientRequests[i].ASIndex == -1)
        {
            /*
            struct timespec StartNanoseconds;
            clock_gettime(CLOCK_REALTIME, &StartNanoseconds);
            DBG(80, "Processing Request inside Thread with Index:'" << i << "' StartNanoseconds:" << StartNanoseconds.tv_nsec);
            */

            BasePropsResult_t BaseProps;
            _parseRequestProperties(_ClientRequests[i].HTTPPayload, BaseProps);

            RequestHeaderResult_t Headers;
            _parseRequestHeaders(_ClientRequests[i].HTTPPayload, Headers);

            DBG(120, "RequestType:'" << BaseProps.at(2) << "'");
            DBG(120, "RequestPath:'" << BaseProps.at(1) << "'");
            DBG(120, "HTTPVersion:'" << BaseProps.at(0) << "'");

            const string NamespaceID = Headers.at("Host");
            DBG(120, "NamespaceID:'" << NamespaceID << "'");

            FileProperties_t FileProps;

            stringstream current_date;
            std::time_t tt = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now());
            struct std::tm * ptm = std::localtime(&tt);
            current_date << std::put_time(ptm, "%a, %d %b %Y %T") << '\n';

            /*
            for (const auto &Namespace:_Namespaces) {
                DBG(120, "ThreadHandler NamespaceID:'" << Namespace.first << "' PathRel:'" << Namespace.second.PathRel << "'");
            }
            */

            NamespaceProps_t NamespaceProps = _Namespaces.at(NamespaceID);

            DBG(120, "NamespacePath:'" << NamespaceProps.FilesystemRef->Path << "'");
            DBG(120, "NamespaceBasePath:'" << NamespaceProps.FilesystemRef->BasePath << "'");

            if (NamespaceProps.FilesystemRef->checkFileExists(BaseProps.at(1))) {
                FileProps = NamespaceProps.FilesystemRef->getFilePropertiesByFile(BaseProps.at(1));

                //DBG(80, "ParentPidFD:" << _Globals.ParentPidFD);
                DBG(80, "ClientFileDescriptor:" << _ClientRequests[i].ClientFD);
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

                DBG(120, "ClientFD:" << _ClientRequests[i].ClientFD << " ClientFDShared:" << _ClientRequests[i].ClientFDShared << " StaticFileFD:" << FileProps.Filedescriptor);

                int r = write(_ClientRequests[i].ClientFDShared, send_buf, strlen(send_buf));

                if (r < 0) { DBG(50, "write() err:" << strerror(errno)); }

                lseek(FileProps.Filedescriptor, 0, SEEK_SET);

                int SentBytes = 0;
                unsigned int SumBytes = 0;

                while (1) {
                    SentBytes = sendfile(_ClientRequests[i].ClientFDShared, FileProps.Filedescriptor, 0, FileProps.FileSize);
                    if (SentBytes < 0) {
                        uint8_t errsv = errno;
                        DBG(300, "sendfile() err:" << strerror(errsv));
                        if (errsv != EAGAIN) { break; }
                    }
                    else {
                        SumBytes += SentBytes;
                    }
                    if (SumBytes == FileProps.FileSize) { break; }
                }
                DBG(300, "sendfile() wrote bytes:" << SumBytes);
            }
            else {
                string Response = "HTTP/1.1 404 Not Found\n";
                Response.append("Date: ");
                Response.append(current_date.str());
                Response.append(" GMT\n");
                Response.append("Server: falcon/0.1\n");
                Response.append("Content-Length: 26\n");
                Response.append("Content-Type: text/html\n\n");
                Response.append("<html>404 Not Found</html>");

                const char* send_buf = Response.c_str();

                DBG(120, "ClientFD:" << _ClientRequests[i].ClientFD << " ClientFDShared:" << _ClientRequests[i].ClientFDShared << " StaticFileFD:" << FileProps.Filedescriptor);

                int res1 = write(_ClientRequests[i].ClientFDShared, send_buf, strlen(send_buf));
            }

            /*
            struct timespec EndNanoseconds;
            clock_gettime(CLOCK_REALTIME, &EndNanoseconds);

            uint64_t DurationNanoseconds = EndNanoseconds.tv_nsec - StartNanoseconds.tv_nsec;
            DBG(80, "Thread processing took Nanoseconds:" << DurationNanoseconds);
            */
        }

        if (_ClientRequests[i].ASIndex >= 0)
        {
            stringstream current_date;
            std::time_t tt = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now());
            struct std::tm * ptm = std::localtime(&tt);
            current_date << std::put_time(ptm, "%a, %d %b %Y %T") << '\n';

            string Response = "HTTP/1.1 200 OK\n";
            Response.append("Date: ");
            Response.append(current_date.str());
            Response.append(" GMT\n");
            Response.append("Server: falcon/0.1\n");
            Response.append("Content-Length: " + to_string(_ClientRequests[i].PayloadLength) + "\n");
            Response.append("Content-Type: application/json\n\n");
            Response.append(_ClientRequests[i].HTTPPayload);

            const char* send_buf = Response.c_str();

            int r = write(_ClientRequests[i].ClientFDShared, send_buf, strlen(send_buf));
        }
    }
}
