#pragma once

#include <assert.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

constexpr auto SAMPLES = 5000u;
constexpr uint32_t delta = 0x9E3779B9;

using Key = std::array<uint32_t, 4>;
using Data = std::vector<uint64_t>;

auto now = []() { return std::chrono::high_resolution_clock::now(); };

// shadow Key due to OpenCL requiring different type
template <class Key, class Function>
void test(const Data &expected, const Data &in, const Key &key, Function fn) {
  using namespace std::chrono;

  // warmup
  for (auto i = 0u; i < 100; ++i) {
    assert(expected == fn(in, key));
  }

  auto start = now();
#pragma omp parallel for
  for (auto i = 0u; i < SAMPLES; ++i) {
    fn(in, key);
  }
  auto end = now();
  auto sum = duration_cast<microseconds>(end - start);
  auto secs = 1.0 * sum.count() / decltype(sum)::period::den;
  auto total_size = 8.0 * SAMPLES * in.size();
  std::cout << "takes " << sum.count() / SAMPLES << "us ("
            << (total_size / std::mega::num) / secs << "MB/s)\n";
}

union FromBytes {
  uint8_t bytes[8];
  uint64_t block;
};
union ToBytes {
  uint64_t block;
  uint8_t bytes[8];
};
