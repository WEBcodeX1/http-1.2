#!/bin/sh

if [ -z "$(cat /etc/hosts | grep testapp1.local)" ]
then
	echo "127.0.0.1		testapp1.local" >> /etc/hosts
fi

if [ -z "$(cat /etc/hosts | grep testapp2.local)" ]
then
	echo "127.0.0.1		testapp2.local" >> /etc/hosts
fi

