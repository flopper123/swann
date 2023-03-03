#include <benchmark/benchmark.h>

#include "../dataset/load.hpp"

static void DoSetup(const benchmark::State &state)
{
  Dataset out = load_hdf5(DataSize::XS);
}

static void BM_load_XS_dataset(benchmark::State &state)
{
  for (auto _ : state) {
    Dataset out = load_hdf5(DataSize::XS);
  }
}
BENCHMARK(BM_load_XS_dataset)->Setup(DoSetup);

static void BM_parse_XS_dataset(benchmark::State &state)
{
  // Setup
  DataSize size = DataSize::XS;
  H5::DataSet dataset = fetch_dataset(size);

  auto [rows, cols] = get_dataset_dimensions(dataset);

  std::vector<ui64> data_output(rows * cols);

  dataset.read(&data_output[0], H5::PredType::NATIVE_UINT64);

  // Time
  for (auto _ : state) {
    Dataset out = parse_dataset(data_output, rows, cols);
  }
}

BENCHMARK(BM_parse_XS_dataset)->Setup(DoSetup);