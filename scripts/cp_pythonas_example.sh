#!/bin/sh

python_version=$(find /usr/local/lib/python* | head -1 | tr -d -c 0-9.)
dst_path="/usr/local/lib/python${python_version}/dist-packages"

echo "Found Python Version:${python_version} DST Path:${dst_path}"

cp ./www/WebApp.py ${dst_path}/

