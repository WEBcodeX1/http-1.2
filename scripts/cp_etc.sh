#!/bin/sh

mkdir -p /etc/falcon-http
chown falcon-http:falcon-http /etc/falcon-http
chmod 750 /etc/falcon-http
cp ./config/config.json /etc/falcon-http/
chmod 550 /etc/falcon-http/*
chown -R falcon-http:falcon-http /etc/falcon-http/*
