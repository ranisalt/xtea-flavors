#include "stl.h"

#include <execution>

namespace execution = std::execution;

namespace xtea {
namespace stl {

Data encrypt(Data in, const Key &k) {
  auto kernel = [&](uint64_t data) {
    uint32_t left = data, right = data >> 32;

    uint32_t sum = 0u;
    for (auto i = 0u; i < 32u; ++i) {
      left += ((right << 4 ^ right >> 5) + right) ^ (sum + k[sum & 3]);
      sum += delta;
      right += ((left << 4 ^ left >> 5) + left) ^ (sum + k[(sum >> 11) & 3]);
    }

    return static_cast<uint64_t>(right) << 32 | left;
  };

  std::transform(execution::par, in.begin(), in.end(), in.begin(), kernel);
  return in;
}

Data decrypt(Data in, const Key &k) {
  auto kernel = [&](uint64_t data) {
    uint32_t left = data, right = data >> 32;

    uint32_t sum = delta * 32;
    for (auto i = 0u; i < 32u; ++i) {
      right -= ((left << 4 ^ left >> 5) + left) ^ (sum + k[(sum >> 11) & 3]);
      sum -= delta;
      left -= ((right << 4 ^ right >> 5) + right) ^ (sum + k[sum & 3]);
    }

    return static_cast<uint64_t>(right) << 32 | left;
  };

  std::transform(execution::par, in.begin(), in.end(), in.begin(), kernel);
  return in;
}

namespace {

void sanity_check() {
  std::array<uint32_t, 4> k1 = {0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF};
  FromBytes p1{0xEF, 0xBE, 0xAD, 0xDE, 0xEF, 0xBE, 0xAD, 0xDE};
  FromBytes c1{0xB5, 0x8C, 0xF2, 0xFA, 0xE0, 0xC0, 0x40, 0x09};

  std::cout << "Checking std::execution::par XTEA...\n";
  assert(encrypt({p1.block}, k1)[0] == c1.block);
  assert(decrypt({c1.block}, k1)[0] == p1.block);
}

} // namespace

void benchmark(const Key &key, const Data &plain, const Data &cipher) {
  sanity_check();

  std::cout << "Encrypt ";
  test(cipher, plain, key, encrypt);
  std::cout << "Decrypt ";
  test(plain, cipher, key, decrypt);
}

} // namespace stl
} // namespace xtea
