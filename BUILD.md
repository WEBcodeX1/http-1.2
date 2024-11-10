# 1. Prerequisites

Basic Prerequisites (Ubuntu 22.04 or Debian 12):
- git
- cmake
- python setup-tools
- python pip

```bash
# apt-get install git cmake python3-setuptools python3-pip
```

## 1.1. C++ Boost Libraries

The following Boost Libraries ```libboost1.74-dev``` (Ubuntu 22.04) are used. 

- Boost Unittest
- Boost Python

To install complete boost development files under Ubuntu 22.04 do:

```bash
# apt-get install libboost-all-dev
```

## 1.2. Python Dev (Interpreter Source)

If not done by Boost Python Library, the Python Interpreter Source Code is required for embedding. 

```bash
# apt-get install python3-dev
```

## 1.3. Python XML Microparser

We use our external Python XML Parser to bridge XML config to C++ using Boost Python library.

```bash
pip3 install xmlmicroparser
```

Or either download package from https://github.com/clauspruefer/python-xml-microparser/archive/refs/tags/0.54beta.tar.gz
or build manually (see [XML Parser Documentation](https://github.com/clauspruefer/python-xml-microparser/blob/master/README.md)).

### 1.3.1. Install Downloaded Package

Install globally as root.

```bash
# sudo pip3 install ./python-xml-microparser-0.54beta.tar.gz --break-system-packages
```

>[!IMPORTANT]
> Some python3 pip versions do not allow installing packages globally anymore, the --break-system-packages flag is mandatory.

## 1.4. Compile / Install

```bash
# cmake CMakeLists.txt
# make
# make install
```

On installation "testapp1.local" and "testapp2.local" with address 127.0.0.1 will
be added to ```/etc/hosts```.

## 1.5. Start Server

Raise ulimit for open files before starting server. This will be put inside server startup
script later.

```bash
# . ./scripts/ulimit.sh
# /usr/local/bin/falcon-as
```

Stop server with signal SIGTERM (```kill http-1.2```).

## 1.6. Check Server Working

### 1.6.1. Check GET requests

Open http://testapp1.local or http://testapp2.local in a local browser to check if
installation has succeeded.

### 1.6.2. Check POST requests

```bash
# wget --post-data='{"payload": 123}' --header='Content-Type: application/json' http://testapp1.local/python/test
```
