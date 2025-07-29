#ifndef LibEvent_hpp
#define LibEvent_hpp

#include "../../src/Debug.cpp"

#include <memory>
#include <string>
#include <functional>


class Event
{

public:

    Event();
    ~Event();

    void registerEvent(
            const bool(*FPCheckFunction)(),
            void(*FPCallbackFunction)()
    );

    const bool checkEvent();
    void callCallback();
    void checkEventProcessCallback();

protected:

    function<const bool()> CheckFunctionRef;
    function<void()> CallbackFunctionRef;

};

#endif
