// ─────────────────────────────────────────────────────────────────────────────
// test-parser-memory.cpp
//
// Heap-allocation benchmark: new (C++23 ispanstream / string_view) HTTP
// parser vs legacy (vector<string> / destructive split) implementation.
//
// Global ::operator new / ::operator delete are replaced with tracking
// wrappers.  While g_tracking is true every allocation is counted and its
// size accumulated; deallocation is not subtracted so the metric captures
// *gross* bytes allocated (total work given to the allocator), which is the
// relevant measure for GC / cache pressure.
//
// Generates NUM_REQUESTS (500) random valid HTTP requests with varying header
// counts and payload sizes.  Each request is parsed NUM_MEASURE (10) times;
// the median allocation count and byte total are written to a CSV file.
//
// Usage: ./test-parser-memory [output.csv]
//        Default output path: results-memory.csv
// ─────────────────────────────────────────────────────────────────────────────

#include <cstdlib>
#include <new>

// ─── Allocation tracker (must appear before any other includes) ───────────────
static bool   g_tracking    = false;
static size_t g_alloc_count = 0;
static size_t g_alloc_bytes = 0;

void* operator new(size_t n) {
    void* p = std::malloc(n);
    if (!p) throw std::bad_alloc{};
    if (g_tracking) { ++g_alloc_count; g_alloc_bytes += n; }
    return p;
}
void* operator new[](size_t n) {
    void* p = std::malloc(n);
    if (!p) throw std::bad_alloc{};
    if (g_tracking) { ++g_alloc_count; g_alloc_bytes += n; }
    return p;
}
void operator delete(void* p)            noexcept { std::free(p); }
void operator delete(void* p, size_t)    noexcept { std::free(p); }
void operator delete[](void* p)          noexcept { std::free(p); }
void operator delete[](void* p, size_t)  noexcept { std::free(p); }
// ─────────────────────────────────────────────────────────────────────────────

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <spanstream>

#include "../../../lib/http/httpparser.hpp"
#include "../../../lib/http/httpconstants.hpp"

using namespace std;

// ─── RAII allocation measurement scope ───────────────────────────────────────
//
// count() / bytes() read the live globals while tracking is active; the
// destructor only turns tracking off (it does not copy values to members).

struct AllocScope {
    AllocScope() {
        g_alloc_count = 0;
        g_alloc_bytes = 0;
        g_tracking    = true;
    }
    ~AllocScope() {
        g_tracking = false;
    }
    size_t count() const { return g_alloc_count; }
    size_t bytes() const { return g_alloc_bytes; }
};

// ─── Legacy (pre-optimisation) implementations ───────────────────────────────

static void legacy_parseRequestHeaders(
        const string&              RequestIn,
        unordered_map<string,string>& ResultRef)
{
    vector<string> Lines;
    string Request = RequestIn;
    StringHelper::split(Request, "\r\n", Lines);
    Lines.push_back(Request);
    for (auto& Line : Lines) {
        vector<string> HeaderPair;
        StringHelper::rsplit(Line, Line.length(), ": ", HeaderPair);
        if (HeaderPair.size() == 2 &&
            !HeaderPair[0].empty() &&
            !HeaderPair[1].empty())
        {
            ResultRef.emplace(HeaderPair[1], HeaderPair[0]);
        }
    }
}

static void legacy_parseGETParameter(
        const string&              RequestURL,
        unordered_map<string,string>& ResultRef)
{
    const size_t start = RequestURL.find('?');
    if (start == string::npos || RequestURL.length() <= start) return;
    string params = RequestURL.substr(start + 1);
    vector<string> pairs;
    StringHelper::split(params, "&", pairs);
    if (!params.empty()) pairs.push_back(params);
    for (auto& pair : pairs) {
        const size_t eq = pair.find('=');
        if (eq != string::npos && eq != 0 && pair.length() > eq) {
            ResultRef.emplace(pair.substr(0, eq), pair.substr(eq + 1));
        }
    }
}

