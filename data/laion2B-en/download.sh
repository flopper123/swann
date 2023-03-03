#!/bin/bash
# --------------------------------------------------------------------------------------
# :USAGE: $ ./download.sh ${N} 
# --------------------------------------------------------------------------------------
# :DESCRIPTION: 
#    Extracts HDF5 encoded bit vectors from the LAION2B dataset 
#    https://sisap-challenges.github.io/datasets/#1024-bit_binary_sketches
# 
#    The vectors are represented as Binary sketches of 1024-bit from laion2B-en 
# --------------------------------------------------------------------------------------
# :PARAMETERS:
#    <N>: The number of vectors to extract  
#         If left empty it will extract all of the vectors
#                 
#         Options for <N> are...
#         __________________________________________
#         |     :input:      |       :size:        |
#         |      100M        |       13GB          |
#         |      30M         |       3.7GB         |
#         |      10M         |       1.3GB         |
#         |      300K        |       37MB          |
#         |      100K        |       13MB          | 
#         ``````````````````````````````````````````
# ---------------------------------------------------------------------------------------
# :EXAMPLE: 
#    $./download.sh 100M
#    $./download.sh 30M
#    $./download.sh all
#
# ---------------------------------------------------------------------------------------

N="${1}" 
DEST="$(dirname ${BASH_SOURCE})/hdf5"

function extract_dataset() {
  count=$1 # Denotes N:the dataset to extract

  url="http://ingeotec.mx/~sadit/metric-datasets/LAION/SISAP23-Challenge/hamming/en-bundles/laion2B-en-hamming-n=${count}.h5"
  dest_file="$DEST/laion2B-en-hamming-n=${count}.h5"
  
  echo "[+] Checking if $dest_file file exists locally"
  if [ -e $dest_file ]
  then
    echo "[+] Data file already exists... Skipping download"
  else
    echo "[+] Data file not found locally. Downloading to $dest_file..."
    wget -P $DEST $url
  fi
}

if [[ -z "$1" ]]; then
  echo "[-] download.sh - Expected a single parameter, received none"

elif [[ $1 = 'all' ]]; then
	echo "[+] Starting extraction of all the bit-vectors.."
  for n in 100M 30M 10M 300K 100K;
  do
    extract_dataset $n
  done

else
	extract_dataset $1
fi