<h1 align="center">🚀 HTTP/1.2 - FalconAS</h1>
<p align="center"><em>Lightning-Fast, Rock-Solid Python Application Server with HTTP/1.2 Protocol</em></p>

<div align="center">
  <a href="https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql">
    <img src="https://github.com/WEBcodeX1/http-1.2/actions/workflows/github-code-scanning/codeql/badge.svg" alt="CodeQL">
  </a>
  <a href="https://github.com/WEBcodeX1/http-1.2/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/WEBcodeX1/http-1.2?style=flat" alt="License">
  </a>
  <a href="http://docs.webcodex.de/wacp/rfp/">
    <img src="https://img.shields.io/badge/docs-webcodex-blue" alt="Docs">
  </a>
  <a href="https://github.com/WEBcodeX1/http-1.2/releases">
    <img src="https://img.shields.io/github/v/release/WEBcodeX1/http-1.2?include_prereleases" alt="Release">
  </a>
</div>

---

## :pushpin: Overview

**HTTP/1.2** is a revolutionary approach to web protocols, designed as a simpler, more secure, and significantly faster alternative to HTTP/2 and HTTP/3. While Google engineers push HTTP/3, and HTTP/2 remains problematic with major implementation flaws and performance issues, we've built **HTTP/1.2** - a much cleaner solution.

- **Lightning-Fast Performance**: Optimized for modern hardware with epoll-based architecture
- **Rock-Solid Security**: Logical separation via TCP ports, avoiding single-point vulnerabilities
- **Infinitely Scalable**: Built for modern browser applications (SPAs) and cloud-native deployments
- **Zero Bloat**: Eliminates unnecessary complexity while maintaining compatibility
- **Python-Powered**: FalconAS application server with embedded Python scripting

## :bookmark_tabs: Table of Contents

