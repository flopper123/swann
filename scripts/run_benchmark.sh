#!/bin/bash

datetime=$(date +'%s')
outputfile=/swann/benchmark/benchmark-$datetime.csv

DEBUG_DEFAULT=false
DEBUGGING=${DEBUG:-$DEBUG_DEFAULT}

# Check if DEBUGGING is not false
if [ "$DEBUGGING" != false ]; then
  /swann/build/benchmark --benchmark_out=$outputfile --benchmark_out_format=csv
else 
  /swann/build/benchmark --benchmark_out=$outputfile --benchmark_out_format=csv 2>&1 >/dev/null
fi
