#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

constexpr auto SAMPLES = 2500u;
constexpr uint32_t delta = 0x9E3779B9;

using Key = std::array<uint32_t, 4>;
using Data = std::vector<uint64_t>;

inline auto now = []() { return std::chrono::high_resolution_clock::now(); };

// shadow Key due to OpenCL requiring different type
template <class Data, class Key, class Function>
void test(const Data &expected, const Data &in, const Key &key, Function fn) {
  using namespace std::chrono;
  assert(expected == fn(in, key));

  auto results = std::array<uint64_t, SAMPLES>{};
  auto start = now();
  for (auto i = 0u; i < SAMPLES; ++i) {
    auto start = now();
    fn(in, key);
    auto end = now();
    results[i] = duration_cast<microseconds>(end - start).count();
  }
  auto end = now();
  auto run_time = duration_cast<microseconds>(end - start).count();
  using ms = microseconds::period;

  std::sort(results.begin(), results.end());
  auto tot_time = std::accumulate(results.begin(), results.end(), 0u);
  auto avg = 1.0 * tot_time / results.size();
  auto median =
      (results[results.size() / 2] + results[(results.size() + 1) / 2]) / 2.0;
  auto stdev = std::sqrt(std::accumulate(results.begin(), results.end(), 0.0,
                                         [&](double acc, uint64_t val) {
                                           return acc + std::pow(val - avg, 2);
                                         }) /
                         results.size());
  auto lb = std::lower_bound(results.begin(), results.end(), avg - stdev);
  auto ub = std::upper_bound(lb, results.end(), avg + stdev);
  avg = std::accumulate(lb, ub, 0.0) / std::distance(lb, ub);

  auto secs = 1.0 * run_time / ms::den;
  auto total_size = 8.0 * SAMPLES * in.size();
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "takes " << run_time / SAMPLES << "us ("
            << (total_size / std::mega::num) / secs << "MB/s)\n";
  std::cout << "Min: " << results.front() << "us / Max: " << results.back()
            << "us / Average: " << avg << "us / Stdev: " << stdev
            << "us / Median: " << median << "us\n";
}

union FromBytes {
  uint8_t bytes[8];
  uint64_t block;
};
union ToBytes {
  uint64_t block;
  uint8_t bytes[8];
};
