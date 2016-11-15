#ifndef MIPSEMU_STORAGE_H
#define MIPSEMU_STORAGE_H

#include <array>
#include <bitset>
#include <functional> // less
#include <initializer_list>
#include <cmath>

namespace mips
{
namespace storage
{

enum Ordering
{
  BigEndian,
  SmallEndian
};

template <
    std::size_t unit_size,
    std::size_t capacity,
    Ordering ordering>
class Storage
{
public:
  using unit_type = std::bitset<unit_size>;
  using addr_type = std::size_t;

private:
  std::array<unit_type, capacity> data;

public:
  Storage(const std::initializer_list<unsigned long long> &list)
  {
    std::copy(
        list.begin(),
        std::min(list.end(), list.begin() + data.size()),
        data.begin());
  }

  template <std::size_t len>
  std::enable_if_t<
      std::less<std::size_t>()(unit_size, len) && len % unit_size == 0,
      std::bitset<len>>
  read(addr_type addr) const
  {
    unit_type unit = read<unit_size>(addr);
    std::bitset<len - unit_size> rest = read<len - unit_size>(addr + 1);
    return concat(unit, rest);
  }

  template <std::size_t len>
  constexpr std::enable_if_t<
      len == unit_size,
      unit_type>
  read(addr_type addr) const
  {
    return data.at(addr);
  }

private:
  template <std::size_t len1, std::size_t len2>
  constexpr std::bitset<len1 + len2> concat(std::bitset<len1> a, std::bitset<len2> b) const
  {
    switch (ordering)
    {
    case BigEndian:
      return (a.to_ullong() << len2) | b.to_ullong();
    case SmallEndian:
      return (b.to_ullong() << len1) | a.to_ullong();
    }
  }
};

} /* ns storage */

} /* ns mips */

#endif //MIPSEMU_STORAGE_H
