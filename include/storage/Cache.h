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
    size_t len_addr,
    typename CacheImpl = Storage<8, BigEndian>>
class Cache
{
  LowerStorage &lower;

  CacheImpl caches;
  std::vector<size_t> tags;
  std::vector<size_t> evict_way;
  std::vector<bool> valid;

  mutable CacheResult cache_result;

  size_t const size_block;
  size_t const num_ways;
  size_t const capacity;

  detail::Dimensions const idx_way, idx_way_offset;

  size_t const len_index = std::log2(capacity / size_block / num_ways);
  size_t const len_offset = std::log2(size_block);
  size_t const len_tag = len_addr - len_index - len_offset;

public:
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
        idx_way(detail::Dimensions({num_ways})),
        idx_way_offset(detail::Dimensions({num_ways, size_block}))
  {
    tags.resize(capacity / size_block);
    valid.resize(capacity / size_block);
    evict_way.resize(capacity / size_block / num_ways);
  }

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

  constexpr size_t _(size_t idx, size_t way) const
  {
    return idx_way({idx, way});
  }

  constexpr size_t __(size_t idx, size_t way, size_t offset) const
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
      cache_block(addr, way);
    }
    return caches.read(__(idx, way, offset(addr)));
  }

  void write(size_t addr, unit_type const &value)
  {
    size_t idx = index(addr);
    size_t way = find_hit_way(idx, tag(addr));
    if (way < num_ways)
    {
      cache_result = WriteHit;
      caches.write(__(idx, way, offset(addr)), value);
    }    
    else
    {
      cache_result = WriteMiss;
      lower.write(addr, value);
    }
  }

  void clear_cache_result() const
  {
    cache_result = NoAccess;
  }

protected:
  int find_hit_way(size_t idx, size_t tag) const
  {
    for (size_t i = 0; i < num_ways; i++)
    {
      if (valid.at(_(idx, i)) && tags.at(_(idx, i)) == tag)
      {
        return i;
      }
    }
    return num_ways;
  }

  void cache_block(size_t addr, size_t way)
  {
    size_t idx = index(addr);
    if (valid.at(_(idx, way)))
    {
      size_t cur_addr = form_addr(tags.at(_(idx, way)), idx, 0);
      for (size_t i = 0; i < size_block; i++)
      {
        lower.write(cur_addr + i, caches.read(__(idx, way, i)));
      }
    }

    for (size_t i = 0; i < size_block; i++)
    {
      caches.write(__(idx, way, i), lower.read(addr + i));
    }

    tags.at(_(idx, way)) = tag(addr);
    valid.at(_(idx, way)) = true;
    evict_way.at(idx) = (way + 1) % num_ways;
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
