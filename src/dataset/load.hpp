#pragma once

#include <H5Cpp.h>

#include "../global.hpp"
#include "../index/point.hpp"

enum DataSize {XS, S, M, L, XL};
std::string DATA_SIZES_LIST[] = {"100K", "300K", "10M", "30M", "100M"};

constexpr ui32 D = 1024;

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
  assert(system(NULL)); 
  
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
 * @param size Number of vectors to download
 * @returns 
*/
Dataset load_hdf5(DataSize size) {
  const std::string filePath = download_laion2B_dataset(size),
                    groupName = "/",
                    datasetName = "hamming";
  auto dataType = H5::PredType::NATIVE_UINT64;

  assert(!filePath.empty());

  // Open the file and get the dataset
  // swann-swann-1  | HDF5 "/swann/data/laion2B-en/hdf5/laion2B-en-hamming-n=100K.h5" {
  // swann-swann-1  | GROUP "/" {
  // swann-swann-1  |    DATASET "hamming" {
  // swann-swann-1  |       DATATYPE  H5T_STD_U64LE
  // swann-swann-1  |       DATASPACE  SIMPLE { ( 100000, 16 ) / ( 100000, 16 ) }
  // swann-swann-1  |    }
  // swann-swann-1  | }
  // swann-swann-1  | }H5T_STD_U64LE
  H5::H5File file(filePath, H5F_ACC_RDONLY);

  H5::Group group = file.openGroup(groupName);

  H5::DataSet dataset = group.openDataSet(datasetName);

  H5::DataSpace dataspace = dataset.getSpace();
  std::cout << "Typeof dataset is: " << dataset.getTypeClass() << std::endl;

  // Get the number of vectors and the dimensionality
  hsize_t data_dims_out[2];
  dataspace.getSimpleExtentDims(data_dims_out, NULL);

  // TO:DO: Optimize later when bigger datasets are used
  std::vector<ui64> data_output(data_dims_out[0] * data_dims_out[1]);

  dataset.read(&data_output[0], dataType);

  std::cout << "data_dims_out[0]::" << data_dims_out[0] << "  data_dims_out[1]::" << data_dims_out[1] << std::endl;
  // Convert to Point<D> format
  // // std::vector<Point<D>> points(data_dims_out[0]);
  // //! 1024/32 => Number of ui32 elements in a Point<D>
  // //! data_dims_out[0] : Number of points in dataset
  // //! data_dims_out[1] : Number of dimension of points 1024/32
  // // for (int i = 0; i < data_dims_out[0]; ++i)
  // // {
  // //   Point<D> point;
  // //   for (int j = 0; j < data_dims_out[1]; j++) {

  // //   }
  // // }
  return Dataset();
}