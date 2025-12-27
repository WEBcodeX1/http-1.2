# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## v0.1

### In Progress - Active Milestones

#### Global CPU Binding (Milestone #15)
- **Issue #99**: Globally Bind CPU(s) to Processes
  - Processes: Server Process (Client Handling), PythonAS, ResultProcessor (Threadhandler)
  - Implementation planned at global level, not Virtual Host level
  - Labels: `global`, `priority high`

#### HTTP/1.2 Draft / RFP (Milestone #9)
- **Issue #61**: Finish "HTTP/1.2" Draft / RFP
  - Document must be rewritten completely
  - Labels: `RFP`
  - Assigned to: clauspruefer, Copilot

- **Issue #60**: Finish "Exemplary HTTP Network Processing" Document
  - Also to be published on Der-IT-Pruefer.de
  - Labels: `RFP`
  - Assigned to: clauspruefer

#### Internal Library lib-http (Milestone #3)
- **Issue #16**: Implement libhttp sub-component "net"
  - Small client library needed for performance tests and HTTP/1.2 request processing
  - Implementation using libevent in `/lib/event`
  - Labels: `library`, `HTTP/1.2`

## [v0.1rc] - 2025-10-07

### 📦 **Release 0.1rc (Milestone #17) - Completed**

#### 🐛 **Fixed**
- **Issue #150**: Unix Domain Socket Connection Unstable
  - Fixed connection stability issues in Unix Domain Socket implementation
  - Assigned to: Copilot

- **Issue #149**: Multiple IP Connections Unstable
  - Resolved instability with multiple IP connections
  - Assigned to: clauspruefer

- **Issue #147**: Multiple Exceptions Stop Client Socket Processing / Crash
  - Fixed server crashes caused by multiple exceptions in client socket processing
  - Assigned to: Copilot

- **Issue #140**: ResultProcessor Process Not Dropping Privileges
  - Security fix: ResultProcessor now properly drops privileges
  - Assigned to: clauspruefer

#### 📋 **Pull Requests Merged**
- **PR #157**: Fix Version v0.1rc
- **PR #155**: Release Version v0.1rc
- **PR #156**: Complete Sphinx documentation restructure
- **PR #153**: Fix Unix Domain Socket Processing
- **PR #151**: Fix Unix Domain Socket EAGAIN Handling in FD Passing Operations
- **PR #148**: Fix Map Access Exceptions Causing Server Crashes On Invalid Requests

## [v0.1.2] - 2025-08-03

### 📦 **Release 0.1.2 (Milestone #16) - Completed**

#### ✨ **Added**
- **Issue #119**: Add Startup Scripts (High Priority)
  - Added systemd, OpenRC, and SysVinit startup scripts
  - Assigned to: clauspruefer, Copilot

- **Issue #100**: Add PythonAS VirtualHost Configuration (High Priority)
  - Enhanced PythonAS with VirtualHost configuration support
  - Assigned to: clauspruefer

- **Issue #12**: Add 304 (Not Modified) logic to Static Server Component (High Priority)
  - Implemented HTTP 304 response handling for static files
  - Assigned to: clauspruefer

#### 🔄 **Changed**
- **Issue #117**: Use JSON Configuration / JSON Parser
  - Migrated from XML to JSON-based configuration system
  - Labels: `enhancement`, `config`

- **Issue #101**: Move C++ Source To /src (High Priority)
  - Reorganized source code structure for better maintainability
  - Assigned to: clauspruefer

- **Issue #102**: Strip Binary On Installation (Non Debug Build) (High Priority)
  - Optimized production builds by stripping debug symbols
  - Labels: `cosmetical`

#### 🐛 **Fixed**
- **Issue #132**: Runtime Exception Thrown On Server Start
  - Fixed critical startup exception in core server initialization
  - Labels: `bug`, `core`

- **Issue #110**: Fix / Test Termination Handling
  - Improved graceful shutdown and termination handling
  - Assigned to: clauspruefer, Copilot

