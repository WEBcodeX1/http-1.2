#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memory>

#include "../../../lib/http/httpparser.hpp"

using namespace std;

static const string HTTP_REQUEST_GET_SINGLE1("GET /test/test.png HTTP/1.1\r\nCustomHeader: one\r\n\r\n");
static const string HTTP_REQUEST_GET_SINGLE2("GET /test/test2.png HTTP/1.1\r\nHost: test.dns\r\nAnotherHeader: two\r\n\r\n");
static const string HTTP_REQUEST_GET_SINGLE3("GET /test/test3.png HTTP/1.1\r\nThirdHeader: three\r\n\r\n");

static const string HTTP_REQUEST_POST_SINGLE("POST /backend/test1 HTTP/1.1\r\nHost: test.loalnet\r\nContent-Type: application/json\r\nContent-Length: 2\r\n\r\n{}");

static const string HTTP_REQUEST_POST_PARTIAL1("POST /other/path HTTP/1.1\r\nHost: test.loal");
static const string HTTP_REQUEST_POST_PARTIAL2("net\r\nContent-Type: application/json\r\nContent-Length: 10\r");
static const string HTTP_REQUEST_POST_PARTIAL3("\n\r\n{12345678}");


BOOST_AUTO_TEST_CASE( test_single_valid_get_request_full_transmit )
{
    cout << "Check single GET request (full buffer transmit at once)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_GET_SINGLE1.c_str(), HTTP_REQUEST_GET_SINGLE1.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("CustomHeader");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/test/test.png");
    BOOST_TEST(rh == "one");

}

BOOST_AUTO_TEST_CASE( test_single_post_request_full_transmit )
{
    cout << "Check single POST request (full buffer transmit at once)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_POST_SINGLE.c_str(), HTTP_REQUEST_POST_SINGLE.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Host");
    auto rp = Requests.at(0).Payload;

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << " Payload: " << rp << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 2);
    BOOST_TEST(ru == "/backend/test1");
    BOOST_TEST(rh == "test.loalnet");
    BOOST_TEST(rp == "{}");
}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_request_full_transmit )
{
    cout << "Check multiple GET requests (full buffer transmit at once)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    parser->appendBuffer(full_request.c_str(), full_request.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("AnotherHeader");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("ThirdHeader");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 3);

    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 1);
    BOOST_TEST(ru2 == "/test/test2.png");
    BOOST_TEST(rh2 == "two");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test3.png");
    BOOST_TEST(rh3 == "three");

}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_and_post_requests_full_transmit )
{
    cout << "Check multiple GET and POST requests (full buffer transmit at once)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    parser->appendBuffer(full_request.c_str(), full_request.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("Content-Type");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("Host");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 5);

    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 2);
    BOOST_TEST(ru2 == "/backend/test1");
    BOOST_TEST(rh2 == "application/json");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test2.png");
    BOOST_TEST(rh3 == "test.dns");

}

BOOST_AUTO_TEST_CASE( test_single_valid_post_request_partial_transmit )
{
    cout << "Check single POST request (partial transmit)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL1.c_str(), HTTP_REQUEST_POST_PARTIAL1.length());
    parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL2.c_str(), HTTP_REQUEST_POST_PARTIAL2.length());
    parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL3.c_str(), HTTP_REQUEST_POST_PARTIAL3.length());
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Content-Type");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 2);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/other/path");
    BOOST_TEST(rh == "application/json");

}

BOOST_AUTO_TEST_CASE( test_single_valid_get_request_1byte_segment )
{
    cout << "Check single GET request (1-byte segments)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    for (size_t i = 0; i < HTTP_REQUEST_GET_SINGLE1.length(); ++i) {
        parser->appendBuffer(HTTP_REQUEST_GET_SINGLE1.c_str() + i, 1);
    }
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("CustomHeader");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/test/test.png");
    BOOST_TEST(rh == "one");

}

BOOST_AUTO_TEST_CASE( test_single_post_request_1byte_segment )
{
    cout << "Check single POST request (1-byte segments)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    for (size_t i = 0; i < HTTP_REQUEST_POST_SINGLE.length(); ++i) {
        parser->appendBuffer(HTTP_REQUEST_POST_SINGLE.c_str() + i, 1);
    }
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Host");
    auto rp = Requests.at(0).Payload;

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << " Payload: " << rp << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 1);
    BOOST_TEST(rm == 2);
    BOOST_TEST(ru == "/backend/test1");
    BOOST_TEST(rh == "test.loalnet");
    BOOST_TEST(rp == "{}");
}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_request_1byte_segment )
{
    cout << "Check multiple GET requests (1-byte segments)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    for (size_t i = 0; i < full_request.length(); ++i) {
        parser->appendBuffer(full_request.c_str() + i, 1);
    }
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("AnotherHeader");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("ThirdHeader");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 3);

    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 1);
    BOOST_TEST(ru2 == "/test/test2.png");
    BOOST_TEST(rh2 == "two");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test3.png");
    BOOST_TEST(rh3 == "three");

}

