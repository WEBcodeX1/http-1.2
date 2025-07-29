#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../../../src/Helper.hpp"


using namespace std;

BOOST_AUTO_TEST_CASE( test_string_split_1 )
{
    cout << "Check string split." << endl;

    vector<string> Result;
    string TestString1("TestValue1-TestValue2-TestValue3");
    String::split(TestString1, "-", Result);

    cout << "Res[0]:" << Result.at(0) << endl;
    cout << "Res[1]:" << Result.at(1) << endl;
    //cout << "Res[2]:" << Result.at(2) << endl;

    //BOOST_CHECK(1 == 2);
    BOOST_TEST(Result.at(0) == "TestValue1");
    BOOST_TEST(Result.at(1) == "TestValue2");
    //BOOST_TEST(Result.at(2) == "TestValue3");

    Result.clear();
    string TestString2("TestHTTP1\n\rTestHTTP2\n\rTestHTTP3\n\n\rEnd");
    String::split(TestString2, "\n\r", Result);

    cout << "Res[0]:" << Result.at(0) << endl;
    cout << "Res[1]:" << Result.at(1) << endl;
    cout << "Res[2]:" << Result.at(2) << endl;
    //cout << "Res[3]:" << Result.at(3) << endl;

    BOOST_TEST(Result.at(0) == "TestHTTP1");
    BOOST_TEST(Result.at(1) == "TestHTTP2");
    BOOST_TEST(Result.at(2) == "TestHTTP3\n");
    //BOOST_TEST(Result.at(3) == "End");
}

BOOST_AUTO_TEST_CASE( test_string_split_remains )
{
    cout << "Check string split." << endl;

    vector<string> Result;
    string TestString1("TestHTTP1\n\r");
    String::split(TestString1, "\n\r", Result);

    cout << "Res[0]:" << Result.at(0) << endl;

    cout << "String remains:'" << TestString1 << "'" << endl;

    BOOST_TEST(Result.at(0) == "TestHTTP1");
    BOOST_TEST(TestString1.length() == 0);
}

BOOST_AUTO_TEST_CASE( test_string_rsplit_1 )
{
    cout << "Check string rsplit." << endl;

    vector<string> Result;
    string TestString1("GET /path/to/file.txt HTTP/1.1\n Continues \n Again... ");
    size_t StartPos = TestString1.find("\n");

    String::rsplit(TestString1, StartPos, " ", Result);

    cout << "Res[0]:" << Result.at(0) << endl;
    cout << "Res[1]:" << Result.at(1) << endl;
    cout << "Res[2]:" << Result.at(2) << endl;

    BOOST_TEST(Result.at(0) == "HTTP/1.1");
    BOOST_TEST(Result.at(1) == "/path/to/file.txt");
    BOOST_TEST(Result.at(2) == "GET");

    Result.clear();
    string TestString2("GET / HTTP/1.1\nHeader1: HeaderContent1\nHeader2: HeaderContent2\n\r");
    size_t StartPos2 = TestString2.find("\n");

    String::rsplit(TestString2, StartPos2, " ", Result);

    cout << "Res[0]:" << Result.at(0) << endl;
    cout << "Res[1]:" << Result.at(1) << endl;
    cout << "Res[2]:" << Result.at(2) << endl;

    BOOST_TEST(Result.at(0) == "HTTP/1.1");
    BOOST_TEST(Result.at(1) == "/");
    BOOST_TEST(Result.at(2) == "GET");
}

BOOST_AUTO_TEST_CASE( test_string_rfind_replace )
{
    string TestString("This is a string to replace \n\rEND.");
    size_t FindPos = TestString.rfind("\n\r");
    TestString = TestString.replace(0, FindPos+2, "");
    cout << "TestString:" << TestString << endl;
    BOOST_TEST(TestString == "END.");
}
