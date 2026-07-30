// ─────────────────────────────────────────────────────────────────────────────
// test-parser-performance.cpp
//
// Wall-clock timing benchmark: new (C++23 std::generator / string_view) HTTP
// parser vs legacy (vector<string> / destructive split) implementation.
//
// Generates NUM_REQUESTS (500) random valid HTTP requests with varying header
// counts and payload sizes.  Each request is parsed NUM_ITERS (500) times by
// both implementations.  Per-request averages, minima and maxima (in
// nanoseconds) are written to a CSV file.
//
// Usage: ./test-parser-performance [output.csv]
//        Default output path: results-performance.csv
// ─────────────────────────────────────────────────────────────────────────────

#include <algorithm>
#include <chrono>
#include <cstring>
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

#include "../../../lib/http/httpparser.hpp"
#include "../../../lib/http/httpconstants.hpp"

using namespace std;
using namespace chrono;
using Nanos = chrono::nanoseconds::rep;

// ─── Legacy (pre-optimisation) implementations ───────────────────────────────

// Replicates old _parseRequestHeaders:
//   – copies the request into a mutable string
//   – destructive StringHelper::split into vector<string> of lines
//   – StringHelper::rsplit per line to separate key / value
static void legacy_parseRequestHeaders(
        const string&              RequestIn,
        unordered_map<string,string>& ResultRef)
{
    vector<string> Lines;
    string Request = RequestIn;                     // copy: old param was string&
    StringHelper::split(Request, "\r\n", Lines);    // destructive; erases tokens
    Lines.push_back(Request);                        // push remainder after last \r\n
    for (auto& Line : Lines) {
        vector<string> HeaderPair;
        StringHelper::rsplit(Line, Line.length(), ": ", HeaderPair);
        if (HeaderPair.size() == 2 &&
            !HeaderPair[0].empty() &&
            !HeaderPair[1].empty())
        {
            ResultRef.emplace(HeaderPair[1], HeaderPair[0]); // key, value
        }
    }
}

// Replicates old _parseGETParameter:
//   – string copy of the params substring (no string_view)
//   – destructive StringHelper::split into vector<string>
static void legacy_parseGETParameter(
        const string&              RequestURL,
        unordered_map<string,string>& ResultRef)
{
    const size_t start = RequestURL.find('?');
    if (start == string::npos || RequestURL.length() <= start) return;

    string params = RequestURL.substr(start + 1);  // copy
    vector<string> pairs;
    StringHelper::split(params, "&", pairs);         // destructive
    if (!params.empty()) pairs.push_back(params);    // push remainder

    for (auto& pair : pairs) {
        const size_t eq = pair.find('=');
        if (eq != string::npos && eq != 0 && pair.length() > eq) {
            ResultRef.emplace(
                pair.substr(0, eq),
                pair.substr(eq + 1));
        }
    }
}

// ─── New (optimised) implementations ─────────────────────────────────────────

// Mirrors HTTPParser::_parseRequestHeaders
static void new_parseRequestHeaders(
        string_view    Request,
        RequestHeader_t& ResultRef)
{
    for (const auto line : StringHelper::linesOf(Request)) {
        const size_t cs = line.find(": ");
        if (cs != string_view::npos && cs > 0) {
            const string_view val = line.substr(cs + 2);
            if (!val.empty())
                ResultRef.emplace(string(line.substr(0, cs)), string(val));
        }
    }
}

