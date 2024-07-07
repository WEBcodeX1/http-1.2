#ifndef EventTest_hpp
#define EventTest_hpp

#include "../../../lib/event/Event.hpp"

using namespace std;

//- test class
class Callback : public Event
{

public:

    Callback();
    ~Callback();

    static const bool check_function();
    static void callback_function();

};

#endif
