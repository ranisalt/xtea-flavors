#pragma once

#include "base.h"

namespace xtea {
namespace ref {

Data encrypt(Data data, const Key &k);
Data decrypt(Data data, const Key &k);

void benchmark(const Key &, const Data &, const Data &);

} // namespace ref
} // namespace xtea
