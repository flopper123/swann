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

# http://ingeotec.mx/~sadit/metric-datasets/LAION/SISAP23-Challenge/hamming/en-bundles

N="${1}"
DEST="$(dirname ${BASH_SOURCE})/sisap23"

function extract_queries() {

  url="https://sisap-23-challenge.s3.amazonaws.com/SISAP23-Challenge/public-queries-en-hammingv2.h5"

  dest_filename="sisap23-queries.h5"
  dest_file="$DEST/$dest_filename"
  
  echo "[+] Checking if $dest_file file exists locally"
  if [ -e $dest_file ]
  then
    echo "[+] Data file already exists... Skipping download"
  else
    echo "[+] Data file not found locally. Downloading to $dest_file..."
    wget -c $url -O $dest_file
  fi
}

function extract_dataset() {
  count=$1 # Denotes N:the dataset to extract
  
  url="https://sisap-23-challenge.s3.amazonaws.com/SISAP23-Challenge/laion2B-en-hammingv2-n=${count}.h5"

  dest_filename="laion2B-${count}.h5"
  dest_file="$DEST/$dest_filename"
  
  echo "[+] Checking if $dest_file file exists locally"
  if [ -e $dest_file ]
  then
    echo "[+] Data file already exists... Skipping download"
  else
    echo "[+] Data file not found locally. Downloading to $dest_file..."
    wget -c $url -O $dest_file
  fi
}

if [[ -z "$1" ]]; then
  echo "[-] download.sh - Expected a single parameter, received none"

elif [[ $1 = 'all' ]]; then
	echo "[+] Starting extraction of all the bit-vectors.."
  for n in 100M 30M 10M 300K 100K;
  do
    extract_dataset $n
    extract_queries $n
  done

else
	extract_dataset $1
	extract_queries $1
fi