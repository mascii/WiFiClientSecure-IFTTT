#!/bin/bash

cat <(echo "const char* test_ca_cert = \\") <(cat $1 | tr -d "\r" | sed -e "s/^/\"/" -e "s/$/\\\n\" \\\/") <(echo ";") > certificate.h

exit 0
