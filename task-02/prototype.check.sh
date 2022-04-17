#!/usr/bin/env bash

set -xe

(
    cd py/ && cat '../ip_filter.tsv' | python3 'prototype.py' 2>&1 > 'prototype.out.txt'
    md5sum --check 'prototype.md5.txt'
)
