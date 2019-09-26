#!/bin/sh
ls | grep -v cc.sh | grep -v libcrypto.so.1.1 | grep -v libssl.so.1.1 | xargs rm
ls | xargs echo
get appweb.tar.gz
tar zxvf appweb.tar.gz
rm -f appweb.tar.gz