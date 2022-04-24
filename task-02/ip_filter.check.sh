#!/usr/bin/env bash

set -xe

(
    cd cxx/ && cat '../ip_filter.tsv' | ./ip_filter 2>&1 > 'ip_filter.out.txt'
    md5sum --check 'ip_filter.md5.txt'
)
