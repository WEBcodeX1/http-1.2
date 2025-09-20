#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <memory>
#include "TestClass.hpp"

BOOST_AUTO_TEST_CASE( test_event )
{
    Callback* pTest = new Callback();
    pTest->checkEvent();
    pTest->callCallback();
}
