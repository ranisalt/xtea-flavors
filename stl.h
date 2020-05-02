#pragma once

#include "base.h"

namespace xtea {
namespace stl {

Data encrypt(Data data, const Key &k);
Data decrypt(Data data, const Key &k);

void benchmark(const Key &, const Data &, const Data &);

} // namespace stl
} // namespace xtea
