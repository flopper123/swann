# Seperate data into hdf5 group structure:
#   'hamming', 
#   'queries' ('points', 'answers')
import sys
import os
import time
import multiprocessing
from typing import List
import h5py
import numpy as np
import random
from sklearn.model_selection import train_test_split, ShuffleSplit

import tqdm
from tqdm.contrib.concurrent import thread_map


# Class for finding k nearest neighbours by brute force
class BFkNN:
  def __init__(self, data_points, k):
    self.data_points = data_points
    self.k = k

  def find_knn(self, query_point) -> List[int]:
    arr = []

    for point in self.data_points:
      zipped = zip(query_point, point)
      arr.append(
        sum([
          (x ^ y).bit_count()
          for (x, y) in zipped
        ])
      )
    
    # Return the k shortest distances
    return sorted(arr)[:self.k]

def mk_ans(data_points, query_points, kn):
  # Build bruteforce solution
  bfknn = BFkNN(data_points, kn)
  
  with multiprocessing.Pool() as pool:
    ans = list(tqdm.tqdm(pool.imap(bfknn.find_knn, query_points), total=len(query_points), file=sys.stdout)) # Make the queries in parallel

  return ans

# Number of nearest neighbors to find
k = 100

h5filename = sys.argv[1]
# Open h5file with read & write permissions
h5 = h5py.File(h5filename,'r+') 

# Check if preprocess has been performed
print("[+] Checks if file has been preprocessed")

if 'queries' in h5:
  sys.exit()

print("[+] Preprocessing initiated", flush=True)

# Data is encoded as R*C with R points encoded over C dimensions of ui64
dataset = h5['hamming']
dataset_10k_fraction = 10000 / len(dataset)

print("[+] Shuffling dataset...", flush=True)

start = time.time()

random.seed(420)
random.shuffle(dataset)

print("[+] Shuffle took: ", int(time.time()-start), "s", flush=True)

print("[+] Splitting dataset...", flush=True)
data_points, query_points = train_test_split(dataset, test_size=dataset_10k_fraction, shuffle=False)


print("[+] Generating answers for", len(query_points), "queries...")
start = time.time()
ans = mk_ans(data_points, query_points, k)

print("[+] Brute force took: ", int(time.time()-start), "s")

# Override content of 'hamming' group with new data without the query points
del h5['hamming']
h5.create_dataset('hamming', data=data_points, dtype=np.uint64, compression="gzip")

# Create groups & subgroups
query_group = h5.create_group('queries')
query_group.create_dataset('points', data=query_points, dtype=np.uint64, compression="gzip")
query_group.create_dataset('answers', data=ans, dtype=np.uint32, compression="gzip")

# Close file
h5.close()

h5 = h5py.File(h5filename,'r') 

print("[+] H5 keys:", h5.keys())

# Data is encoded as R*C with R points encoded over C dimensions of ui64
qdset = h5['queries/points']
pdset = h5['queries/answers']
hdset = h5['hamming']

print("Queries:", qdset.shape, "\nAnswers:", pdset.shape, "\nHamming:", hdset.shape)

h5.close()
