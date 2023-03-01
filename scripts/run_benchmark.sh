#!/bin/bash

datetime=$(date +'%s')
outputfile=/swann/benchmark/benchmark-$datetime.csv

/swann/build/benchmark --benchmark_out=$outputfile --benchmark_out_format=csv