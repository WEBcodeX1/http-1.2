#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../../../Client.hpp"
#include "../../../ClientHandler.hpp"


using namespace std;

BOOST_AUTO_TEST_CASE( test_single_get_request )
{
    cout << "Check single GET request." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.InterpreterCount = 2;
    NamespaceProps.PathRel = "/test";

    Namespaces.insert(
        NamespacePair_t("test1", NamespaceProps)
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = new ASRequestHandler(
        Namespaces,
        { SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD));

    std::string Request("GET /test/test.png HTTP/1.1\r\nCustomHeader: one\r\n\r\n");
    ClientObj->appendBuffer(Request.c_str(), Request.length());
    auto r = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r == 1);
}

BOOST_AUTO_TEST_CASE( test_multiple_get_request )
{
    cout << "Check multiple (2) GET requests." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.InterpreterCount = 2;
    NamespaceProps.PathRel = "/test";

    Namespaces.insert(
        NamespacePair_t("test1", NamespaceProps)
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = new ASRequestHandler(
        Namespaces,
        { SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD));

    std::string Request("GET /test/test1.png HTTP/1.1\r\nCustomHeader: one\r\n\r\nGET /test/test2.png HTTP/1.1\r\nCustomHeader: two\r\n\r\n");
    ClientObj->appendBuffer(Request.c_str(), Request.length());
    auto r = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r == 2);
}

BOOST_AUTO_TEST_CASE( test_single_post_request )
{
    cout << "Check single POST request." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.InterpreterCount = 2;
    NamespaceProps.PathRel = "/test";

    Namespaces.insert(
        NamespacePair_t("test1", NamespaceProps)
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = new ASRequestHandler(
        Namespaces,
        { SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD));

    std::string Request("POST /python/test1.py HTTP/1.1\r\nHost: test.loalnet\r\nContent-Type: application/json\r\nContent-Length: 2\r\n\r\n{}");
    ClientObj->appendBuffer(Request.c_str(), Request.length());
    auto r = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r == 0);
}

BOOST_AUTO_TEST_CASE( test_multiple_get_request_truncated )
{
    cout << "Check multiple (3) GET requests, truncated over 2 requests." << endl;

    void* SHMBase = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMGetRequests = static_cast<char*>(SHMBase) + sizeof(atomic_uint16_t) + sizeof(uint16_t);

    void* SHMASMeta = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASRequests = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    void* SHMASResults = mmap(NULL, SHMEM_STATICFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    Namespaces_t Namespaces;
    NamespaceProps_t NamespaceProps;
    NamespaceProps.FilesystemRef = nullptr;
    NamespaceProps.InterpreterCount = 2;
    NamespaceProps.PathRel = "/test";

    Namespaces.insert(
        NamespacePair_t("test1", NamespaceProps)
    );

    ASRequestHandlerRef_t ASRequestHandlerRef = new ASRequestHandler(
        Namespaces,
        { SHMASMeta, SHMASRequests, SHMASResults }
    );

    ClientFD_t ClientFD = 1;
    ClientRef_t ClientObj(new HTTPParser(ClientFD));

    std::string Request("GET /t/tA.png HTTP/1.1\r\nCustomHeader: a\r\n\r\nGET /t/tB.png HTTP/1.1\r\nCustomHeader: b\r\n\r\nGET /t/tC.png HT");
    ClientObj->appendBuffer(Request.c_str(), Request.length());
    auto r1 = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    std::string Request2("TP/1.1\r\nCustomHeader: c\r\n\r\n");
    ClientObj->appendBuffer(Request2.c_str(), Request2.length());
    auto r2 = ClientObj->processRequests(SHMGetRequests, ASRequestHandlerRef);

    BOOST_TEST(r1 == 2);
    BOOST_TEST(r2 == 1);
}
