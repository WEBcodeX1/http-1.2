<h1 align="center">🚀 HTTP/1.2 - FalconAS</h1>
<p align="center"><em>Lightning-Fast, Next-Level Python / Java Application Server with HTTP/1.2 Protocol</em></p>

<div align="center">
  <a href="https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql">
    <img src="https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql/badge.svg" alt="CodeQL">
  </a>
  <a href="https://github.com/WEBcodeX1/http-1.2/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/WEBcodeX1/http-1.2?style=flat" alt="License">
  </a>
  <a href="https://docs.webcodex.de/developer/falconas/doxygen/inherits.html">
    <img src="https://img.shields.io/badge/docs-webcodex-blue" alt="Docs Oxygen">
  </a>
  <a href="https://docs.webcodex.de/developer/falconas/sphinx/index.html">
    <img src="https://img.shields.io/badge/docs-sphinx-blue" alt="Docs Sphinx">
  </a>
  <!--
  <a href="https://github.com/WEBcodeX1/http-1.2/releases">
    <img src="https://img.shields.io/github/v/release/WEBcodeX1/http-1.2?include_prereleases" alt="Release">
  </a>
  -->
</div>

---

## :pushpin: Overview

**HTTP/1.2** is a revolutionary approach to web protocols, designed as a simpler, more secure,
and significantly faster alternative to HTTP/2 and propably HTTP/3.

We've built **HTTP/1.2** - a much more lightweight solution.

- ✅ **Lightning-Fast Performance**: Optimized for modern hardware with epoll-based architecture
- ✅ **Rock-Solid Security**: Runs with tight backend process-separation model (non-threaded)
- ✅ **SPA Realtime Demands**: Built for modern browser applications (SPAs) real-time demands
- ✅ **Zero Bloat**: Eliminates unnecessary complexity while maintaining compatibility
- ✅ **Python-Powered**: FalconAS application server with embedded Python scripting
- ✅ **Java-Powered**: FalconAS application server with embedded Java scripting

## :bookmark_tabs: Table of Contents

