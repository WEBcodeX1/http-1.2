#ifndef Debug_cpp
#define Debug_cpp

#include <iostream>
#include <sstream>
#include <string>

#include "Constant.hpp"

using namespace std;

#define DBG(level, debugmsg) \
        if (LOG_LEVEL > level) { \
            cout << dec << __func__ << "(), " << __LINE__ << " - " << debugmsg << endl; \
        }

#define ERR(errmsg) \
        cerr << dec << __func__ << "(), " << __LINE__ << " - " << errmsg << endl; \

#endif
