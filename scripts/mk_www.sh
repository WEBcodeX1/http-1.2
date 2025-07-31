#!/bin/sh

mkdir -p /var/www/app1/static/
mkdir -p /var/www/app2/static/
mkdir /var/www/app1/backend/
mkdir /var/www/app2/backend/

cp ./www/index-app1.html /var/www/app1/static/index.html
cp ./www/index-app2.html /var/www/app2/static/index.html
cp ./www/falcon-robotics.jpg /var/www/app1/static/
cp ./www/falcon-robotics.jpg /var/www/app2/static/

cp ./www/WebApp.py /var/www/app1/backend/
cp ./www/WebApp.py /var/www/app2/backend/

cp ./test/eval/JavaJNI/WebApp.class /var/www/app1/backend/
cp ./test/eval/JavaJNI/json-java.jar /var/www/app1/backend/
cp ./test/eval/JavaJNI/WebApp.class /var/www/app2/backend/
cp ./test/eval/JavaJNI/json-java.jar /var/www/app2/backend/

chown root:root /var/www
chmod 755 /var/www

chown -R falcon-http:falcon-http /var/www/app1
chown -R falcon-http:falcon-http /var/www/app2
chmod 755 /var/www/app1
chmod 755 /var/www/app2

