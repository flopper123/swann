
import sys

import h5py

# TODO:
# Read h5 data points
# Use sklearn to seperate data into query points and dataset points
# Create simple python bruteforce to find the x true nearest neighbors of each query point
# Save the results in h5 file

h5filename = sys.argv[1]

h5 = h5py.File(h5filename,'r+')

print(h5.keys())

group = h5['hamming']



