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

## Required Actions

Look into the 'examples' directory. This contains some examples of the required file structure for FRED to work, which must be present in the working directory when running FREDServer binary.

1. Create 'config/fred.conf' with your FREDServer name, and the network address of your DIM DNS server (nominally hosted on the same machine that runs FREDServer)
2. Create 'sections', 'boards', 'boards/yourdevice', and 'boards/yourdevice/commands' directories
3. Create config files corresponding to your hardware design, including 'sections/yourgenericdevice.section', 'boards/yourdevice/yourdevice.conf'
4. Create sequence files that correspond to the instruction sequence to be executed by your hardware in e.g. 'boards/yourdevice/commands/readsequence.sqc', etc.

## Run FRED

You have to be in FREDServer directory

./bin/FREDServer

## Examples

FREDServer/examples contains:
*  SCA, SWT and MAPI examples
*  config directory for FRED configuration

To use an example with your own instance of FRED, copy recursively the content of the desired example and the 
config directory to FREDServer directory. (Be sure to check the file contents and modify them according to your setup.)
