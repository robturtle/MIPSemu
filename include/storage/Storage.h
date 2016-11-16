#ifndef MIPSEMU_STORAGE_H
#define MIPSEMU_STORAGE_H

#include <array>
#include <bitset>
#include <functional> // less
#include <initializer_list>
#include <cmath>
#include "storage/Ordering.h"
#include "storage/bits.h"

namespace mips
{
namespace storage
{

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
  Storage() {}

  Storage(const std::initializer_list<unsigned long long> &list)
  {
    std::copy(
        list.begin(),
        std::min(list.end(), list.begin() + data.size()),
        data.begin());
  }

  /**
   * Read len-bit long data started from address addr,
   * len must be a multiple of unit_size.
   * @return a std::bitset<len> containing that value
   */
  template <std::size_t len>
  std::enable_if_t<
      std::less<std::size_t>()(unit_size, len) && len % unit_size == 0,
      std::bitset<len>>
  read(addr_type addr) const
  {
    unit_type unit = read<unit_size>(addr);
    std::bitset<len - unit_size> rest = read<len - unit_size>(addr + 1);
    return bits::concat(ordering, unit, rest);
  }

  template <std::size_t len>
  constexpr std::enable_if_t<
      len == unit_size,
      unit_type>
  read(addr_type addr) const
  {
    return data.at(addr);
  }

  template <std::size_t val_len, std::size_t addr_len>
  std::bitset<val_len>
  read(const std::bitset<addr_len> &addr) const
  {
    return read<val_len>(addr.to_ullong());
  }

  /**
   * Write len-bit long data in where address started at addr,
   * len must be a multiple of unit_size.
   */
  template <std::size_t len>
  std::enable_if_t<
      std::less<std::size_t>()(unit_size, len) && len % unit_size == 0>
  write(addr_type addr, const std::bitset<len> &dest)
  {
    using rest_type = std::bitset<len - unit_size>;
    std::pair<unit_type, rest_type> splited = bits::split<unit_size, len - unit_size>(ordering, dest);
    data.at(addr) = std::get<0>(splited);
    write<len - unit_size>(addr + 1, std::get<1>(splited));
  }

  template <std::size_t len>
  std::enable_if_t<
      len == unit_size>
  write(addr_type addr, const std::bitset<len> &dest)
  {
    data.at(addr) = dest;
  }

  template <std::size_t val_len, std::size_t addr_len>
  void
  write(const std::bitset<addr_len> &addr, const std::bitset<val_len> &dest)
  {
    write(addr.to_ullong(), dest);
  }

};

} /* ns storage */

} /* ns mips */

#endif //MIPSEMU_STORAGE_H
