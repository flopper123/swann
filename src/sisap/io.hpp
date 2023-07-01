#pragma once
#include "../dataset/load.hpp"

inline static std::string get_sisap_dataset_string(DataSize size) {
  const std::string laion2b_path = "/swann/data/laion2B-en/";
  const std::string file_path = laion2b_path + "sisap/laion2B-" + DATA_SIZES_LIST[size] + ".h5";
  return file_path;
}

inline static std::string get_sisap_dataset_queries_string() {
  const std::string laion2b_path = "/swann/data/laion2B-en/";
  const std::string file_path = laion2b_path + "sisap/sisan23-queries.h5";
  return file_path;
}

/**
 * @brief Downloads 1024-binary sketches from Laion2B-en and 
 *        returns a string holding the path to the file
 * @param size: The size of the input
 * @returns A string holding the path to the dataset
 **/
inline static std::string download_sisap_laion2B_dataset(DataSize size)
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

  return get_sisap_dataset_string(size);
}



inline static H5::DataSet fetch_sisap_points_dataset(DataSize size)
{
  return fetch_local_dataset( size, get_sisap_dataset_string(size), "/", "hamming" );
}

inline static H5::DataSet fetch_sisap_query_dataset(DataSize size)
{
  return fetch_local_dataset( size, get_sisap_dataset_queries_string(), "/", "queries" );
}

template<ui32 D> 
inline static PointsDataset<D> load_sisap(DataSize size) {
  return parse_hdf5_points<D>(fetch_sisap_points_dataset(size));
}

template<ui32 D> 
inline static PointsDataset<D> load_sisap_queries(DataSize size) {
  return parse_hdf5_points<D>(fetch_sisap_query_dataset(size));
}