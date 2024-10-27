#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "test-result-order-class.cpp"


BOOST_AUTO_TEST_CASE( test_basic_non_ordered_http12 )
{
    ResultOrder o = ResultOrder();
    o.append(20,  5, {2, 100, 0});
    o.append(30, 20, {1, 300, 0});
    o.append(20,  7, {2, 102, 0});
    o.append(20, 10, {2, 105, 0});

    ResultPropsList_t TestData = o.getNextRequests(20, 2);
    for (auto& Data : TestData) {
        cout << "SendfileFD:" << Data.SendfileFD << " ASIndex:" << Data.ASIndex << endl;
    }

    BOOST_TEST(TestData[0].SendfileFD == 100);
    BOOST_TEST(TestData[1].SendfileFD == 102);
    BOOST_TEST(TestData[2].SendfileFD == 105);

}

BOOST_AUTO_TEST_CASE( test_basic_ordered_http11 )
{
    ResultOrder o = ResultOrder();
    o.append(20,  1, {1, 100, 0});
    o.append(20,  2, {1, 300, 0});
    o.append(20,  3, {1, 102, 0 });
    o.append(20,  5, {1, 105, 0});

    o.processClients();

    ResultPropsList_t TestData = o.getNextRequests(20, 1);
    for (auto& Data : TestData) {
        cout << "SendfileFD:" << Data.SendfileFD << " ASIndex:" << Data.ASIndex << endl;
    }

    BOOST_TEST(TestData[0].SendfileFD == 100);
    BOOST_TEST(TestData[1].SendfileFD == 300);
    BOOST_TEST(TestData[2].SendfileFD == 102);

}

BOOST_AUTO_TEST_CASE( test_unordered_http11_1 )
{
    ResultOrder o = ResultOrder();
    o.append(20,  10, {1, 100, 0});
    o.append(20,  5, {1, 300, 0});
    o.append(20,  3, {1, 102, 0 });
    o.append(20,  8, {1, 105, 0});
    o.append(20,  2, {1, 106, 0});
    o.append(20,  9, {1, 107, 0});
    o.append(20,  1, {1, 108, 0});
    o.append(20,  8, {1, 109, 0});
    o.append(20,  20, {1, 110, 0});
    o.append(20,  21, {1, 111, 0});
    o.append(20,  4, {1, 112, 0});
    o.append(20,  13, {1, 113, 0});

    o.processClients();

    ResultPropsList_t TestData = o.getNextRequests(20, 1);
    for (auto& Data : TestData) {
        cout << "SendfileFD:" << Data.SendfileFD << " ASIndex:" << Data.ASIndex << endl;
    }

    BOOST_TEST(TestData[0].SendfileFD == 108);
    BOOST_TEST(TestData[1].SendfileFD == 106);
    BOOST_TEST(TestData[2].SendfileFD == 102);
    BOOST_TEST(TestData[3].SendfileFD == 112);
    BOOST_TEST(TestData[4].SendfileFD == 300);
}