// ─── New (optimised) implementations ─────────────────────────────────────────

static void new_parseRequestHeaders(
        string_view    Request,
        RequestHeader_t& ResultRef)
{
    ispanstream ss(span<const char>(Request.data(), Request.size()));
    string line;
    while (getline(ss, line, '\n')) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const size_t cs = line.find(": ");
        if (cs != string::npos && cs > 0) {
            string_view lv(line);
            string_view key = lv.substr(0, cs);
            string_view val = lv.substr(cs + 2);
            if (!val.empty())
                ResultRef.emplace(string(key), string(val));
        }
    }
}

static void new_parseGETParameter(
        string_view    RequestURL,
        URLParamMap_t& ResultRef)
{
    const size_t start = RequestURL.find('?');
    if (start == string_view::npos || RequestURL.length() <= start) return;
    string_view params = RequestURL.substr(start + 1);
    vector<string_view> pairs;
    StringHelper::split(params, "&", pairs);
    for (const auto& pair : pairs) {
        const size_t eq = pair.find('=');
        if (eq != string_view::npos && eq != 0 && pair.length() > eq) {
            ResultRef.emplace(
                string(pair.substr(0, eq)),
                string(pair.substr(eq + 1)));
        }
    }
}

// ─── Request generator ────────────────────────────────────────────────────────

struct GenRequest {
    string headerSection;
    string url;
    string method;
    int    headerCount;
    int    urlParamCount;
    int    payloadSize;
};

static const pair<string,string> kHeaders[] = {
    {"Host",             "example.com"},
    {"User-Agent",       "Benchmark/1.0"},
    {"Accept",           "text/html,application/xhtml+xml,application/xml;q=0.9"},
    {"Accept-Language",  "en-US,en;q=0.9"},
    {"Accept-Encoding",  "gzip, deflate, br"},
    {"Connection",       "keep-alive"},
    {"Cache-Control",    "no-cache"},
    {"Pragma",           "no-cache"},
    {"Authorization",    "******"},
    {"X-Request-Id",     "req-"},
    {"X-Forwarded-For",  "192.168.1.100"},
    {"Referer",          "https://example.com/page"},
    {"Origin",           "https://example.com"},
    {"Accept-Charset",   "utf-8"},
    {"DNT",              "1"},
};
static constexpr int kHeaderPoolSize = (int)(sizeof(kHeaders) / sizeof(kHeaders[0]));

static GenRequest generateRequest(mt19937& rng, int idx)
{
    uniform_int_distribution<int> methodDist(0, 1);
    uniform_int_distribution<int> headerDist(1, kHeaderPoolSize - 1);
    uniform_int_distribution<int> paramDist(0, 5);
    uniform_int_distribution<int> payloadDist(0, 2048);

    const bool isPost      = (methodDist(rng) == 1);
    const int  numHeaders  = headerDist(rng);
    const int  numParams   = isPost ? 0 : paramDist(rng);
    const int  payloadSize = isPost ? payloadDist(rng) : 0;

    string url = "/api/resource/" + to_string(idx % 100);
    if (numParams > 0) {
        url += '?';
        for (int i = 0; i < numParams; ++i) {
            if (i > 0) url += '&';
            url += 'p' + to_string(i) + "=v" + to_string((i * 37 + idx) % 1000);
        }
    }

    ostringstream oss;
    oss << (isPost ? "POST" : "GET") << ' ' << url << " HTTP/1.1\r\n";
    oss << "Host: " << kHeaders[0].second << "\r\n";
    int added = 1;
    for (int i = 1; i < kHeaderPoolSize && added <= numHeaders; ++i, ++added) {
        oss << kHeaders[i].first << ": " << kHeaders[i].second;
        if (i == 9) oss << to_string(idx);
        oss << "\r\n";
    }
    if (isPost) {
        oss << "Content-Length: " << payloadSize << "\r\n";
        oss << "Content-Type: application/json\r\n";
    }

    return {
        oss.str(),
        url,
        isPost ? "POST" : "GET",
        added + (isPost ? 2 : 0),
        numParams,
        payloadSize
    };
}

