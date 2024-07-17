# Prerequisites
## C++ Boost Libraries

The following Boost Libraries ```libboost1.74-dev``` (Ubuntu 22.04) are used. 

- Boost Unittest
- Boost Regex
- Boost Python

To install complete boost development files under Ubuntu 22.04 do:

```bash
# apt-get install libboost-all-dev
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
# cmake build
# cmake install
```
