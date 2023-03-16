# Seperate data into hdf5 group structure:
#   'hamming', 
#   'queries' ('points', 'answers')
import sys
import time
import multiprocessing
import h5py
import numpy as np
from sklearn.model_selection import train_test_split

# Class for finding k nearest neighbours by brute force
class BFkNN:
  def __init__(self, data_points, k):
    self.data_points = data_points
    self.k = k
    
  def find_knn(self, query_point) -> [int]:
    arr = []
    for i in range(len(self.data_points)):
      point = self.data_points[i]
      zipped = zip(query_point, point)
      arr.append(
        (
          sum([
            (x ^ y).bit_count()
            for (x, y) in zipped
          ]),
          i
        )
      ) 
    return list(
      map(
        lambda p: p[1], # Return index
        sorted(arr)[:self.k] # Sort by distance
      )
    )

def mk_ans(data_points, query_points, kn):
  # Build bruteforce solution
  bfknn = BFkNN(data_points, kn)
  

  pool = multiprocessing.Pool()
  ans = pool.map(bfknn.find_knn, query_points) # Make the queries in parralel
  
  return ans


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
data_points, query_points = train_test_split(dataset, test_size=0.1, shuffle=False)


print("[+] Generating answers for", len(query_points), "queries...")
start = time.time()
ans = mk_ans(data_points, query_points, k)
print("[+] Time: ", (time.time()-start))

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