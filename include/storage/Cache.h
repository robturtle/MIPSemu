#ifndef MIPS_YANGLIU_CACHE_1124_2104
#define MIPS_YANGLIU_CACHE_1124_2104
#include <cstddef>
#include <cmath>
#include <bitset>
#include <vector>
#include "storage/bits.h"
#include "storage/Storage.h"

namespace mips
{
namespace storage
{

namespace detail
{

struct Dimensions
{
  std::vector<size_t> sizes;

  Dimensions() : sizes(std::vector<size_t>()) {}
  Dimensions(std::vector<size_t> const &sizes) : sizes(sizes) {}

  size_t operator() (std::vector<size_t> const &idxs) const
  {
    if (sizes.size() != idxs.size() - 1)
    {
      throw std::string("dimension number mismatch");
    }
    size_t len = sizes.size();
    size_t addr = idxs.at(0);
    for (size_t i = 0; i < len; i++)
      addr = addr * sizes.at(i) + idxs.at(i + 1);
    return addr;
  }

};

template <typename Container>
class MultiDimensionArray
{
private:
  Container container;
  Dimensions dimensions;

public:
  void reshape(std::vector<size_t> const &d) { dimensions = d; }

  decltype(container.at(0)) at(std::vector<size_t> const &idxs)
  {
    return container.at(dimensions(idxs));
  }

  void resize(size_t new_size)
  {
    container.resize(new_size);
  }
};

struct WayInfo
{
  size_t tag;
  bool valid;
};

} /* ns detail */

enum CacheResult
{
  NoAccess = 0,
  ReadHit,
  ReadMiss,
  WriteHit,
  WriteMiss,
};

template <
    typename LowerStorage,
    size_t len_addr_,
    typename CacheImpl = Storage<8, BigEndian>>
class Cache
{
private:
  static size_t const len_addr = len_addr_;

  LowerStorage &lower;
  CacheImpl caches;

  detail::MultiDimensionArray<std::vector<detail::WayInfo>> way_info;
  std::vector<size_t> evict_way;

  detail::Dimensions const idx_way_offset;

  mutable CacheResult cache_result;

public:
  size_t const size_block;
  size_t const num_ways;
  size_t const capacity;

  size_t const len_index = std::log2(capacity / size_block / num_ways);
  size_t const len_offset = std::log2(size_block);
  size_t const len_tag = len_addr - len_index - len_offset;

  using unit_type = typename CacheImpl::unit_type;

  Cache(
      LowerStorage &lower,
      size_t size_block,
      size_t num_ways,
      size_t capacity)
      : lower(lower),
        caches(CacheImpl(capacity)),
        size_block(size_block),
        num_ways(num_ways),
        capacity(capacity),
        idx_way_offset(detail::Dimensions({num_ways, size_block}))
  {
    setup();
  }

  Cache(Cache const &other)
      : Cache(other.lower, other.size_block, other.num_ways, other.capacity) {}

private:
  void setup()
  {
    way_info.resize(0);
    way_info.resize(capacity / size_block);
    way_info.reshape({num_ways});
    evict_way.resize(0);
    evict_way.resize(capacity / size_block / num_ways);
  }

public:
  constexpr size_t index(size_t addr) const
  {
    return bits::range(addr, len_offset, len_offset + len_index);
  }

  constexpr size_t offset(size_t addr) const
  {
    return bits::range(addr, 0, len_offset);
  }

  constexpr size_t tag(size_t addr) const
  {
    return bits::range(addr, len_offset + len_index, len_addr);
  }

  constexpr size_t linear_addr(size_t idx, size_t way, size_t offset) const
  {
    return idx_way_offset({idx, way, offset});
  }

  unit_type read(size_t addr)
  {
    size_t idx = index(addr);
    size_t way = find_hit_way(idx, tag(addr));
    if (way < num_ways)
    {
      cache_result = ReadHit;
    }
    else
    {
      cache_result = ReadMiss;
      way = evict_way.at(idx);
      cache_block(addr);
    }
    return caches.read(linear_addr(idx, way, offset(addr)));
  }

  void write(size_t addr, unit_type const &value)
  {
    size_t idx = index(addr);
    size_t way = find_hit_way(idx, tag(addr));
    if (way < num_ways)
    {
      cache_result = WriteHit;
      caches.write(linear_addr(idx, way, offset(addr)), value);
    }    
    else
    {
      cache_result = WriteMiss;
      lower.write(addr, value);
    }
  }

  CacheResult last_cache_result() const
  {
    return cache_result;
  }

  void clear_cache_result() const
  {
    cache_result = NoAccess;
  }

protected:
  int find_hit_way(size_t idx, size_t tag)
  {
    for (size_t i = 0; i < num_ways; i++)
    {
      auto info = way_info.at({idx, i});
      if (info.valid && info.tag == tag)
      {
        return i;
      }
    }
    return num_ways;
  }

  void cache_block(size_t addr)
  {
    size_t idx = index(addr);
    size_t way = evict_way.at(idx);
    write_back(idx, way);

    size_t base = addr >> len_offset << len_offset;
    size_t cache_base = linear_addr(idx, way, 0);
    for (size_t i = 0; i < size_block; i++)
    {
      caches.write(cache_base + i, lower.read(base + i));
    }

    way_info.at({idx, way}) = {tag(addr), true};
    evict_way.at(idx) = (way + 1) % num_ways;
  }

  void write_back(size_t idx, size_t way)
  {
    auto info = way_info.at({idx, way});
    if (!info.valid) return;
    size_t back_addr = form_addr(info.tag, idx, 0);
    size_t cache_base = linear_addr(idx, way, 0);
    for (size_t i = 0; i < size_block; i++)
    {
      lower.write(back_addr + i, caches.read(cache_base + i));
    }
  }

  constexpr size_t form_addr(size_t tag, size_t idx, size_t offset) const
  {
    size_t addr = (tag << len_index) | idx;
    return (addr << len_offset) | offset;
  }
};

} /* ns storage */
} /* ns mips */
#endif
