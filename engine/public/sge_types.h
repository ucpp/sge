#ifndef _SGE_TYPES_H_
#define _SGE_TYPES_H_

#include <cstdint>

// Number of bits in a single byte
constexpr int BITS_PER_BYTE = 8;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using byte = uint8;

static_assert(sizeof(int8) * BITS_PER_BYTE == 8, "int8 size is incorrect");
static_assert(sizeof(int16) * BITS_PER_BYTE == 16, "int16 size is incorrect");
static_assert(sizeof(int32) * BITS_PER_BYTE == 32, "int32 size is incorrect");
static_assert(sizeof(int64) * BITS_PER_BYTE == 64, "int64 size is incorrect");

#endif //!_SGE_TYPES_H_