1. [Quick Start](#racehorse-quick-start)
2. [Features](#star2-features)
3. [Why HTTP/1.2?](#mega-why-http12)
4. [Build & Installation](#hammer_and_wrench-build--installation)
5. [Testing](#test_tube-testing)
6. [HTTP/2 vs HTTP/3 Analysis](#chart_with_upwards_trend-http2-vs-http3-analysis)
7. [Proof of Concept](#bulb-proof-of-concept)
8. [Technical Architecture](#gear-technical-architecture)
9. [Documentation](#page_with_curl-documentation)
10. [Community & Support](#globe_with_meridians-community--support)
11. [Contributing](#wave-contributing)
12. [Future Milestones](#alarm_clock-future-milestones)
13. [License](#memo-license)

---

## :racehorse: Quick Start

Get HTTP/1.2 FalconAS running in minutes:

```bash
# Clone repository
git clone https://github.com/WEBcodeX1/http-1.2.git
cd http-1.2

# Install dependencies (Ubuntu 22.04/Debian 12)
apt-get install git cmake python3-pip libboost-all-dev python3-dev
pip3 install xmlmicroparser

# Build and install
cmake CMakeLists.txt .
make
make install

# Configure system
. ./scripts/ulimit.sh
. ./scripts/set-transparent-hugepages.sh

# Start server
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
- **🔒 Secure by Design**: Logical TCP port separation prevents attack vectors
- **⚡ Zero Bloat**: Eliminates HTTP/2's excessive complexity
- **🎯 SPA-Optimized**: Perfect for modern Single Page Applications
- **🐍 Python-Powered**: Embedded Python scripting with Boost integration
- **☁️ Cloud-Ready**: Application server-friendly and proxy server-friendly
- **♾️ Infinitely Scalable**: Built for modern hardware and infrastructure

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
- **XML Configuration**: Clean, maintainable configuration system

---

## :mega: Why HTTP/1.2?

### The Problem with HTTP/2 & HTTP/3

**HTTP/2 Complexity Crisis:**
- Excessive complexity destroys HTTP/1.1's elegant simplicity
- Everything packed into one "black box" without logical separation
- Single TCP port creates security vulnerabilities
- Libraries are confusing and difficult to understand
- TLS/SSL handling unnecessarily embedded in protocol

**HTTP/3 UDP Issues:**
- UDP-based approach unsuitable for real-time applications
- Works only for precalculated CDN data
- Creates new complexity without solving core issues

### Our Solution: HTTP/1.2

HTTP/1.2 **fixes** HTTP/1.1's single major flaw (broken pipelining) while **avoiding** HTTP/2's complexity disasters:

> **🎯 Key Innovation**: Adding a simple `Request-UUID` header eliminates response ordering issues, making HTTP/2's complex Layer-7 multiplexing unnecessary.

This renders HTTP/2 and HTTP/3 **obsolete** while drastically reducing complexity.

---

## :hammer_and_wrench: Build & Installation

### Prerequisites

**Operating System Support:**
- Ubuntu 22.04 (Jammy Jellyfish) 
- Ubuntu 24.04 (Noble Numbat)
- Debian 12

**Required Dependencies:**
```bash
# Core build tools
apt-get install git cmake python3-pip

# C++ Boost libraries
apt-get install libboost-all-dev

# Python development headers
apt-get install python3-dev

# XML parsing module
pip3 install xmlmicroparser
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

**Test Types:**
- **Unit Tests**: Core component validation (`/test/unit/`)
- **Integration Tests**: End-to-end functionality (`/test/integration/`)
- **Performance Tests**: Benchmarking and optimization (`/test/performance/`)
- **Evaluation Tests**: Protocol compliance (`/test/eval/`)

**Running Tests:**
```bash
# Build with tests
make test

# Run specific test suites
cd test/
cmake .
make
./unit_tests
./integration_tests
```

See [test documentation](/test/README.md) for detailed testing procedures.

---

## :chart_with_upwards_trend: HTTP/2 vs HTTP/3 Analysis

### :warning: HTTP/2 Complexity Issues

HTTP/2 introduces excessive complexity. The simplicity of HTTP/1.1 is completely
lost. Open-source libraries are often confusing and difficult to understand, even
when using libevent.

Everything is packed into one large box, lacking logical separation and security.
All traffic is transmitted over a single TCP port, which poses significant risks.
For instance, an IPS process can be killed by a zero-day vulnerability inside a
TCP packet.

Additionally, incorporating TLS/SSL handling directly into the protocol is no
longer appropriate. Instead, this should be handled by separate components like
nginx (reverse proxy), ingress-nginx on Kubernetes, stunnel, or a hardware
load balancer.

Avoid duplicating functionality when better solutions have existed for decades.

HTTP/2 introduces **excessive complexity** that completely destroys HTTP/1.1's elegant simplicity:

- **Monolithic Design**: Everything packed into one "black box" without logical separation
- **Security Vulnerabilities**: Single TCP port creates attack vectors (e.g., IPS process killed by zero-day in TCP packet)  
- **Library Confusion**: Open-source implementations are confusing and difficult to understand
- **Misplaced TLS Integration**: SSL/TLS handling embedded in protocol instead of using proven solutions

> **🚫 Architecture Anti-Pattern**: Avoid duplicating functionality when better solutions (nginx, stunnel, hardware load balancers) have existed for decades.

### :gear: HTTP/1.1 Pipelining - The Real Issue

HTTP/2 was created to solve HTTP/1.1's **single major flaw**: broken pipelined connections.

**The Problem:**
- Browsers removed pipelining due to misordered request/reply pairs
- Instead of fixing this small issue, HTTP/2 copied TCP/IP Layer-3 features into Layer-7

**Our Solution:**
HTTP/1.2 fixes this with minimal changes:
```http
Request-UUID: 550e8400-e29b-41d4-a716-446655440000
```

Every request includes a unique identifier, making response ordering unnecessary. **This renders HTTP/2's complex multiplexing obsolete.**

> **⚡ Result**: HTTP/1.2 achieves HTTP/2's benefits while maintaining HTTP/1.1's simplicity.

### :no_entry_sign: HTTP/3 UDP Problems

HTTP/3's UDP-based approach creates more problems than it solves:

- **Limited Use Cases**: Only suitable for precalculated CDN data, not real-time applications
- **False Claims**: Assertions about TCP/IP retransmission blocking are incorrect - issues arise from OSI upper layers, not Layer-3
- **Added Complexity**: Creates new complexity without addressing core architectural issues

Details: [Exemplary HTTP Network Processing](http://der-it-pruefer.de/)

---

## :bulb: Proof of Concept

**FalconAS** - Our working HTTP/1.2 server implementation includes:

### Current Components
- **🗂️ Static Content Server**: High-performance file serving with `sendfile()` 
- **🐍 Application Server**: Python scripting with embedded interpreter
- **🔄 Shared Memory Architecture**: Process-based design avoiding Python GIL
- **⚡ Epoll Integration**: Lightning-fast connection handling

### Planned Components  
- **🔀 Proxy Server**: Load balancing and reverse proxy capabilities (in development)

### Primary Milestone

The **primary goal** is providing a lightning-fast, rock-solid, and secure Python application server component specifically optimized for:

- **Modern Browser Applications** (Single Page Applications)
- **Cloud-Native Deployments** (Kubernetes, Docker)  
- **High-Performance Workloads** (utilizing modern hardware capabilities)

FalconAS will serve as a key component of the [x0 JavaScript Framework](https://github.com/WEBcodeX1/x0) ecosystem.

---

## :gear: Technical Architecture

### Process Model

```text
┌─────────────────┬─────────────────┬─────────────────┬─────────────────┐
│ Server Process  │ AS Process 1    │ AS Process x    │ Result Processor│
│                 │ Python Interp.  │ Python Interp.  │                 │
├─────────────────┴─────────────────┴─────────────────┴─────────────────┤
│ Shared Memory                                                         │
│  • StaticFS Requests                                                  │
│  • AS Metadata                                                        │  
│  • AS Requests                                                        │
│  • AS Results                                                         │
└───────────────────────────────────────────────────────────────────────┘
```

**Design Principles:**
- **Process-Based**: Separate processes avoid Python GIL limitations
- **Shared Memory**: High-performance IPC with atomic locks (kernel mutex-less)
- **Huge Pages**: Memory optimization for better performance

### Performance Philosophy: Epoll vs Coroutines

**⚠️ Coroutine Warning**: While trending, coroutines can be **counterproductive** for scalability if implemented incorrectly.

**The Problem with Coroutines:**
- Main loop iterates over **ALL** active/idle connection file descriptors
- 10,000 keep-alive connections = 10,000 syscalls (context switches)
- Massive overhead for checking received data

**Our Epoll Advantage:**
- **Single syscall** informs about multiple FDs with pending data
- Optimized for 64-bit server CPUs and modern hardware
- Used by nginx and other high-performance servers
- **Lightning-fast performance** with proper implementation

> **💡 Key Insight**: Technologies like nginx achieve superior performance by leveraging epoll's efficiency rather than iterating through connections.

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
- [ ] **Proxy Server Component**: Complete the load balancing/reverse proxy module
- [ ] **Kubernetes Integration**: Native cloud-native deployment support  
- [ ] **x0 Framework Integration**: Seamless SPA framework compatibility
- [ ] **Performance Benchmarks**: Comprehensive HTTP/1.1, HTTP/2, HTTP/3 comparisons

### Long-term Vision
- [ ] **Protocol Standardization**: Submit HTTP/1.2 as official RFC
- [ ] **Enterprise Features**: Advanced monitoring, analytics, management
- [ ] **Multi-Language Support**: Additional backend language bindings
- [ ] **Hardware Optimization**: Leverage latest CPU features (Intel XEON 6)

See [GitHub Milestones](https://github.com/WEBcodeX1/http-1.2/milestones) for detailed roadmap.

---

## :memo: License

**GNU Affero General Public License v3.0** - See [LICENSE](LICENSE) for details.

---

<p align="center">
  <strong>🚀 Ready to experience the future of web protocols?</strong><br>
  <sub>Made with ❤️ by Claus Prüfer / clickIT / WEBcodeX</sub>
</p>
