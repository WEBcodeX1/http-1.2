#include "Event.hpp"

Event::Event()
{
    DBG(120, "Constructor");
}

Event::~Event()
{
    DBG(120, "Destructor");
}

void Event::registerEvent(
        const bool(*FPCheckFunction)(),
        void(*FPCallbackFunction)())
{
    CheckFunctionRef = bind(FPCheckFunction);
    CallbackFunctionRef = bind(FPCallbackFunction);
}

const bool Event::checkEvent()
{
    return CheckFunctionRef();
}

void Event::callCallback()
{
    CallbackFunctionRef();
}
