#pragma once
#include "../dataset/load.hpp"

/**
 * @brief Save results to HDF5 file
 **/
inline static void save_results_to_hdf5(
    std::string file_path,
    std::vector<std::vector<std::pair<ui32, ui32>>> results,
    DataSize size,
    float build_time, float query_time,
    std::string hyperparams_description 
) {
  // Constants
  const std::string FOLDER_NAME = "/swann/result/";
  const std::string ALGO_NAME = "swann";
  const std::string DATA_NAME = "hammingv2";
  constexpr ui32 outer_dims = 10000; // 10k queries
  constexpr ui32 inner_dims = 10; // 10 closest neighbours

  // Create file
  H5::H5File file(FOLDER_NAME + file_path, H5F_ACC_TRUNC);

  // Create dataspace
  hsize_t dims[2] = {outer_dims, inner_dims};
  H5::DataSpace dataspace(2, dims);

  // Create dataset
  H5::DataSet *knns_dataset  = new H5::DataSet(file.createDataSet("knns",  H5::PredType::NATIVE_UINT32, dataspace));
  H5::DataSet *dists_dataset = new H5::DataSet(file.createDataSet("dists", H5::PredType::NATIVE_UINT32, dataspace));

  // Write results data to dists and knns
  ui32 dists[outer_dims][inner_dims], knns[outer_dims][inner_dims];
  for (ui32 i = 0; i < outer_dims; i++) {
    for (ui32 j = 0; j < inner_dims; j++) {
      auto &[idx, dist] = results[i][j];
      dists[i][j] = dist;
      knns[i][j]  = idx;
    }
  }
  
  // Save results
  knns_dataset->write(&knns[0], H5::PredType::NATIVE_UINT32);
  dists_dataset->write(&dists[0], H5::PredType::NATIVE_UINT32);

  // Create attributes
  H5::StrType strdatatype(H5::PredType::C_S1, 1000);
  H5::Attribute *algo_name_attr = new H5::Attribute(file.createAttribute("algo", strdatatype, H5S_SCALAR));
  H5::Attribute *data_name_attr = new H5::Attribute(file.createAttribute("data", strdatatype, H5S_SCALAR));
  H5::Attribute *data_size_attr = new H5::Attribute(file.createAttribute("size", strdatatype, H5S_SCALAR));
  H5::Attribute *build_time_attr = new H5::Attribute(file.createAttribute("buildtime", H5::PredType::NATIVE_FLOAT, H5S_SCALAR));
  H5::Attribute *query_time_attr = new H5::Attribute(file.createAttribute("querytime", H5::PredType::NATIVE_FLOAT, H5S_SCALAR));
  H5::Attribute *hyperparams_attr = new H5::Attribute(file.createAttribute("params", strdatatype, H5S_SCALAR));

  // Write attributes
  algo_name_attr->write(strdatatype, ALGO_NAME);
  data_name_attr->write(strdatatype, DATA_NAME);
  data_size_attr->write(strdatatype, DATA_SIZES_LIST[size]);
  build_time_attr->write(H5::PredType::NATIVE_FLOAT, &build_time);
  query_time_attr->write(H5::PredType::NATIVE_FLOAT, &query_time);
  hyperparams_attr->write(strdatatype, hyperparams_description);
}

inline static std::string get_sisap_dataset_string() {
  const std::string laion2b_path = "/swann/data/laion2B-en/";
  const std::string file_path = laion2b_path + "sisap23/laion2B-points.h5";
  return file_path;
}

inline static std::string get_sisap_dataset_queries_string() {
  const std::string laion2b_path = "/swann/data/laion2B-en/";
  const std::string file_path = laion2b_path + "sisap23/sisap23-queries.h5";
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
  const std::string laion2b_path = "/swann/data/";
  const std::string command = laion2b_path + "sisapchallenge.sh " + DATA_SIZES_LIST[size] + (should_debug ? "" : " > /dev/null");


  // Execute download script
  std::system(command.c_str());

  return get_sisap_dataset_string();
}

inline static H5::DataSet fetch_sisap_points_dataset()
{
  return fetch_local_dataset( DataSize::XL, get_sisap_dataset_string(), "/", "hamming" );
}

inline static H5::DataSet fetch_sisap_query_dataset()
{
  return fetch_local_dataset( DataSize::XL, get_sisap_dataset_queries_string(), "/", "hamming" );
}

template<ui32 D> 
inline static PointsDataset<D> load_sisap() {
  return parse_hdf5_points<D>(fetch_sisap_points_dataset());
}

template<ui32 D> 
inline static PointsDataset<D> load_sisap_queries() {
  return parse_hdf5_points<D>(fetch_sisap_query_dataset());
}