#include "test-result-order.hpp"


using namespace std;

ResultOrder::ResultOrder()
{
}

ResultOrder::~ResultOrder()
{
}

void ResultOrder::append(const ClientFD_t ClientFD, const RequestNr_t ReqNr, const RequestProps_t ReqProps)
{
    if (_Requests.contains(ClientFD)) {
        _Requests.at(ClientFD).emplace(
            ReqNr, ReqProps
        );
    }
    else {
        MapReqNrProps_t ReqNrProps;
        ReqNrProps.emplace(ReqNr, ReqProps);
        _Requests.emplace(
            ClientFD, ReqNrProps
        );
    }
}

void ResultOrder::processClients()
{
    for (auto& [ClientFD, Requests]: _Requests) {
        if (!_LastRequest.contains(ClientFD)) {
            _LastRequest.emplace(ClientFD, LastRequestProps_t{ 1, chrono::system_clock::to_time_t (chrono::system_clock::now()) });
        }
    }
}

ResultPropsList_t ResultOrder::getNextRequests(const ClientFD_t ClientFD, const HTTPType_t HTTPType)
{
    //- for test return ResultPropsList_t, prod: "add" ResultProps_t directly to ThreadHandler

    ResultPropsList_t ReturnData;

    //- HTTP/1.2 (unordered)
    if (HTTPType == 2) {
        for (auto& [ReqNr, ReqProps]: _Requests.at(ClientFD)) {
            if (ReqProps.HTTPType == 2) {
                ReturnData.push_back( { ReqProps.SendfileFD, ReqProps.ASIndex} );
            }
        }
        return ReturnData;
    }

    //- HTTP/1.1 (ordered)
    if (HTTPType == 1) {
        RequestNr_t LastRequestNrCheck = _LastRequest.at(ClientFD).RequestNr;
        RequestNr_t ReqIndex = 0;
        for (auto& [ReqNr, ReqProps]: _Requests.at(ClientFD)) {
            //- if first ReqNr is not next (LastRequest+1) do not process
            if (ReqProps.HTTPType == 1 && ReqIndex == 0 && ReqNr != LastRequestNrCheck) { break; }
            if (ReqProps.HTTPType == 1 && ReqNr == LastRequestNrCheck) {
                ReturnData.push_back( { ReqProps.SendfileFD, ReqProps.ASIndex} );
            }
            LastRequestNrCheck += 1;
            ReqIndex += 1;
        }
    }
    return ReturnData;
}
