# Prerequisites

Basic Prerequisites:
- git
- cmake

```bash
# apt-get install git cmake
```

## C++ Boost Libraries

The following Boost Libraries ```libboost1.74-dev``` (Ubuntu 22.04) are used. 

- Boost Unittest
- Boost Regex
- Boost Python

To install complete boost development files under Ubuntu 22.04 do:

```bash
# apt-get install libboost-all-dev
```

## Python Dev (Interpreter Source)

If not done by Boost Python Library, the Python Interpreter Source Code is required for embedding. 

```bash
# apt-get install python3-dev
```

## Python XML Microparser

We use our external Python XML Parser to bridge XML config to C++ using Boost Python library.

### Clone Repository

```bash
# cd repodir
# git clone https://github.com/clauspruefer/python-xml-microparser.git
```

### Install Globally (force)

```bash
# python3 setup.py
```

## Compile / Install

```bash
# cmake CMakeLists.txt
# cmake build
# cmake install
```
