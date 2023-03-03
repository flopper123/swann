#include <benchmark/benchmark.h>

#include "../dataset/load.hpp"



// Measure load time of datasets of variable size
static void BM_load_datasets(benchmark::State &state) {
  // Setup
  DataSize size = (state.range(0) == 100000 ? DataSize::XS : DataSize::S);

  // Measure
  for (auto _ : state) {
    Dataset out = load_hdf5(size);
  }

  // Store input size
  state.counters["input_size"] = state.range(0);
}


// Measure parsing time of datasets of variable size
static void BM_parse_datasets(benchmark::State &state) {
  // Setup
  DataSize size = (state.range(0) == 100000 ? DataSize::XS : DataSize::S);
  
  H5::DataSet dataset = fetch_dataset(size);

  auto [rows, cols] = get_dataset_dimensions(dataset);

  std::vector<ui64> data_output(rows * cols);

  dataset.read(&data_output[0], H5::PredType::NATIVE_UINT64);

  // Measure
  for (auto _ : state) {
    Dataset out = parse_dataset(data_output, rows, cols);
  }

  // Store input size
  state.counters["input_size"] = state.range(0);
}



// Setup function
static void DoSetup(const benchmark::State &state)
{
  Dataset out0 = load_hdf5(DataSize::XS);
  Dataset out1 = load_hdf5(DataSize::S);
}

// Add to benchmarks
BENCHMARK(BM_load_datasets)->Name("LoadDatasets")->Arg(100000)->Arg(300000)->Unit(benchmark::kMillisecond)->Setup(DoSetup);

BENCHMARK(BM_parse_datasets)->Name("ParseDatasets")->Arg(100000)->Arg(300000)->Unit(benchmark::kMillisecond)->Setup(DoSetup);