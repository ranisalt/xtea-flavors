#include "base.h"
#include "ocl.h"
#include "ref.h"
#include "stl.h"

#include <algorithm>

// constexpr auto VECTOR_SIZE = 25000u;
// 48*64bytes + 3*16bytes + 3*4bytes + 3bytes, worst size possible
constexpr auto VECTOR_SIZE = 24584u;
constexpr auto MESSAGE_LEN = (VECTOR_SIZE / (sizeof(uint64_t)));

int main() {
  auto k1 = Key{};
  std::generate(k1.begin(), k1.end(), rand);

  auto p1 = std::vector<uint64_t>(MESSAGE_LEN);
  std::generate(p1.begin(), p1.end(), rand);

  // assume reference implementation works; this gets sanity checked
  auto c1 = xtea::ref::encrypt(p1, k1);

  xtea::ref::benchmark(k1, p1, c1);
  xtea::ocl::benchmark(k1, p1, c1);
  xtea::stl::benchmark(k1, p1, c1);

  return 0;
}
