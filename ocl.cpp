#include "base.h"

#define BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION

#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/types/builtin.hpp>

namespace compute = boost::compute;

namespace xtea {
namespace ocl {

auto encrypt(Data in, const compute::uint4_ &k) {
  BOOST_COMPUTE_CLOSURE(uint64_t, encrypt_block, (uint64_t data), (k, delta), {
    unsigned left = data, right = data >> 32;
    for (int i = 0, sum = 0, next_sum = delta; i < 32;
         ++i, sum = next_sum, next_sum += delta) {
      left += ((right << 4 ^ right >> 5) + right) ^ (sum + k[sum & 3]);
      right += ((left << 4 ^ left >> 5) + left) ^
               (next_sum + k[(next_sum >> 11) & 3]);
    }
    return ((unsigned long long)right) << 32 | left;
  });

  compute::vector<uint64_t> out(in.begin(), in.end());
  compute::transform(out.begin(), out.end(), out.begin(), encrypt_block);
  compute::copy(out.begin(), out.end(), in.begin());
  return in;
}

auto decrypt(Data in, const compute::uint4_ &k) {
  BOOST_COMPUTE_CLOSURE(uint64_t, decrypt_block, (uint64_t data), (k, delta), {
    unsigned left = data, right = data >> 32;
    for (int i = 0, sum = delta * 32, next_sum = sum - delta; i < 32;
         ++i, sum = next_sum, next_sum -= delta) {
      right -= ((left << 4 ^ left >> 5) + left) ^ (sum + k[(sum >> 11) & 3]);
      left -=
          ((right << 4 ^ right >> 5) + right) ^ (next_sum + k[next_sum & 3]);
    }
    return ((unsigned long long)right) << 32 | left;
  });

  compute::vector<uint64_t> out(in.begin(), in.end());
  compute::transform(out.begin(), out.end(), out.begin(), decrypt_block);
  compute::copy(out.begin(), out.end(), in.begin());
  return in;
}

namespace {

void sanity_check() {
  compute::uint4_ k1 = {0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF};
  FromBytes p1{0xEF, 0xBE, 0xAD, 0xDE, 0xEF, 0xBE, 0xAD, 0xDE};
  FromBytes c1{0xB5, 0x8C, 0xF2, 0xFA, 0xE0, 0xC0, 0x40, 0x09};

  std::cout << "Checking OpenCL XTEA...\n";

  try {
    assert(encrypt({p1.block}, k1)[0] == c1.block);
    assert(decrypt({c1.block}, k1)[0] == p1.block);
  } catch (const boost::compute::program_build_failure &e) {
    std::cout << e.build_log();
  }
}

} // namespace

void benchmark(const Key &key, const Data &plain, const Data &cipher) {
  sanity_check();

  auto k1 = compute::uint4_{key[0], key[1], key[2], key[3]};
  std::cout << "Encrypt ";
  test(cipher, plain, k1, encrypt);
  std::cout << "Decrypt ";
  test(plain, cipher, k1, decrypt);
}

} // namespace ocl
} // namespace xtea
