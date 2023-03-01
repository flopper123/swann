#include <benchmark/benchmark.h>

void BM_StringCreation(benchmark::State &state)
{
  for (auto _ : state)
    std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

void BM_StringCopy(benchmark::State &state)
{
  std::string x = "hello";
  for (auto _ : state)
    std::string copy(x);
}
BENCHMARK(BM_StringCopy);