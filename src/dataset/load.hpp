#pragma once

#include "../global.hpp"
#include "../index/point.hpp"

#include "./dataset.hpp"

#include <H5Cpp.h>
#include <sstream>


inline static std::string get_dataset_string(DataSize size) {
  const std::string laion2b_path = "/swann/data/laion2B-en/";
  const std::string file_name = laion2b_path + "hdf5/laion2B-" + DATA_SIZES_LIST[size] + ".h5";
  return file_name;
}

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

  // Check for debug printing
  auto debugging = std::getenv("DEBUG");
  bool should_debug = debugging != nullptr && std::string(debugging) == "true";

  // Make the path of the python script relative to the directory
  const std::string laion2b_path = "/swann/data/laion2B-en/";
  const std::string command = laion2b_path + "download.sh " + DATA_SIZES_LIST[size] + (should_debug ? "" : " > /dev/null");


  // Execute download script
  std::system(command.c_str());

  return get_dataset_string(size);
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

inline static H5::DataSet fetch_local_dataset(
  DataSize size, 
  std::string filePath, 
  std::string groupName, 
  std::string datasetName
) {
  assert(!filePath.empty());
  
  H5::H5File file(filePath, H5F_ACC_RDONLY);
  H5::Group group = file.openGroup(groupName);
  return group.openDataSet(datasetName);
}

inline static H5::DataSet fetch_points_dataset(DataSize size)
{
  return fetch_local_dataset( size, get_dataset_string(size), "/", "hamming" );
}

inline static H5::DataSet fetch_query_dataset(DataSize size)
{
  return fetch_local_dataset( size, get_dataset_string(size), "/queries", "points" );
}

inline static H5::DataSet fetch_answers_dataset(DataSize size) 
{
  return fetch_local_dataset( size, get_dataset_string(size), "/queries", "answers" );
}

template<ui32 D>
inline static PointsDataset<D> parse_points_dataset(std::vector<ui64> &in, const int rows, const int cols)
{
  PointsDataset<D> src;

  assert(D / cols == 64);
  for (int r = 0; r < rows; ++r) {
    std::stringstream ss;

    for (int c = 0; c < cols; ++c) {
      ss << std::bitset<64>(in[r * cols + c]);
    }
    
    src.emplace_back(ss.str()); // construct point from bitstring
  }

  assert(src.size() == rows);

  return src;
}

template<ui32 D>
inline static PointsDataset<D> parse_hdf5_points(H5::DataSet dataset) {

  auto [rows, cols] = get_dataset_dimensions(dataset);

  std::vector<ui64> data_output(rows * cols);

  dataset.read(&data_output[0], H5::PredType::NATIVE_UINT64);
  dataset.close();

  return parse_points_dataset<D>(data_output, rows, cols);
}

inline static std::vector<std::vector<ui32>> parse_hdf5_answers(H5::DataSet dataset)
{
  auto [rows, cols] = get_dataset_dimensions(dataset);
  
  ui32* ans = new ui32[rows * cols];
  
  dataset.read(&ans[0], H5::PredType::NATIVE_UINT32);
  dataset.close();

  std::vector<std::vector<ui32>> ans_vec(rows, std::vector<ui32>(cols, 0));

  ui32 maximum = 0;
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      maximum = std::max(ans[r * cols + c], maximum);
      ans_vec[r][c] = ans[r * cols + c];
    }
  }

  assert(ans_vec.size() == rows);
  
  return ans_vec;
}

/**
 * @brief Load a hdf5 dataset
 * @param src The dataset to load into.
 *            The points in the dataset will be pushed back
 * @param size Number of vectors to download
 */
template<ui32 D>
inline static PointsDataset<D> load_hdf5(DataSize size) {
  return parse_hdf5_points<D>(fetch_points_dataset(size));
}

// Load queries:
template<ui32 D>
inline static QueryDataset<D> load_queries(DataSize size) {

  PointsDataset<D> query = parse_hdf5_points<D>(fetch_query_dataset(size)); // List of points
  
  std::vector<std::vector<ui32>> answers = parse_hdf5_answers(fetch_answers_dataset(size));

  assert(query.size() == answers.size());
  const ui32 N = query.size();
  
  QueryDataset<D> dataset(N);
  for (int n = 0; n < N; n++) {
    dataset[n] = { query[n], answers[n] };
  }
  return dataset;
}

// Load benchmark datasets:
template<ui32 D>
inline static BenchmarkDataset<D> load_benchmark_dataset(DataSize size) {
  auto points = load_hdf5<D>(size);
  auto queries = load_queries<D>(size);

  return {points, queries};
}