BOOST_AUTO_TEST_CASE( test_multiple_valid_get_and_post_requests_1byte_segment )
{
    cout << "Check multiple GET and POST requests (1-byte segments)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    string full_request(HTTP_REQUEST_GET_SINGLE1);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE2);
    full_request.append(HTTP_REQUEST_POST_SINGLE);
    full_request.append(HTTP_REQUEST_GET_SINGLE3);

    for (size_t i = 0; i < full_request.length(); ++i) {
        parser->appendBuffer(full_request.c_str() + i, 1);
    }
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv1 = Requests.at(0).HTTPVersion;
    auto rm1 = Requests.at(0).HTTPMethod;
    auto ru1 = Requests.at(0).URL;
    auto rh1 = Requests.at(0).RequestHeaders.at("CustomHeader");

    auto rv2 = Requests.at(1).HTTPVersion;
    auto rm2 = Requests.at(1).HTTPMethod;
    auto ru2 = Requests.at(1).URL;
    auto rh2 = Requests.at(1).RequestHeaders.at("Content-Type");

    auto rv3 = Requests.at(2).HTTPVersion;
    auto rm3 = Requests.at(2).HTTPMethod;
    auto ru3 = Requests.at(2).URL;
    auto rh3 = Requests.at(2).RequestHeaders.at("Host");

    cout << "Request size: " << rs << endl;
    cout << "Request 1 version: " << rv1 << " method: " << rm1 << " URL: " << ru1 << " Header: " << rh1 << endl;
    cout << "Request 2 version: " << rv2 << " method: " << rm2 << " URL: " << ru2 << " Header: " << rh2 << endl;
    cout << "Request 3 version: " << rv3 << " method: " << rm3 << " URL: " << ru3 << " Header: " << rh3 << endl;

    BOOST_TEST(rs == 5);

    BOOST_TEST(rv1 == 1);
    BOOST_TEST(rm1 == 1);
    BOOST_TEST(ru1 == "/test/test.png");
    BOOST_TEST(rh1 == "one");

    BOOST_TEST(rv2 == 1);
    BOOST_TEST(rm2 == 2);
    BOOST_TEST(ru2 == "/backend/test1");
    BOOST_TEST(rh2 == "application/json");

    BOOST_TEST(rv3 == 1);
    BOOST_TEST(rm3 == 1);
    BOOST_TEST(ru3 == "/test/test2.png");
    BOOST_TEST(rh3 == "test.dns");

}

BOOST_AUTO_TEST_CASE( test_single_valid_post_request_partial_transmit_1byte_segment )
{
    cout << "Check single POST request (partial transmit, 1-byte segments)." << endl;

    RequestsVector_t Requests;

    unique_ptr<HTTPParser> parser = make_unique<HTTPParser>(4096);

    for (size_t i = 0; i < HTTP_REQUEST_POST_PARTIAL1.length(); ++i) {
        parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL1.c_str() + i, 1);
    }
    for (size_t i = 0; i < HTTP_REQUEST_POST_PARTIAL2.length(); ++i) {
        parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL2.c_str() + i, 1);
    }
    for (size_t i = 0; i < HTTP_REQUEST_POST_PARTIAL3.length(); ++i) {
        parser->appendBuffer(HTTP_REQUEST_POST_PARTIAL3.c_str() + i, 1);
    }
    Requests = parser->getRequests();

    auto rs = Requests.size();
    auto rv = Requests.at(0).HTTPVersion;
    auto rm = Requests.at(0).HTTPMethod;
    auto ru = Requests.at(0).URL;
    auto rh = Requests.at(0).RequestHeaders.at("Content-Type");

    cout << "Request size: " << rs << " version: " << rv << " method: " << rm << " URL: " << ru << " Header: " << rh << endl;

    BOOST_TEST(rs == 1);
    BOOST_TEST(rv == 2);
    BOOST_TEST(rm == 1);
    BOOST_TEST(ru == "/other/path");
    BOOST_TEST(rh == "application/json");

}

