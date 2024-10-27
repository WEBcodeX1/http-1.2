#include "Server.hpp"

using namespace std;

namespace bp = boost::python;

int main()
{
    cout << "### >> ---------------------------------------------------------------" << endl;
    cout << "### >> FalconAS Server Version " << SERVER_VERSION << endl;
    cout << "### >> ---------------------------------------------------------------" << endl;

    //- init server object
    Server ServerObj;
    ServerObj.init();

    return 0;
}
