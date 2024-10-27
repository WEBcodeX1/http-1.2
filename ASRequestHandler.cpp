#include "ASRequestHandler.hpp"

using namespace std;


ASRequestHandler::ASRequestHandler(Namespaces_t Namespaces, BaseAdresses_t BaseAddresses)
{
    DBG(120, "Constructor");
    _Namespaces = Namespaces;
    setBaseAddresses(BaseAddresses);
    _calculateOffsets();
    _resetSharedMem();
}

ASRequestHandler::~ASRequestHandler()
{
    DBG(120, "Destructor");
}

void ASRequestHandler::_calculateOffsets() {

    AppServerID_t OffsetStart = 0;
    AppServerID_t OffsetEnd = -1;

    for (const auto &Namespace:_Namespaces) {

        OffsetEnd += Namespace.second.InterpreterCount;

        _VHostOffsets.insert(
            PairVHostOffsets_t(Namespace.first, { OffsetStart, OffsetEnd } )
        );
        _VHostOffsetsPrecalc.insert(
            PairVHostOffsetsPrecalc_t(Namespace.first, {} )
        );

        OffsetStart += Namespace.second.InterpreterCount;

    }

    for (const auto &Offset:_VHostOffsets) {
        DBG(80, "Namespace:" << Offset.first << " Start:" << Offset.second.OffsetStart << " End:" << Offset.second.OffsetEnd );
    }

    for (const auto &Offset:_VHostOffsets) {
        uint16_t i = _VHostOffsets.at(Offset.first).OffsetStart;
        uint16_t n = _VHostOffsets.at(Offset.first).OffsetEnd;
        while(i <= n) {
            _VHostOffsetsPrecalc.at(Offset.first).push_back(i);
            ++i;
        }
    }

    for (const auto &Offset:_VHostOffsetsPrecalc) {
        for (const auto &Index:Offset.second) {
            DBG(80, "Namespace:" << Offset.first << " Index:" << Index);
       }
    }

}

void ASRequestHandler::_resetSharedMem() {
    for (const auto &Offset:_VHostOffsetsPrecalc) {
        for (const auto &Index:Offset.second) {
            DBG(80, "Namespace:" << Offset.first << " Reset@Index:" << Index);
            new(getMetaAddress(Index, 0)) uint16_t(0);
            new(getMetaAddress(Index, 1)) uint16_t(0);
       }
    }
}

void ASRequestHandler::addRequest(const ASRequestProps_t RequestProps) {
    _Queue.push_back(RequestProps);
}

AppServerID_t ASRequestHandler::_getNextFreeAppServerID(string VirtualHost) {
    if (_VHostOffsetsPrecalc.contains(VirtualHost)) {
        for (const auto &Index:_VHostOffsetsPrecalc.at(VirtualHost)) {
            DBG(180, "Namespace:" << VirtualHost << " Index:" << Index);
            if (!_Requests.contains(Index)) {
                DBG(140, "Requests does not contain Index, check SHM");
                atomic_uint16_t* CanReadAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 0));
                atomic_uint16_t* WriteReadyAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 1));
                if (*(CanReadAddr) == 0 && *(WriteReadyAddr) == 0) {
                    return Index+1;
                }
            }
        }
    }
    return 0;
}

uint16_t ASRequestHandler::processQueue() {
    //DBG(150, "Processing ASRequestQueue");

    uint QueueIndex = 0;
    vector<uint> EraseElements;

    for (const auto &QueueItem:_Queue) {
        //DBG(180, "ASRequestQueue Item ClientFD:" << QueueItem.ClientFD << " VHost:" << QueueItem.VirtualHost);
        const AppServerID_t AppServerID = _getNextFreeAppServerID(QueueItem.VirtualHost);
        if (AppServerID > 0) {

            DBG(140, "Writing SHM @AppServerID:" << AppServerID);
            void* PayloadAddr = getRequestAddress(AppServerID);

            uint16_t* ClientFDAddr = static_cast<uint16_t*>(getMetaAddress(AppServerID, 2));
            uint16_t* HTTPVersionAddr = static_cast<uint16_t*>(getMetaAddress(AppServerID, 3));
            uint16_t* HTTPMethodAddr = static_cast<uint16_t*>(getMetaAddress(AppServerID, 4));
            uint16_t* ReqNrAddr = static_cast<uint16_t*>(getMetaAddress(AppServerID, 5));
            uint16_t* PayloadLengthAddr = static_cast<uint16_t*>(getMetaAddress(AppServerID, 6));

            *(ClientFDAddr) = QueueItem.ClientFD;
            *(HTTPVersionAddr) = QueueItem.HTTPVersion;
            *(HTTPMethodAddr) = QueueItem.HTTPMethod;
            *(ReqNrAddr) = QueueItem.RequestNr;
            *(PayloadLengthAddr) = QueueItem.Payload.length();

            const char* PayloadChar = QueueItem.Payload.c_str();
            memcpy(PayloadAddr, &PayloadChar[0], QueueItem.Payload.length());

            atomic_uint16_t* CanReadAddr = static_cast<atomic_uint16_t*>(getMetaAddress(AppServerID, 0));
            DBG(140, "Writing at CanReadAddr:" << CanReadAddr);
            new(getMetaAddress(AppServerID, 0)) atomic_uint16_t(1);
            EraseElements.push_back(QueueIndex);
        }
        ++QueueIndex;
    }
    _Queue.erase(EraseElements);
    return 0;
}

VHostOffsetsPrecalc_t ASRequestHandler::getOffsetsPrecalc() {
    return _VHostOffsetsPrecalc;
}
