#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <boost/regex.hpp>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( test_regex_split )
{

    cout << "Check multiple Boost Regex variants." << endl;

    /*
        //- does not work, c++20 does not implement list.push_back()
        string Data("Test String \n\r Hello World\n\r Bla Bla");
        list<string> ResultList;
        list<string>::iterator ListIter;
        regex regexString{"\n\r"};
        regex_split(back_inserter(ListIter), Data, regexString);
    */

}
