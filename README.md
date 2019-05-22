# FREDServer

## Prerequisites

cmake3 devtoolset-7

## Installation

```
git clone https://gitlab.cern.ch/alialfred/FREDServer.git

cd FREDServer

source scl_source enable devtoolset-7

cmake3 .

make all
```



## Run

You have to be in FREDServer directory

./bin/FREDServer

## Examples

FREDServer/examples contains:
*  SCA, SWT and MAPI examples
*  config directory for FRED configuration

To use an example, copy recursively the content of the desired example and the 
config directory to FREDServer directory.
