#ifndef MIPS_YANGLIU_BITS_1115
#define MIPS_YANGLIU_BITS_1115

#include "storage/Ordering.h"
#include <cstdlib>

namespace bits
{

template <std::size_t n>
struct rightmost_ones
{
  static const unsigned long long value = 1 | (rightmost_ones<n - 1>::value << 1);
};

template <>
struct rightmost_ones<0>
{
  static const unsigned long long value = 0;
};

template <std::size_t len1, std::size_t len2>
constexpr std::bitset<len1 + len2>
concat(
    mips::storage::Ordering ordering,
    std::bitset<len1> a,
    std::bitset<len2> b)
{
  switch (ordering)
  {
  case mips::storage::BigEndian:
    return (a.to_ullong() << len2) | b.to_ullong();
  case mips::storage::SmallEndian:
    return (b.to_ullong() << len1) | a.to_ullong();
  }
}

template <std::size_t len1, std::size_t len2>
constexpr std::pair<std::bitset<len1>, std::bitset<len2>>
split(
    mips::storage::Ordering ordering,
    std::bitset<len1 + len2> value)
{
  std::bitset<len1> first;
  std::bitset<len2> second;
  unsigned long long raw_bits = value.to_ullong();
  switch (ordering)
  {
  case mips::storage::BigEndian:
    return std::make_pair(
        raw_bits >> len2,
        raw_bits & rightmost_ones<len2>::value);
  case mips::storage::SmallEndian:
    return std::make_pair(
        raw_bits & rightmost_ones<len1>::value,
        raw_bits >> len1);
  }
}

} /* ns bits */

#endif