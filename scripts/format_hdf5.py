import sys

import h5py

# Expects filename
fp = sys.argv[1]

f = h5py.File(fp, 'a')
f.attrs['algo'] = f.attrs['algo'].decode('ascii')
f.attrs['data'] = f.attrs['data'].decode('ascii')
f.attrs['size'] = f.attrs['size'].decode('ascii')
f.attrs['params'] = f.attrs['params'].decode('ascii')
f.close()