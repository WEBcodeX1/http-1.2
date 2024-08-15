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
        _Requests.at(ClientFD).insert(
            PairReqNrProps_t(ReqNr, ReqProps)
        );
    }
    else {
        MapReqNrProps_t ReqNrProps;
        ReqNrProps.insert(PairReqNrProps_t(ReqNr, ReqProps));
        _Requests.insert(
            PairRequests_t(ClientFD, ReqNrProps)
        );
    }
}

void ResultOrder::_processClients()
{
    for (auto& [ClientFD, LastReq]: _Requests) {
        if (!_LastRequest.contains(ClientFD)) {
            _LastRequest.insert(PairLastRequest_t(ClientFD, { 1, chrono::system_clock::to_time_t (chrono::system_clock::now()) }));
        }
    }
}

int main(int argc, char *argv[])
{
    ResultOrder o = ResultOrder();
    o.append(20,  5, {0, 100, 0});
    o.append(20,  7, {0, 102, 0 });
    o.append(20, 10, {0, 105, 0});
    return 0;
}
