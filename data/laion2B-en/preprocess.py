
import sys

import h5py
import numpy as np
from sklearn.model_selection import train_test_split

# TODO:
# Read h5 data points
# Use sklearn to seperate data into query points and dataset points
# Create simple python bruteforce to find the x true nearest neighbors of each query point
# Save the results in h5 file

h5filename = sys.argv[1]
h5 = h5py.File(h5filename,'r+')
print(h5.keys())

group = h5['hamming']



X, y = np.arange(10).reshape((5, 2)), range(5)
print(X)

print(list(y))


print(train_test_split(y, shuffle=False))