// ─── Median helper ────────────────────────────────────────────────────────────

template<typename T>
static T median(vector<T>& v) {
    sort(v.begin(), v.end());
    return v[v.size() / 2];
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[])
{
    const string csvPath = (argc > 1) ? argv[1] : "results-memory.csv";

    constexpr int NUM_REQUESTS = 500;
    constexpr int NUM_MEASURE  = 10;   // repeated measurements for stable median

    // ── Generate requests ─────────────────────────────────────────────────────
    mt19937 rng(42);
    vector<GenRequest> requests;
    requests.reserve(NUM_REQUESTS);
    for (int i = 0; i < NUM_REQUESTS; ++i)
        requests.push_back(generateRequest(rng, i));

    // ── Warmup – fill allocator pools so measurements are stable ─────────────
    for (int w = 0; w < 5; ++w) {
        for (const auto& req : requests) {
            { RequestHeader_t          m; new_parseRequestHeaders(req.headerSection, m); }
            { unordered_map<string,string> m; legacy_parseRequestHeaders(req.headerSection, m); }
            { URLParamMap_t            m; new_parseGETParameter(req.url, m); }
            { unordered_map<string,string> m; legacy_parseGETParameter(req.url, m); }
        }
    }

    // ── Per-request measurement results ───────────────────────────────────────
    struct Result {
        size_t nh_count, nh_bytes;   // new  header parse
        size_t lh_count, lh_bytes;   // legacy header parse
        size_t np_count, np_bytes;   // new  GET-param parse
        size_t lp_count, lp_bytes;   // legacy GET-param parse
    };
    vector<Result> results(NUM_REQUESTS);

    for (int r = 0; r < NUM_REQUESTS; ++r) {
        const auto& req = requests[r];

        vector<size_t> nh_cnt(NUM_MEASURE), nh_byt(NUM_MEASURE);
        vector<size_t> lh_cnt(NUM_MEASURE), lh_byt(NUM_MEASURE);
        vector<size_t> np_cnt(NUM_MEASURE), np_byt(NUM_MEASURE);
        vector<size_t> lp_cnt(NUM_MEASURE), lp_byt(NUM_MEASURE);

        for (int m = 0; m < NUM_MEASURE; ++m) {
            {
                AllocScope sc;
                RequestHeader_t map;
                new_parseRequestHeaders(req.headerSection, map);
                nh_cnt[m] = sc.count(); nh_byt[m] = sc.bytes();
            }
            {
                AllocScope sc;
                unordered_map<string,string> map;
                legacy_parseRequestHeaders(req.headerSection, map);
                lh_cnt[m] = sc.count(); lh_byt[m] = sc.bytes();
            }
            {
                AllocScope sc;
                URLParamMap_t map;
                new_parseGETParameter(req.url, map);
                np_cnt[m] = sc.count(); np_byt[m] = sc.bytes();
            }
            {
                AllocScope sc;
                unordered_map<string,string> map;
                legacy_parseGETParameter(req.url, map);
                lp_cnt[m] = sc.count(); lp_byt[m] = sc.bytes();
            }
        }

        results[r] = {
            median(nh_cnt), median(nh_byt),
            median(lh_cnt), median(lh_byt),
            median(np_cnt), median(np_byt),
            median(lp_cnt), median(lp_byt),
        };
    }

    // ── Write CSV ─────────────────────────────────────────────────────────────
    ofstream csv(csvPath);
    if (!csv) {
        cerr << "Error: cannot open output file: " << csvPath << "\n";
        return 1;
    }

    csv << "request_idx,method,header_count,url_param_count,payload_size,"
           "new_header_alloc_count,new_header_alloc_bytes,"
           "legacy_header_alloc_count,legacy_header_alloc_bytes,"
           "header_count_reduction,header_bytes_reduction_pct,"
           "new_getparam_alloc_count,new_getparam_alloc_bytes,"
           "legacy_getparam_alloc_count,legacy_getparam_alloc_bytes,"
           "getparam_count_reduction,getparam_bytes_reduction_pct\n";

    for (int r = 0; r < NUM_REQUESTS; ++r) {
        const auto& req = requests[r];
        const auto& res = results[r];

        const long  hdr_count_diff = static_cast<long>(res.lh_count) - static_cast<long>(res.nh_count);
        const double hdr_bytes_pct = (res.lh_bytes > 0)
            ? 100.0 * (1.0 - static_cast<double>(res.nh_bytes) / res.lh_bytes) : 0.0;

        const long  par_count_diff = static_cast<long>(res.lp_count) - static_cast<long>(res.np_count);
        const double par_bytes_pct = (res.lp_bytes > 0)
            ? 100.0 * (1.0 - static_cast<double>(res.np_bytes) / res.lp_bytes) : 0.0;

        csv << r                   << ','
            << req.method          << ','
            << req.headerCount     << ','
            << req.urlParamCount   << ','
            << req.payloadSize     << ','
            << res.nh_count        << ','
            << res.nh_bytes        << ','
            << res.lh_count        << ','
            << res.lh_bytes        << ','
            << hdr_count_diff      << ','
            << fixed << setprecision(1) << hdr_bytes_pct << ','
            << res.np_count        << ','
            << res.np_bytes        << ','
            << res.lp_count        << ','
            << res.lp_bytes        << ','
            << par_count_diff      << ','
            << par_bytes_pct       << '\n';
    }
    csv.close();

    // ── Print summary to stdout ────────────────────────────────────────────────
    size_t tot_nh_b = 0, tot_lh_b = 0, tot_np_b = 0, tot_lp_b = 0;
    size_t tot_nh_c = 0, tot_lh_c = 0, tot_np_c = 0, tot_lp_c = 0;
    for (int r = 0; r < NUM_REQUESTS; ++r) {
        tot_nh_c += results[r].nh_count; tot_nh_b += results[r].nh_bytes;
        tot_lh_c += results[r].lh_count; tot_lh_b += results[r].lh_bytes;
        tot_np_c += results[r].np_count; tot_np_b += results[r].np_bytes;
        tot_lp_c += results[r].lp_count; tot_lp_b += results[r].lp_bytes;
    }

    cout << fixed << setprecision(1);
    cout << "Memory results written to: " << csvPath << "\n\n";
    cout << "Summary over " << NUM_REQUESTS << " requests (median over " << NUM_MEASURE << " measurements):\n";
    cout << "  Header parsing:\n"
         << "    Allocations: new=" << tot_nh_c << "  legacy=" << tot_lh_c
         << "  saved=" << static_cast<long>(tot_lh_c) - static_cast<long>(tot_nh_c) << '\n'
         << "    Bytes:       new=" << tot_nh_b << "  legacy=" << tot_lh_b
         << "  reduction=" << (tot_lh_b > 0 ? 100.0 * (1.0 - static_cast<double>(tot_nh_b) / tot_lh_b) : 0.0) << "%\n";
    cout << "  GET-param parsing:\n"
         << "    Allocations: new=" << tot_np_c << "  legacy=" << tot_lp_c
         << "  saved=" << static_cast<long>(tot_lp_c) - static_cast<long>(tot_np_c) << '\n'
         << "    Bytes:       new=" << tot_np_b << "  legacy=" << tot_lp_b
         << "  reduction=" << (tot_lp_b > 0 ? 100.0 * (1.0 - static_cast<double>(tot_np_b) / tot_lp_b) : 0.0) << "%\n";

    return 0;
}