// Mirrors HTTPParser::_parseGETParameter
static void new_parseGETParameter(
        string_view    RequestURL,
        URLParamMap_t& ResultRef)
{
    const size_t start = RequestURL.find('?');
    if (start == string_view::npos || RequestURL.length() <= start) return;
    const string_view params = RequestURL.substr(start + 1);
    for (const auto pair : StringHelper::splitView(params, "&")) {
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
    string headerSection;   // full request line + headers (no \r\n\r\n or body)
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
    {"X-Request-Id",     "req-"},       // unique suffix appended below
    {"X-Forwarded-For",  "192.168.1.100"},
    {"Referer",          "https://example.com/page"},
    {"Origin",           "https://example.com"},
    {"Accept-Charset",   "utf-8"},
    {"DNT",              "1"},
};
static constexpr int kHeaderPoolSize = static_cast<int>(sizeof(kHeaders) / sizeof(kHeaders[0]));

static GenRequest generateRequest(mt19937& rng, int idx)
{
    uniform_int_distribution<int> methodDist(0, 1);
    uniform_int_distribution<int> headerDist(1, kHeaderPoolSize - 1);
    uniform_int_distribution<int> paramDist(0, 5);
    uniform_int_distribution<int> payloadDist(0, 2048);

    const bool isPost       = (methodDist(rng) == 1);
    const int  numHeaders   = headerDist(rng);          // extra headers beyond Host
    const int  numParams    = isPost ? 0 : paramDist(rng);
    const int  payloadSize  = isPost ? payloadDist(rng) : 0;

    // Build URL
    string url = "/api/resource/" + to_string(idx % 100);
    if (numParams > 0) {
        url += '?';
        for (int i = 0; i < numParams; ++i) {
            if (i > 0) url += '&';
            url += 'p' + to_string(i) + "=v" + to_string((i * 37 + idx) % 1000);
        }
    }

    // Build header section (request line + headers, no trailing \r\n\r\n)
    ostringstream oss;
    oss << (isPost ? "POST" : "GET") << ' ' << url << " HTTP/1.1\r\n";

    // Host is always first
    oss << "Host: " << kHeaders[0].second << "\r\n";
    int added = 1;
    for (int i = 1; i < kHeaderPoolSize && added <= numHeaders; ++i, ++added) {
        oss << kHeaders[i].first << ": " << kHeaders[i].second;
        if (i == 9) oss << to_string(idx);   // make X-Request-Id unique
        oss << "\r\n";
    }
    if (isPost) {
        oss << "Content-Length: " << payloadSize << "\r\n";
        oss << "Content-Type: application/json\r\n";
    }

    const int totalHeaders = added + (isPost ? 2 : 0);

    return {
        oss.str(),
        url,
        isPost ? "POST" : "GET",
        totalHeaders,
        numParams,
        payloadSize
    };
}

// ─── Timing helper ────────────────────────────────────────────────────────────

static inline Nanos now_ns()
{
    return duration_cast<nanoseconds>(
        high_resolution_clock::now().time_since_epoch()).count();
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[])
{
    const string csvPath = (argc > 1) ? argv[1] : "results-performance.csv";

    constexpr int NUM_REQUESTS = 500;
    constexpr int NUM_ITERS    = 500;

    // ── Generate requests with a fixed seed for reproducibility ──────────────
    mt19937 rng(42);
    vector<GenRequest> requests;
    requests.reserve(NUM_REQUESTS);
    for (int i = 0; i < NUM_REQUESTS; ++i)
        requests.push_back(generateRequest(rng, i));

    // ── Warmup – 10 passes to populate CPU caches ─────────────────────────────
    for (int w = 0; w < 10; ++w) {
        for (const auto& req : requests) {
            { RequestHeader_t          m; new_parseRequestHeaders(req.headerSection, m); }
            { unordered_map<string,string> m; legacy_parseRequestHeaders(req.headerSection, m); }
            { URLParamMap_t            m; new_parseGETParameter(req.url, m); }
            { unordered_map<string,string> m; legacy_parseGETParameter(req.url, m); }
        }
    }

    // ── Per-request benchmark results ─────────────────────────────────────────
    struct Result {
        Nanos nh_avg, nh_min, nh_max;   // new  header parsing
        Nanos lh_avg, lh_min, lh_max;   // legacy header parsing
        Nanos np_avg, np_min, np_max;   // new  GET-param parsing
        Nanos lp_avg, lp_min, lp_max;   // legacy GET-param parsing
    };
    vector<Result> results(NUM_REQUESTS);

    for (int r = 0; r < NUM_REQUESTS; ++r) {
        const auto& req = requests[r];
        vector<Nanos> nh(NUM_ITERS), lh(NUM_ITERS), np(NUM_ITERS), lp(NUM_ITERS);

        for (int it = 0; it < NUM_ITERS; ++it) {
            RequestHeader_t map;
            Nanos t0 = now_ns();
            new_parseRequestHeaders(req.headerSection, map);
            nh[it] = now_ns() - t0;
        }
        for (int it = 0; it < NUM_ITERS; ++it) {
            unordered_map<string,string> map;
            Nanos t0 = now_ns();
            legacy_parseRequestHeaders(req.headerSection, map);
            lh[it] = now_ns() - t0;
        }
        for (int it = 0; it < NUM_ITERS; ++it) {
            URLParamMap_t map;
            Nanos t0 = now_ns();
            new_parseGETParameter(req.url, map);
            np[it] = now_ns() - t0;
        }
        for (int it = 0; it < NUM_ITERS; ++it) {
            unordered_map<string,string> map;
            Nanos t0 = now_ns();
            legacy_parseGETParameter(req.url, map);
            lp[it] = now_ns() - t0;
        }

        auto stats = [](vector<Nanos>& v, Nanos& avg, Nanos& mn, Nanos& mx) {
            sort(v.begin(), v.end());
            mn  = v.front();
            mx  = v.back();
            avg = accumulate(v.begin(), v.end(), Nanos{0}) / static_cast<Nanos>(v.size());
        };
        stats(nh, results[r].nh_avg, results[r].nh_min, results[r].nh_max);
        stats(lh, results[r].lh_avg, results[r].lh_min, results[r].lh_max);
        stats(np, results[r].np_avg, results[r].np_min, results[r].np_max);
        stats(lp, results[r].lp_avg, results[r].lp_min, results[r].lp_max);
    }

    // ── Write CSV ─────────────────────────────────────────────────────────────
    ofstream csv(csvPath);
    if (!csv) {
        cerr << "Error: cannot open output file: " << csvPath << "\n";
        return 1;
    }

    csv << "request_idx,method,header_count,url_param_count,payload_size,"
           "new_header_avg_ns,new_header_min_ns,new_header_max_ns,"
           "legacy_header_avg_ns,legacy_header_min_ns,legacy_header_max_ns,"
           "header_speedup_x,"
           "new_getparam_avg_ns,new_getparam_min_ns,new_getparam_max_ns,"
           "legacy_getparam_avg_ns,legacy_getparam_min_ns,legacy_getparam_max_ns,"
           "getparam_speedup_x\n";

    for (int r = 0; r < NUM_REQUESTS; ++r) {
        const auto& req = requests[r];
        const auto& res = results[r];

        const double header_speedup = (res.nh_avg > 0)
            ? static_cast<double>(res.lh_avg) / res.nh_avg : 0.0;
        const double param_speedup  = (res.np_avg > 0)
            ? static_cast<double>(res.lp_avg) / res.np_avg  : 0.0;

        csv << r                    << ','
            << req.method           << ','
            << req.headerCount      << ','
            << req.urlParamCount    << ','
            << req.payloadSize      << ','
            << res.nh_avg           << ','
            << res.nh_min           << ','
            << res.nh_max           << ','
            << res.lh_avg           << ','
            << res.lh_min           << ','
            << res.lh_max           << ','
            << fixed << setprecision(2) << header_speedup << ','
            << res.np_avg           << ','
            << res.np_min           << ','
            << res.np_max           << ','
            << res.lp_avg           << ','
            << res.lp_min           << ','
            << res.lp_max           << ','
            << param_speedup        << '\n';
    }
    csv.close();

    // ── Print summary to stdout ────────────────────────────────────────────────
    Nanos sum_nh = 0, sum_lh = 0, sum_np = 0, sum_lp = 0;
    for (int r = 0; r < NUM_REQUESTS; ++r) {
        sum_nh += results[r].nh_avg;
        sum_lh += results[r].lh_avg;
        sum_np += results[r].np_avg;
        sum_lp += results[r].lp_avg;
    }

    cout << fixed << setprecision(2);
    cout << "Performance results written to: " << csvPath << "\n\n";
    cout << "Summary over " << NUM_REQUESTS << " requests x " << NUM_ITERS << " iterations:\n";
    cout << "  Header parsing:     new=" << sum_nh / 1000 << " µs total"
         << "  legacy=" << sum_lh / 1000 << " µs total"
         << "  speedup=" << (sum_nh > 0 ? static_cast<double>(sum_lh) / sum_nh : 0.0) << "x\n";
    cout << "  GET-param parsing:  new=" << sum_np / 1000 << " µs total"
         << "  legacy=" << sum_lp / 1000 << " µs total"
         << "  speedup=" << (sum_np > 0 ? static_cast<double>(sum_lp) / sum_np : 0.0) << "x\n";

    return 0;
}
