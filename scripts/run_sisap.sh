#!/bin/bash

/swann/build/sisap

# Format H5
python3 /swann/scripts/format_hdf5.py /swann/result/results_test.h5

# Run eval
python3 /swann/eval/eval.py

cd /swann/eval
python3 ./plot.py /swann/result/res.csv