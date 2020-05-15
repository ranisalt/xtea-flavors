#include "avx.h"
#include "ocl.h"
#include "ref.h"
#include "stl.h"

// constexpr auto VECTOR_SIZE = 25000u;
// 48*64bytes + 3*16bytes + 3*4bytes + 3bytes, worst size possible for
// vectorization
constexpr auto VECTOR_SIZE = 245840u;
constexpr auto MESSAGE_LEN = (VECTOR_SIZE / (sizeof(uint64_t)));

int main() {
  std::cout << "Benchmarking with " << VECTOR_SIZE / std::kilo::num
            << "KB packets\n";

  auto k1 = Key{};
  std::generate(k1.begin(), k1.end(), rand);

  auto p1 = std::vector<uint64_t>(MESSAGE_LEN);
  std::generate(p1.begin(), p1.end(), rand);

  // assume reference implementation works; this gets sanity checked
  auto c1 = xtea::ref::encrypt(p1, k1);

  xtea::ref::benchmark(k1, p1, c1);
  std::cout << '\n';
  xtea::stl::benchmark(k1, p1, c1);
  std::cout << '\n';
  xtea::avx::benchmark(k1, p1, c1);
  std::cout << '\n';
  xtea::ocl::benchmark(k1, p1, c1);

  return 0;
}
