// #include <benchmark/benchmark.h>

// #include "/swann/dataset/load.hpp"

// static void DoSetup(const benchmark::State &state)
// {
//   Dataset in;
//   load_hdf5(in, DataSize::XS);
// }

// void BM_load_XS_dataset(benchmark::State &state)
// {
//   for (auto _ : state) {
//     // Dataset in;
//     // load_hdf5(in, DataSize::XS);
//     int i = 4;
//   }
// }
// // Register the function as a benchmark
// BENCHMARK(BM_load_XS_dataset)->Setup(DoSetup);