- **Issue #104**: Idiotic GET / POST Parsing
  - Fixed HTTP request parsing issues for GET/POST methods
  - Assigned to: clauspruefer

- **Issue #96**: Result Processing Not Working Correctly (High Priority)
  - Resolved critical issues in result processing pipeline
  - Assigned to: clauspruefer

#### 🧪 **Testing**
- **Issue #98**: Implement ResultOrder Test (High Priority)
  - Added comprehensive testing for ResultOrder functionality
  - Labels: `test`

#### 📚 **Documentation**
- **Issue #122**: Correct README.md
  - Updated and corrected project documentation
  - Assigned to: clauspruefer

#### 📋 **Major Pull Requests**
- **PR #133**: Release Version v0.1.2
- **PR #129**: Release Version v0.1.1
- **PR #127**: Correct README.md
- **PR #118**: Replace Python Config Processing
- **PR #115**: Make GET / POST Requests Working
- **PR #107**: Version 0.1rc1 Release Pull Request

### Pending Features & Enhancements

#### High Priority
- **Issue #152**: Think About Moving ResultProcessor Into Server Thread
  - Move processing logic into Server process as single thread
  - Would eliminate need for ClientFD passing via unix domain socket
  - Labels: `enhancement`, `re-factor`

#### Medium Priority
- **Issue #72**: Implement MaxParallelExecutedThreads into ResultScheduler
  - Labels: `enhancement`

- **Issue #71**: Add Limits Handling
  - Check limits at multiple positions
  - Labels: `not done yet`, `priority low`

#### Low Priority
- **Issue #45**: Add Optional TCP-CORK / TCP-NODELAY Feature
  - Currently hardcoded TCP-NODELAY
  - TCP-CORK has 200ms latency but reduces TCP traffic
  - Labels: `enhancement`, `priority low`, `config`

- **Issue #28**: Implement Continuous Integration
  - ✅ CodeQL building and build status implemented
  - ⏳ Evaluate additional CI possibilities
  - Labels: `priority low`

### Recent Development Activity

