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
    std::size_t unit_size_,
    std::size_t capacity_,
    Ordering ordering_>
class Storage
{
// TODO: iterator
public:
  using unit_type = std::bitset<unit_size_>;
  using addr_type = std::size_t;

  static const std::size_t unit_size = unit_size_;
  static const std::size_t capacity = capacity_;
  static const Ordering ordering = ordering_;

private:
  std::array<unit_type, capacity_> data;

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
   * len must be a multiple of unit_size_.
   * @return a std::bitset<len> containing that value
   */
  template <std::size_t len>
  constexpr std::enable_if_t<
      std::less<std::size_t>()(unit_size_, len) && len % unit_size_ == 0,
      std::bitset<len>>
  read(addr_type addr) const
  {
    unit_type unit = read<unit_size_>(addr);
    std::bitset<len - unit_size_> rest = read<len - unit_size_>(addr + 1);
    return bits::concat(ordering_, unit, rest);
  }

  template <std::size_t len>
  constexpr std::enable_if_t<
      len == unit_size_,
      unit_type>
  read(addr_type addr) const
  {
    return data.at(addr);
  }

  template <std::size_t val_len, std::size_t addr_len>
  constexpr std::bitset<val_len>
  read(const std::bitset<addr_len> &addr) const
  {
    return read<val_len>(addr.to_ullong());
  }

  /**
   * Write len-bit long data in where address started at addr,
   * len must be a multiple of unit_size_.
   */
  template <std::size_t len>
  constexpr std::enable_if_t<
      std::less<std::size_t>()(unit_size_, len) && len % unit_size_ == 0>
  write(addr_type addr, const std::bitset<len> &dest)
  {
    using rest_type = std::bitset<len - unit_size_>;
    std::pair<unit_type, rest_type> splited = bits::split<unit_size_, len - unit_size_>(ordering_, dest);
    data.at(addr) = std::get<0>(splited);
    write<len - unit_size_>(addr + 1, std::get<1>(splited));
  }

  template <std::size_t len>
  constexpr std::enable_if_t<
      len == unit_size_>
  write(addr_type addr, const std::bitset<len> &dest)
  {
    data.at(addr) = dest;
  }

  template <std::size_t val_len, std::size_t addr_len>
  constexpr void
  write(const std::bitset<addr_len> &addr, const std::bitset<val_len> &dest)
  {
    write(addr.to_ullong(), dest);
  }

};

} /* ns storage */

} /* ns mips */

#endif //MIPSEMU_STORAGE_H
