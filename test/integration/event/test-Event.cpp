#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <memory>
#include "TestClass.hpp"

BOOST_AUTO_TEST_CASE( test_event )
{
    auto pTest = std::make_unique<Callback>();
    pTest->checkEvent();
    pTest->callCallback();
}
