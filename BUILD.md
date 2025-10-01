# 1. Prerequisites

Basic Prerequisites (Ubuntu 22.04 or Debian 12):
- git
- cmake

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

## 1.3. Compile / Install

On installation "testapp1.local" and "testapp2.local" with address 127.0.0.1 will
be added to ```/etc/hosts```. A startup script for your Linux init system (systemd, OpenRC, 
or SysVinit) will also be automatically installed to ```/etc/systemd/system/``` or ```/etc/init.d/```.

```bash
# compile / install
cmake CMakeLists.txt .
make
make install
```

```bash
# debug build
cmake -DDEBUG_BUILD=1 CMakeLists.txt .
make
make install
```

```bash
# java backend build
export JAVA_HOME=/usr/lib/jvm/jdk-24.0.2-oracle-x64/
cmake -DJAVA_BACKEND=1 CMakeLists.txt .
make
make install
```

## 1.4. Start Server

After installation, the appropriate startup script is automatically installed for your init system.
The startup script already includes ulimit settings for open files and kernel hugepages configuration.

**Using systemd (Ubuntu 22.04+, Debian 12+):**
```bash
# enable and start service
sudo systemctl daemon-reload
sudo systemctl enable falcon-as
sudo systemctl start falcon-as

# check status
sudo systemctl status falcon-as
```

**Using OpenRC (Devuan, Gentoo, Alpine):**
```bash
# enable and start service
sudo rc-update add falcon-as default
sudo rc-service falcon-as start

# check status
sudo rc-service falcon-as status
```

**Using SysVinit (Debian ≤11, older systems):**
```bash
# enable and start service
sudo update-rc.d falcon-as defaults
sudo service falcon-as start

# check status
sudo service falcon-as status
```

**Manual start (without service):**
```bash
# start server manually
. ./scripts/ulimit.sh
. ./scripts/set-transparent-hugepages.sh
/usr/local/bin/falcon-as
```

Stop server with signal SIGTERM (```pkill falcon-as```).

## 1.5. Check Server Working

### 1.5.1. Check GET requests

Open http://testapp1.local/index.html or http://testapp2.local/index.html in a local browser
to check if setup is working correctly.

### 1.5.2. Check POST requests

```bash
# test post request
wget --post-data='{"payload": 123}' --header='Content-Type: application/json' http://testapp1.local/backend/
```
