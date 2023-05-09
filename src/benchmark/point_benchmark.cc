#include <benchmark/benchmark.h>
#include <iostream>
#include "../index/point.hpp"
#include "../index/point_fast.hpp"

static void BM_count_point(benchmark::State &state) {
  // Setup
  std::vector<Point<1024>> points;
  ui64 N = state.range(0);
  for (ui32 i = 0; i < N; i++) {
    points.push_back(random_point<1024>(0.5));
  }
  double acc_cnt = 0;
  // Measure
  for (auto _ : state)
  {
    for(auto &p : points) {
      acc_cnt += p.count();
    }
  }

  state.counters["N"] = N;
  state.counters["acc_cnt"] = acc_cnt;
  state.counters["avg_cnt"] = acc_cnt / N;
}

static void BM_count_pointfast(benchmark::State &state) {
  std::vector<PointFast<1024>> points;
  ui64 N = state.range(0);
  for (ui32 i = 0; i < N; i++) {
    points.push_back(PointFast<1024>::Random(0.5));
  }

  double acc_cnt = 0;

  // Measure
  for (auto _ : state)
  {
    for(auto &p : points) {
      acc_cnt += p.count();
    }
  }

  state.counters["N"] = N;
  state.counters["acc_cnt"] = acc_cnt;
  state.counters["avg_cnt"] = acc_cnt / N;
}

static void BM_distance_pointfast(benchmark::State &state) {
  std::vector<std::pair<PointFast<1024>,PointFast<1024>>> points;
  ui64 N = state.range(0);
  for (ui32 i = 0; i < N; i++) {
    points.push_back({PointFast<1024>::Random(0.5), PointFast<1024>::Random(0.5)});
  }

  double acc_distance = 0;

  // Measure
  for (auto _ : state)
  {
    for(auto &[p1,p2] : points) {
      acc_distance += p1.distance(p2);
    }
  }

  state.counters["N"] = N;
  state.counters["acc_distance"] = acc_distance;
  state.counters["avg_distance"] = acc_distance / N;
}


static void BM_distance_point(benchmark::State &state) {
  std::vector<std::pair<Point<1024>,Point<1024>>> points;
  ui64 N = state.range(0);
  for (ui32 i = 0; i < N; i++) {
    points.push_back({random_point<1024>(0.5), random_point<1024>(0.5)});
  }

  double acc_distance = 0;

  // Measure
  for (auto _ : state)
  {
    for(auto &[p1,p2] : points) {
      acc_distance += p1.distance(p2);
    }
  }

  state.counters["N"] = N;
  state.counters["acc_distance"] = acc_distance;
  state.counters["avg_distance"] = acc_distance / N;
}


BENCHMARK(BM_count_pointfast)
    ->Name("Count Set Bits for PointFast")
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Arg(1000000);

BENCHMARK(BM_count_point)
    ->Name("Count Set Bits for Point")
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Arg(1000000);

BENCHMARK(BM_distance_pointfast)
    ->Name("Hamming Distance for PointFast")
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Arg(1000000);

BENCHMARK(BM_distance_point)
    ->Name("Hamming Distance for Point")
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Arg(1000000);
