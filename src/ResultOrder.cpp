#include "ResultOrder.hpp"


using namespace std;

ResultOrder::ResultOrder()
{
}

ResultOrder::~ResultOrder()
{
}

void ResultOrder::reset()
{
    _Requests.clear();
    _LastRequest.clear();
}

void ResultOrder::append(
    const RequestNr_t ReqNr,
    RequestProps_t ReqProps
)
{
    if (_Requests.contains(ReqProps.ClientFD)) {
        _Requests.at(ReqProps.ClientFD).emplace(
            ReqNr, std::move(ReqProps)
        );
    }
    else {
        MapReqNrProps_t ReqNrProps;
        ReqNrProps.emplace(ReqNr, ReqProps);
        _Requests.emplace(
            ReqProps.ClientFD, std::move(ReqNrProps)
        );
    }
}

void ResultOrder::calculate()
{
    for (const auto& [ClientFD, Requests]: _Requests) {
        if (!_LastRequest.contains(ClientFD)) {
            _LastRequest.emplace(ClientFD, LastRequestProps_t{ 1, chrono::system_clock::to_time_t (chrono::system_clock::now()) });
        }
    }
}

void ResultOrder::processRequests(const HTTPType_t HTTPType)
{
    //- for test return ResultPropsList_t, prod: "add" ResultProps_t directly to ThreadHandler

    ClientRequestDataVec_t ResultData;

    for (const auto& [ClientFD, Requests]: _Requests) {

        DBG(200, "Loop ClientFD:" << ClientFD << " Check HTTPType:" << HTTPType);

        //- HTTP/1.2 (unordered)
        if (HTTPType == HTTP1_2) {
            for (const auto& [ReqNr, ReqProps]: _Requests.at(ClientFD)) {
                if (ReqProps.HTTPVersion == HTTP1_2) {
                    DBG(120, "HTTP1_2 RequestProps ClientFD:" << ReqProps.ClientFD << " HTTPType:" << ReqProps.HTTPVersion);
                    ResultData.push_back(ReqProps);
                }
            }
        }

        //- HTTP/1.1 (ordered)
        if (HTTPType == HTTP1_1) {

            RequestNr_t LastRequestNrCheck = _LastRequest.at(ClientFD).RequestNr;
            RequestNr_t ReqIndex = 0;

            DBG(200, "Processing HTTP1.1 LastReqNrCheck:" << LastRequestNrCheck);

            for (const auto& [ReqNr, ReqProps]: _Requests.at(ClientFD)) {

                DBG(200, "ReqIndex:" << ReqIndex << " ReqNr:" << ReqNr);

                //- if first ReqNr is not next (LastRequest+1) do not process
                if (ReqProps.HTTPVersion == HTTP1_1 && ReqIndex == 0 && ReqNr != LastRequestNrCheck) { break; }
                if (ReqProps.HTTPVersion == HTTP1_1 && ReqNr == LastRequestNrCheck) {
                    DBG(200, "HTTP1_1 RequestProps ClientFD:" << ReqProps.ClientFD << " HTTPType:" << ReqProps.HTTPVersion);
                    ResultData.push_back(ReqProps);
                }
                LastRequestNrCheck += 1;
                ReqIndex += 1;
            }
        }
    }
    ThreadHandler::_addRequests(std::move(ResultData));
}

/*
//void ResultOrder::THsetGlobalData(pidfd_t ParentPidFD, Namespaces_t Namespaces)
void ResultOrder::THsetGlobalData(pidfd_t ParentPidFD)
{
    //ThreadHandler::_setGlobalData(ParentPidFD, Namespaces);
    //ThreadHandler::_setGlobalData(ParentPidFD);
}
*/

void ResultOrder::THprocessThreads()
{
    ThreadHandler::_processThreads();
}