1. [Quick Start](#racehorse-quick-start)
2. [Features](#star2-features)
3. [Why HTTP/1.2?](#mega-why-http12)
4. [Build & Installation](#hammer_and_wrench-build--installation)
5. [Testing](#test_tube-testing)
6. [Technical Architecture](#gear-technical-architecture)
7. [Documentation](#page_with_curl-documentation)
8. [Community & Support](#globe_with_meridians-community--support)
9. [Contributing](#wave-contributing)
10. [Future Milestones](#alarm_clock-future-milestones)
11. [License](#memo-license)

---

## :racehorse: Quick Start

Get HTTP/1.2 FalconAS running in minutes:

```bash
# clone repository
git clone https://github.com/WEBcodeX1/http-1.2.git
cd http-1.2

# install dependencies (Ubuntu 22.04/Debian 12)
apt-get install git cmake python3-pip libboost-all-dev python3-dev nlohmann-json3-dev

# build and install
cmake CMakeLists.txt .
make
make install

# configure system
. ./scripts/ulimit.sh
. ./scripts/set-transparent-hugepages.sh

# start server
/usr/local/bin/falcon-as
```

The server will be available with test applications at:
- `http://testapp1.local/` 
- `http://testapp2.local/`

For detailed installation instructions, see [BUILD.md](BUILD.md).

---

## :star2: Features

### Core Advantages
- **🚀 Lightning-Fast**: Epoll-based architecture for maximum performance
- **🔒 Secure by Design**: Interpreters run non-threaded / unix process isolated
- **⚡ Zero Bloat**: Eliminates HTTP/2's excessive complexity
- **🎯 SPA-Optimized**: Perfect for modern Single Page Applications real time demands
- **🐍 Python-Powered**: Embedded Python scripting with Boost integration
- **🐍 Java-Powered**: Embedded Java scripting with Boost integration

### Protocol Innovations
- **Fixed HTTP/1.1 Pipelining**: Resolves misordered request/reply pairs with UUID headers
- **Permanent Keep-Alive**: Single socket per client for optimal TCP/IP utilization  
- **Request UUID System**: Eliminates response ordering requirements
- **Simplified Architecture**: Text-based protocol maintaining HTTP/1.1 simplicity

### Technical Features
- **Static Content Server**: High-performance file serving with sendfile()
- **Application Server**: Python scripting with shared memory architecture
- **Memory Management**: Huge pages support and optimized memory layout
- **Process Architecture**: Separate processes to avoid Python GIL limitations
- **JSON Configuration**: Clean, maintainable configuration system
- **JSON Powered By**: [nlohmann/json](https://github.com/nlohmann/json) C++ library for configuration and data processing

---

## :mega: Why HTTP/1.2?

### The Problem with HTTP/2 & HTTP/3

**HTTP/2 Complexity Crisis:**
- Excessive complexity destroys HTTP/1.1's elegant simplicity
- Everything packed into one "black box" without logical separation
- Libraries are confusing and difficult to understand
- TLS/SSL handling unnecessarily embedded in protocol

**HTTP/3 UDP Issues:**
- Works only for precalculated CDN data
- Creates new complexity without solving core issues

### Our Solution: HTTP/1.2

HTTP/1.2 **fixes** HTTP/1.1's single major flaw (broken pipelining) while **avoiding** HTTP/2's complexity:

> **🎯 Key Innovation**: Adding a simple `Request-UUID` header eliminates response ordering issues, making HTTP/2's complex Layer-7 multiplexing unnecessary.

---

## :hammer_and_wrench: Build & Installation

### Prerequisites

**Operating System Support:**
- Ubuntu 22.04 (Jammy Jellyfish) 
- Ubuntu 24.04 (Noble Numbat)
- Debian 12

**Required Dependencies:**
```bash
# core build tools
apt-get install git cmake

# c++ boost libraries
apt-get install libboost-all-dev

# python development headers
apt-get install python3-dev

# c++ json library
apt-get install nlohmann-json3-dev
```

### Build Options

**Standard Build:**
```bash
cmake CMakeLists.txt .
make
make install
```

**Debug Build:**
```bash
cmake -DDEBUG_BUILD=1 CMakeLists.txt .
make
make install
```

**Java Backend Build:**
```bash
export JAVA_HOME=/usr/lib/jvm/jdk-24.0.2-oracle-x64/
cmake -DJAVA_BACKEND=1 CMakeLists.txt .
make
make install
```

For complete build instructions, see [BUILD.md](BUILD.md).

---

## :test_tube: Testing

Comprehensive testing infrastructure ensures reliability:

- **Unit Tests**: Core component validation (`/test/unit/`)
- **Integration Tests**: End-to-end functionality (`/test/integration/`)
- **Performance Tests**: Benchmarking and optimization (`/test/performance/`)
- **Evaluation Tests**: Protocol compliance (`/test/eval/`)

See [test documentation](/test/README.md) for detailed testing procedures.

---

## :gear: Technical Architecture

### Process Model

```text
+----------------+---------------+---------------+----------------+
| Server Process | AS Process 1  | AS Process x  | Result Process |
|                | Python Interp.| Python Interp.|                |
+-----------------------------------------------------------------+
| Shared Memory                                                   |
|  - StaticFS Requests                                            |
|  - AS Metadata                                                  |
|  - AS Requests                                                  |
|  - AS Results                                                   |
+-----------------------------------------------------------------+
```

**Design Principles:**
- **Process-Based**: Separate processes avoid Python GIL limitations
- **Shared Memory**: High-performance IPC with atomic locks (kernel mutex-less)
- **Huge Pages**: Memory optimization for better performance

### Performance Philosophy: Epoll vs Coroutines

**⚠️ Coroutine Warning**: While trending, coroutines can be **counterproductive** for scalability if implemented incorrectly.

**The Problem with incorrectly implemented Coroutines:**
- In case a single connection awaits a syscall `read()` result
- Its similar to one `poll()`  for a single connection file descriptor
- 10,000 connections = 10,000 syscalls (context switches)
- Massive overhead for checking received data (unscalable)

**Our Epoll Advantage:**
- **Single syscall** informs about multiple FDs with pending data
- Used by nginx and other high-performance servers

---

## :page_with_curl: Documentation

### Official Documentation
- **Build Instructions**: [BUILD.md](BUILD.md)
- **Testing Guide**: [test/README.md](/test/README.md)  
- **Technical Specifications**: [specs/md/README.md](/specs/md/README.md)
- **Feature Comparison**: [FEATURE-MATRIX.md](FEATURE-MATRIX.md)

### Online Resources
- **CI Documentation**: [/doc/README.md](/doc/README.md)
- **RFP/RFC Specs**: [http://docs.webcodex.de/wacp/rfp/](http://docs.webcodex.de/wacp/rfp/)
- **Detailed Analysis**: [Der IT Prüfer](http://der-it-pruefer.de)

### Protocol Specifications
- **WAP/XML Specs**: [WAP-AS-XML-SPECS.md](WAP-AS-XML-SPECS.md)
- **Internal Libraries**: [lib/README.md](lib/README.md)

---

## :globe_with_meridians: Community & Support

### Get Help & Discuss
- **Issues**: [GitHub Issues](https://github.com/WEBcodeX1/http-1.2/issues) - Bug reports and feature requests
- **Email**: [http1-2@webcodex.de](mailto:http1-2@webcodex.de) - Questions and improvement ideas
- **Security**: [SECURITY.md](SECURITY.md) - Security vulnerability reporting

### Stay Updated
- **Documentation**: [Official Docs](http://docs.webcodex.de/wacp/rfp/)
- **RFC Updates**: [Der IT Prüfer](http://der-it-pruefer.de) - Detailed protocol analysis

---

## :wave: Contributing

We welcome contributions! Whether you're:
- 🐛 **Reporting bugs**
- 💡 **Suggesting features** 
- 📝 **Improving documentation**
- 🔧 **Submitting code**

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

**Code Quality Standards:**
- Clean, readable, maintainable code
- Comprehensive testing
- Security-conscious development
- Commit signing with GPG/SSH keys

---

## :alarm_clock: Future Milestones

### Short-term Goals
- [ ] **x0 Framework Integration**: Seamless SPA framework compatibility
- [ ] **Performance Benchmarks**: Comprehensive HTTP/1.1, HTTP/2, HTTP/3 comparisons

### Long-term Vision
- [ ] **Protocol Standardization**: Submit HTTP/1.2 as official RFC
- [ ] **Enterprise Features**: Advanced monitoring, analytics, management
- [ ] **Proxy Server Component**: Complete the load balancing/reverse proxy module

See [GitHub Milestones](https://github.com/WEBcodeX1/http-1.2/milestones) for detailed roadmap.

---

## :memo: License

**GNU Affero General Public License v3.0** - See [LICENSE](LICENSE) for details.

---

<p align="center">
  <strong>🚀 Ready to experience the future of web protocols?</strong><br>
  <sub>Made with ❤️ by Claus Prüfer / clickIT / WEBcodeX</sub>
</p>
