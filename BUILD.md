# 1. Prerequisites

Basic Prerequisites (Ubuntu 22.04 or Debian 12):
- git
- cmake
- python pip

```bash
# install base requirements
apt-get install git cmake python3-pip
```

## 1.1. C++ Boost Libraries

The following Boost Libraries ```libboost1.74-dev``` (Ubuntu 22.04) are used. 

- Boost Unittest
- Boost Python

To install complete boost development files under Ubuntu 22.04 do:

```bash
# install all boost components
apt-get install libboost-all-dev
```

## 1.2. Python Dev (Interpreter Source)

If not done by Boost Python Library, the Python Interpreter Source Code is required for embedding. 

```bash
# install python dev headers / source
apt-get install python3-dev
```

## 1.3. Python XML Microparser

We use our external Python XML Parser to bridge XML config to C++ using Boost Python library.

```bash
# install xmlmicroparser python module
pip3 install xmlmicroparser
```

Details (e.g. building module) see [XML Parser Documentation](https://github.com/clauspruefer/python-xml-microparser/blob/master/README.md).

## 1.4. Compile / Install

```bash
# compile / install
cmake CMakeLists.txt
make
make install
```

On installation "testapp1.local" and "testapp2.local" with address 127.0.0.1 will
be added to ```/etc/hosts```.

## 1.5. Start Server

Raise ulimit for open files before starting server. This will be put inside server startup
script later.

```bash
# start server
. ./scripts/ulimit.sh
/usr/local/bin/falcon-as
```

Stop server with signal SIGTERM (```kill http-1.2```).

## 1.6. Check Server Working

### 1.6.1. Check GET requests

Open http://testapp1.local or http://testapp2.local in a local browser to check if
installation has succeeded.

### 1.6.2. Check POST requests

```bash
# test post request
wget --post-data='{"payload": 123}' --header='Content-Type: application/json' http://testapp1.local/python/test
```
