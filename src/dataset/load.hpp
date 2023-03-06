#pragma once

#include "../global.hpp"
#include "../index/point.hpp"

#include <H5Cpp.h>
#include <sstream>

enum DataSize {XS, S, M, L, XL};
static inline std::string DATA_SIZES_LIST[] = {"100K", "300K", "10M", "30M", "100M"};

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
inline static std::string download_laion2B_dataset(DataSize size)
{
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

inline static Dataset parse_dataset(std::vector<ui64> &in, const int rows, const int cols)
{
  Dataset src;

  assert(D / cols == 64);
  for (int r = 0; r < rows; ++r) {
    std::stringstream ss;

    for (int c = 0; c < cols; ++c)
      ss << std::bitset<64>(in[r * cols + c]);

    src.emplace_back(ss.str()); // construct point from bitstring
  }

  assert(src.size() == rows);

  return src;
}

/** @returns A pair containing the number of rows and cols of the dataset */
inline static std::pair<int, int> get_dataset_dimensions(H5::DataSet &dataset)
{
  H5::DataSpace dataspace = dataset.getSpace();

  // Get the number of vectors and the dimensionality
  hsize_t data_dims_out[2];
  dataspace.getSimpleExtentDims(data_dims_out, NULL);

  return {(int)data_dims_out[0], (int)data_dims_out[1]};
}

inline static H5::DataSet fetch_dataset(DataSize size)
{
  const std::string filePath = download_laion2B_dataset(size),
                    groupName = "/",
                    datasetName = "hamming";
  assert(!filePath.empty());
  
  H5::H5File file(filePath, H5F_ACC_RDONLY);
  H5::Group group = file.openGroup(groupName);
  return group.openDataSet(datasetName);
}

/**
 * @brief Load a hdf5 dataset
 * @param src The dataset to load into.
 *            The points in the dataset will be pushed back
 * @param size Number of vectors to download
 */
inline static Dataset load_hdf5(DataSize size) {

  H5::DataSet dataset = fetch_dataset(size);

  auto [rows, cols] = get_dataset_dimensions(dataset);

  std::vector<ui64> data_output(rows * cols);

  dataset.read(&data_output[0], H5::PredType::NATIVE_UINT64);

  return parse_dataset(data_output, rows, cols);
}