#### Latest Commits (30 most recent)
- [b5f20c7](https://github.com/WEBcodeX1/http-1.2/commit/b5f20c719f020d8b080b1fb47ca529486acc5efb) - clauspruefer
- [2ea45e3](https://github.com/WEBcodeX1/http-1.2/commit/2ea45e3321537c4b47da8b1590ed2cf701a4c189) - clauspruefer
- [20a8b71](https://github.com/WEBcodeX1/http-1.2/commit/20a8b71f83b55d1d050a72b4292b458decfcbb3e) - clauspruefer
- [1fb8f4a](https://github.com/WEBcodeX1/http-1.2/commit/1fb8f4a535d9db9e43e13fb26889d4085fce5984) - clauspruefer
- [addb78f](https://github.com/WEBcodeX1/http-1.2/commit/addb78f0e4259eae19954218107a5f5580070730) - clauspruefer
- [3c6f479](https://github.com/WEBcodeX1/http-1.2/commit/3c6f4798030b8a5925f0e0717e21e626ea760b64) - clauspruefer
- [6f100d3](https://github.com/WEBcodeX1/http-1.2/commit/6f100d3e3b0eec3a1afba1e5e006626f35f5d914) - Copilot
- [38c62b2](https://github.com/WEBcodeX1/http-1.2/commit/38c62b2e25bc31c571cca848b952ea594d57e571) - clauspruefer
- [169cb3a](https://github.com/WEBcodeX1/http-1.2/commit/169cb3af37ffcff551112b11e2739952ad953cef) - clauspruefer
- [5e2b941](https://github.com/WEBcodeX1/http-1.2/commit/5e2b941927dbe7df99ca48df1f739e0b5f5a537c) - clauspruefer
- [d88bf45](https://github.com/WEBcodeX1/http-1.2/commit/d88bf453707f6eb266b1e4ff4ac63770904f7422) - clauspruefer
- [94897fa](https://github.com/WEBcodeX1/http-1.2/commit/94897fa2d8c177547226106e1f7fda5dab9bea47) - clauspruefer
- [e438108](https://github.com/WEBcodeX1/http-1.2/commit/e43810876ba0111ee236a4976f169f006c38107f) - clauspruefer
- [2d7169f](https://github.com/WEBcodeX1/http-1.2/commit/2d7169f3d6825d61a588501706d084df56ec7a37) - clauspruefer
- [71c12db](https://github.com/WEBcodeX1/http-1.2/commit/71c12db0678d3a0cc1b8d572a2c588139e6a8892) - clauspruefer
- [8cc4087](https://github.com/WEBcodeX1/http-1.2/commit/8cc4087c9a559785b64d707dd4d56daf5c66ee34) - clauspruefer
- [719cbe5](https://github.com/WEBcodeX1/http-1.2/commit/719cbe52f7fce8bf49b6b1c45ff9124a7a82f2d1) - clauspruefer
- [ad1d46a](https://github.com/WEBcodeX1/http-1.2/commit/ad1d46a5dd2d0d67ac63653c441b0e0fab758aaf) - clauspruefer
- [fcc79c0](https://github.com/WEBcodeX1/http-1.2/commit/fcc79c0a14841e9ea7b278bf1668968ac57ae39c) - Copilot
- [bd74ce5](https://github.com/WEBcodeX1/http-1.2/commit/bd74ce54e63a0aa34493ff645ee26d992e60c2e0) - clauspruefer
- [7150959](https://github.com/WEBcodeX1/http-1.2/commit/7150959a2846735bb783ca0db0e5ab25feb59231) - Copilot
- [c5ea074](https://github.com/WEBcodeX1/http-1.2/commit/c5ea0741c43be4601e78c04151efc2478dcbbeea) - Copilot
- [6b24013](https://github.com/WEBcodeX1/http-1.2/commit/6b240138d805a3506dfaedf6672cf1458efb09f8) - clauspruefer
- [5755f84](https://github.com/WEBcodeX1/http-1.2/commit/5755f8400e67951579f32c037a153d7cea7e8576) - clauspruefer
- [ff3414d](https://github.com/WEBcodeX1/http-1.2/commit/ff3414d6c7357a120cb26ff105f8382942eb1698) - clauspruefer
- [36552f8](https://github.com/WEBcodeX1/http-1.2/commit/36552f81cd03b738357d258f588464df872ae01b) - clauspruefer
- [f5e2dbe](https://github.com/WEBcodeX1/http-1.2/commit/f5e2dbe77e28111ea757edaa7b941ef47801894b) - clauspruefer
- [46fe425](https://github.com/WEBcodeX1/http-1.2/commit/46fe42542ac00566b885351d4dee28444f387891) - clauspruefer
- [92a022c](https://github.com/WEBcodeX1/http-1.2/commit/92a022cb6f60a16f0f7785f7736bbad574bcd2eb) - clauspruefer
- [bbaf56e](https://github.com/WEBcodeX1/http-1.2/commit/bbaf56e1dc4dcb6e3a2c17884bf15fe771509446) - clauspruefer

**Note**: This changelog shows the latest 30 commits. For a complete commit history, visit the [repository commits page](https://github.com/WEBcodeX1/http-1.2/commits).

## Project Overview

This project is developing an HTTP/1.2 implementation with focus on:
- High-performance network processing
- XML-based HTTP/1.2 client library development
- JSON-based configuration system
- Multi-backend support (Python and Java)

### Key Achievements
- ✅ **Release v0.1rc** - Unix Domain Socket fixes and documentation improvements
- 🚧 **Active Development** - HTTP/1.2 draft completion, CPU binding, and library enhancements

## How to Contribute

Please follow the [Keep a Changelog](https://keepachangelog.com/) format when updating this file:

- **Added** for new features
- **Changed** for changes in existing functionality
- **Deprecated** for soon-to-be removed features
- **Removed** for now removed features
- **Fixed** for any bug fixes
- **Security** in case of vulnerabilities

---

*Generated on 2025-10-07 - Updated with completed milestone information from #16 and #17*
