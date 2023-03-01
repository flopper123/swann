#pragma once

#include "../global.hpp"
#include "../index/point.hpp"

#include <H5Cpp.h>
#include <sstream>

enum DataSize {XS, S, M, L, XL};
std::string DATA_SIZES_LIST[] = {"100K", "300K", "10M", "30M", "100M"};

constexpr ui32 D = 1024;

//! TO:DO Load and store queries
class Dataset : public std::vector<Point<D>> {
  using std::vector<Point<D>>::vector;
};

/**
 * @brief Downloads 1024-binary sketches from Laion2B-en and 
 *        returns a string holding the path to the file
 * @param size: The size of the input
 * @returns A string holding the path to the dataset
 **/
std::string download_laion2B_dataset(DataSize size) {
  // Assert system call is available
  assert(std::system(NULL)); 
  
  // Make the path of the python script relative to the directory
  const std::string laion2b_path = "/swann/data/laion2B-en/";
  const std::string command = laion2b_path + "download.sh " + DATA_SIZES_LIST[size];

  // Execute download script
  std::system(command.c_str());

  const std::string file_name = laion2b_path + "hdf5/laion2B-en-hamming-n=" + DATA_SIZES_LIST[size] + ".h5";
  return file_name;
}

/**
 * @brief Load a hdf5 dataset
 * @param src The dataset to load into. 
 *            The points in the dataset will be pushed back
 * @param size Number of vectors to download
*/
void load_hdf5(Dataset& src, DataSize size) {
  const int start_size = src.size();
  const std::string filePath = download_laion2B_dataset(size),
                    groupName = "/",
                    datasetName = "hamming";
  assert(!filePath.empty());

  H5::H5File file(filePath, H5F_ACC_RDONLY);
  H5::Group group = file.openGroup(groupName);
  H5::DataSet dataset = group.openDataSet(datasetName);
  H5::DataSpace dataspace = dataset.getSpace();

  // Get the number of vectors and the dimensionality
  hsize_t data_dims_out[2];
  dataspace.getSimpleExtentDims(data_dims_out, NULL);

  //! TO:DO: Optimize later when bigger datasets are used, ideally we should operate on a Dataset pointer
  std::vector<ui64> data_output(data_dims_out[0] * data_dims_out[1]);
  dataset.read(&data_output[0], H5::PredType::NATIVE_UINT64);

  const int R = data_dims_out[0],  // Number of points in dataset, 
            C = data_dims_out[1];  // Number of ui64 to make up a single Point in dataset
  assert(D/C == 64);
 
  for (int r = 0; r < R; ++r) {
    std::stringstream ss;

    //! TO:DO write check of order : I fear bitsets might be constructed 
    //!       in inverse order due to the way they are printed
    for (int c = 0; c < C; ++c)
      ss << std::bitset<64>(data_output[r*C+c]); 

    src.emplace_back(ss.str()); // construct point from bitstring
  }

  assert(src.size()-start_size == R);
}
