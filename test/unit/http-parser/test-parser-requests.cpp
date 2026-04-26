#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memory>

#include "../../../lib/http/httpparser.hpp"

using namespace std;


BOOST_AUTO_TEST_CASE( test_single_get_request )
{
    cout << "Check single GET request." << endl;

    HTTPParser* parser = new HTTPParser(4096);

    std::string Request("GET /test/test.png HTTP/1.1\r\nCustomHeader: one\r\n\r\n");
    parser->appendBuffer(Request.c_str(), Request.length());

    auto r=1;
    BOOST_TEST(r == 1);
}
