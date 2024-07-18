#!/bin/sh

mkdir -p /var/www/app1/static/
mkdir -p /var/www/app2/static/

cp ./www/index-app1.html /var/www/app1/static/index.html
cp ./www/index-app2.html /var/www/app2/static/index.html
cp ./www/falcon-robotics.jpg /var/www/app1/static/
cp ./www/falcon-robotics.jpg /var/www/app2/static/

chown -R falcon-http:falcon-http /var/www