BOOST_AUTO_TEST_CASE( test_single_get_request_1byte_segment )
{
    cout << "Check single GET request (1-byte segments)." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    const json JSONConfig = { { "interpreters", 2 }, { "path", "/test" } };

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.JSONConfig = JSONConfig;

    Namespaces.emplace(
        "test1", NamespaceProps
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = std::make_unique<ASRequestHandler>(
        Namespaces,
        BaseAdresses_t{ SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD, Namespaces));

    std::string Request("GET /test/test.png HTTP/1.1\r\nCustomHeader: one\r\n\r\n");
    for (size_t i = 0; i < Request.length(); ++i) {
        ClientObj->appendBuffer(Request.c_str() + i, 1);
    }
    auto r = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r == 1);
}

BOOST_AUTO_TEST_CASE( test_multiple_get_request_1byte_segment )
{
    cout << "Check multiple (2) GET requests (1-byte segments)." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    const json JSONConfig = { { "interpreters", 2 }, { "path", "/test" } };

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.JSONConfig = JSONConfig;

    Namespaces.emplace(
        "test1", NamespaceProps
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = std::make_unique<ASRequestHandler>(
        Namespaces,
        BaseAdresses_t{ SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD, Namespaces));

    std::string Request("GET /test/test1.png HTTP/1.1\r\nCustomHeader: one\r\n\r\nGET /test/test2.png HTTP/1.1\r\nCustomHeader: two\r\n\r\n");
    for (size_t i = 0; i < Request.length(); ++i) {
        ClientObj->appendBuffer(Request.c_str() + i, 1);
    }
    auto r = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r == 2);
}

BOOST_AUTO_TEST_CASE( test_single_post_as_request_1byte_segment )
{
    cout << "Check single POST request (1-byte segments)." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    const json JSONConfig = { { "interpreters", 2 }, { "path", "/test" } };

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.JSONConfig = JSONConfig;

    Namespaces.emplace(
        "test1", NamespaceProps
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = std::make_unique<ASRequestHandler>(
        Namespaces,
        BaseAdresses_t{ SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD, Namespaces));

    std::string Request("POST /backend/test1 HTTP/1.1\r\nHost: test.loalnet\r\nContent-Type: application/json\r\nContent-Length: 2\r\n\r\n{}");
    for (size_t i = 0; i < Request.length(); ++i) {
        ClientObj->appendBuffer(Request.c_str() + i, 1);
    }
    auto r = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r == 0);
}

BOOST_AUTO_TEST_CASE( test_single_get_as_request_1byte_segment )
{
    cout << "Check single GET AS request (1-byte segments)." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    std::ifstream ConfigFile("./as-get.json");
    const json JSONConfig = json::parse(ConfigFile);

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.JSONConfig = JSONConfig;

    Namespaces.emplace(
        "test.local", NamespaceProps
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = std::make_unique<ASRequestHandler>(
        Namespaces,
        BaseAdresses_t{ SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD, Namespaces));

    std::string Request("GET /backend/test1?param1=test1&param2=test2 HTTP/1.1\r\nHost: test.local\r\nCustomHeader: one\r\n\r\n");
    for (size_t i = 0; i < Request.length(); ++i) {
        ClientObj->appendBuffer(Request.c_str() + i, 1);
    }
    auto r = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r == 0);
}

BOOST_AUTO_TEST_CASE( test_multiple_get_request_truncated_1byte_segment )
{
    cout << "Check multiple (3) GET requests, truncated over 2 requests (1-byte segments)." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    const json JSONConfig = { {"interpreters", 2}, {"path", "/test"} };

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.JSONConfig = JSONConfig;

    Namespaces.emplace(
        "test1", NamespaceProps
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = std::make_unique<ASRequestHandler>(
        Namespaces,
        BaseAdresses_t{ SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD, Namespaces));

    std::string Request("GET /t/tA.png HTTP/1.1\r\nCustomHeader: a\r\n\r\nGET /t/tB.png HTTP/1.1\r\nCustomHeader: b\r\n\r\nGET /t/tC.png HT");
    for (size_t i = 0; i < Request.length(); ++i) {
        ClientObj->appendBuffer(Request.c_str() + i, 1);
    }
    auto r1 = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    std::string Request2("TP/1.1\r\nCustomHeader: c\r\n\r\n");
    for (size_t i = 0; i < Request2.length(); ++i) {
        ClientObj->appendBuffer(Request2.c_str() + i, 1);
    }
    auto r2 = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r1 == 2);
    BOOST_TEST(r2 == 1);
}
