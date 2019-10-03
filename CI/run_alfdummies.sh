#!/bin/bash

echo "Run ALFDummies Bash script started"

export DIM_DNS_NODE=localhost
/builds/alialfred/FREDServer/ALFDummies/FLP042/ALFDummy/bin/ALFDummy --a FLP042 --l 10 --s 10 --v &
/builds/alialfred/FREDServer/ALFDummies/FLP001/ALFDummy/bin/ALFDummy --a FLP001 --l 10 --s 10 --v &
