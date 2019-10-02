# FREDServer

## Version History
* v2.0.1 - Updated examples, and other minor improvements
* v2.0 - Moves compatibility to O2 ALF (from 'alisw-el7' repository/flp-suite Ansible install) - N.B. NOT backwards compatible
  - Existing 'section' files must be modified to reflect new ALF DIM server name (from ALFID to ALF_HOSTNAME)
  - Existing SWT MAPI implementations must be modified to reflect the new protocol (see: [ALF Readme on GitHub](https://github.com/AliceO2Group/ALF/blob/master/README.md))
* v1.9 - Extends MAPI capabilities, including iterative requests to ALF (Iterativemapi), and custom grouping of topics (Mapigroup)
  - New booleans: 
    - 'noReturn' (do not publish to WinCC)
    - 'noRpcRequest' (do not send request to ALF)
    - 'returnError' (publish via _ERR instead of _ANS)
  - New functions: 
    - 'newRequest(string)' (call this MAPI's 'processInputMessage()')
    - 'publishAnswer(string)' & 'publishError(string)' (publish string to _ANS & _ERR for WinCC)
    - 'newMapiGroupRequest(vector<topicname, input>)' (call these topics with their inputs)
  - See READMEs and graphics in examples/Mapi for more details
* v1.8 - Adds optional '--log \<path\>' parameter to the command line for direct logging to file
* v1.7.1 - Adds Topic names to relevant output lines (more informative)
* v1.7 - Adds Error channel: _ERR service joins _REQ and _ANS
  - _ERR will publish errors from ALF as a string, _ANS will not be updated
* v1.6 - Adds '--help', '--parser', and '--verbose' modes, minor bugfixes
  - '--parser' mode will process and validate your configs, then exit without running
* v1.5 - Adds 'Mask' block for masking devices from Group commands
* v1.4 - Changes the way section files are structured to be much cleaner
  - Removes 'ALFS' and 'TOPICS': info now processed from 'MAPPING' and board config, respectively
* v1.3.1 - Bugfix for carriage-return characters in configs
* v1.3 - Adds direct access to CRU registers (r/w), adds helpful parsing errors

## Prerequisites

cmake3 devtoolset-7

## Installation

```
git clone https://gitlab.cern.ch/alialfred/FREDServer.git

cd FREDServer

source scl_source enable devtoolset-7

# Before building, you can add your MAPI, including adding the lines to 'main.cpp'

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

You have to be in the directory containing your configs, then:
```
/path/to/bin/FREDServer [--help] [--log <path>] [--parser] [--verbose]
```
## Examples

FREDServer/examples contains:
*  SCA, SWT and MAPI examples
*  config directory for FRED configuration

To use an example with your own instance of FRED, copy recursively the content of the desired example and the 
config directory to FREDServer directory. (Be sure to check the file contents and modify them according to your setup.)
