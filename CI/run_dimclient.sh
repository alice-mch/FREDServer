#!/bin/bash

echo "Run DIMClient Bash script started"

cd /builds/alialfred/FREDServer/DIMClient
export DIM_DNS_NODE=localhost

bin/DIMClient --F --t FRED1/POWER_BOARDS/POWERUNIT1/INIT
if [ $? -ne 0 ]
then
  echo "FRED TOPIC test failed"
  exit 1
else
  echo "FRED TOPIC test succeeded
  sleep 5
fi

bin/DIMClient --F --t FRED1/POWER_BOARDS/POWERUNIT4/VOLTAGE/READ --m 33
if [ $? -ne 0 ]
then
  echo "FRED TOPIC test failed"
  exit 1
else
  echo "FRED TOPIC test succeeded
  sleep 5
fi

bin/DIMClient --F --t FRED1/RU_SCA/ITSRU0/SCATEMP/READ
if [ $? -ne 0 ]
then
  echo "FRED TOPIC test failed"
  exit 1
else
  echo "FRED TOPIC test succeeded
  sleep 5
fi

bin/DIMClient --F --t FRED1/INNER_BARREL/STAVES/READ_ALL
if [ $? -ne 0 ]
then
  echo "FRED TOPIC test failed"
  exit 1
else
  echo "FRED TOPIC test succeeded
  sleep 5
fi

bin/DIMClient --F --t FRED1/RU_SCA/ITSRU0/SCATEMP/READ --m 123456
if [ $? -ne 0 ]
then
  echo "FRED TOPIC error test succeeded"
  exit 0
else
  echo "FRED TOPIC error test failed"
  exit 1
fi
