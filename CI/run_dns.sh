#!/bin/bash

echo "Run DIM DNS Bash script started"

cd /DIM/dim_v20r26/linux/
export DIM_DNS_NODE=localhost
export LD_LIBRARY_PATH=.
./dns &
