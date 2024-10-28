# Prerequisites

Basic Prerequisites (Ubuntu 22.04 or Debian 12):
- git
- cmake
- python setup-tools
- python pip

```bash
# apt-get install git cmake python3-setuptools python3-pip
```

## C++ Boost Libraries

The following Boost Libraries ```libboost1.74-dev``` (Ubuntu 22.04) are used. 

- Boost Unittest
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

### Build / Install Globally

Build as normal user.

```bash
# python3 setup.py sdist
```

Install globally as root.

```bash
# sudo pip3 install dist/xmlmicroparser-0.50b0.tar.gz
```

>[!IMPORTANT]  
> Some python3 pip versions do not allow installing packages globally anymore, the --break-system-packages flag is mandatory.

```bash
# sudo pip3 install dist/xmlmicroparser-0.50b0.tar.gz --break-system-packages
```

## Compile / Install

```bash
# cmake CMakeLists.txt
# make
# make install
```

On installation "testapp1.local" and "testapp2.local" with address 127.0.0.1 will
be added to ```/etc/hosts```.

## Start Server

Raise ulimit for open files before starting server. This will be put inside server startup
script later.

```bash
# . ./scripts/ulimit.sh
# /usr/local/bin/falcon-as
```

Stop server with signal SIGTERM (```kill http-1.2```).


## Check Server Working

### Check GET requests

Open http://testapp1.local or http://testapp2.local in a local browser to check if
installation has succeeded.

### Check POST requests

```bash
# wget --post-data='{"payload": 123}' --header='Content-Type: application/json' http://testapp1.local/python/test
```
