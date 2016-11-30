#ifndef CS6133_YANGLIU_CACHE_1127_2219
#define CS6133_YANGLIU_CACHE_1127_2219

#ifndef NDEBUG
#include <iostream>
#endif

#include <cmath>
#include <vector>

namespace bits
{
size_t range(size_t addr, size_t lo, size_t hi)
{
  return lo == hi ? 0 : addr << (sizeof(addr)*8 - hi) >> (sizeof(addr)*8 - hi + lo);
}
}

struct WayInfo
{
  size_t tag;
  bool valid;
  bool dirty;
};

template <typename Lower>
class Cache
{
  Lower &lower;
  
  std::vector<char> caches;
  std::vector<WayInfo> way_info;
  std::vector<size_t> evict_way;

public:
  static size_t const len_addr = 32;

  size_t const size_block;
  size_t const ways_per_index;
  size_t const capacity;

  size_t const num_all_ways;
  size_t const num_indices;

  size_t const len_index;
  size_t const len_offset;
  size_t const len_tag;

  Cache(Lower &lower, size_t size_block, size_t ways_per_index, size_t capacity)
      : lower(lower),
        size_block(size_block),
        ways_per_index(ways_per_index != 0 ? ways_per_index : capacity / size_block),
        capacity(capacity),
        num_all_ways(capacity / size_block),
        num_indices(num_all_ways / ways_per_index),
        len_index(std::log2(num_indices)),
        len_offset(std::log2(size_block)),
        len_tag(len_addr - len_index - len_offset)
  {
    setup();
  }

private:
  void setup()
  {
    caches.resize(capacity);
    way_info.resize(num_all_ways);
    evict_way.resize(num_indices);
  }

public:
  size_t index(size_t addr) const
  {
    return bits::range(addr, len_offset, len_offset + len_index);
  }

  size_t offset(size_t addr) const
  {
    return bits::range(addr, 0, len_offset);
  }

  size_t tag(size_t addr) const
  {
    return bits::range(addr, len_offset + len_index, len_addr);
  }

  size_t linear_way_addr(size_t idx, size_t way) const
  {
    return idx * ways_per_index + way;
  }

  size_t linear_caches_addr(size_t idx, size_t way, size_t offset) const
  {
    return (idx * ways_per_index + way) * size_block + offset;
  }

  bool hit(size_t addr) const
  {
    return find_hit_way(index(addr), tag(addr)) != ways_per_index;
  }

  char read(size_t addr)
  {
    size_t idx = index(addr);
    size_t way = find_hit_way(idx, tag(addr));
    if (!hit(addr))
    {
      way = evict_way.at(idx);
      cache_block(addr);
    }
    return caches.at(linear_caches_addr(idx, way, offset(addr)));
  }

  void write(size_t addr, char value)
  {
    size_t idx = index(addr);
    size_t way = find_hit_way(idx, tag(addr));
    if (hit(addr))
    {
      caches.at(linear_caches_addr(idx, way, offset(addr))) = value;
      way_info.at(linear_way_addr(idx, way)).dirty = true;
    }
    else
    {
      lower.write(addr, value);
    }
  }
 
private:
  size_t find_hit_way(size_t idx, size_t tag) const
  {
    size_t way_base = linear_way_addr(idx, 0);
    for (size_t i = 0; i < ways_per_index; i++)
    {
      auto info = way_info.at(way_base + i);
      if (info.valid && info.tag == tag)
      {
        return i;
      }
    }
    return ways_per_index;
  }

  void cache_block(size_t addr)
  {
    size_t idx = index(addr);
    size_t way = evict_way.at(idx);
    write_back(idx, way);

    size_t lower_base = addr >> len_offset << len_offset;
    size_t cache_base = linear_caches_addr(idx, way, 0);
    for (size_t i = 0; i < size_block; i++)
    {
      caches.at(cache_base + i) = lower.read(lower_base + i);
    }
    way_info.at(linear_way_addr(idx, way)) = {tag(addr), true, false};
    evict_way.at(idx) = (way + 1) % ways_per_index;
  }

  void write_back(size_t idx, size_t way)
  {
    auto info = way_info.at(linear_way_addr(idx, way));
    if (!info.valid || !info.dirty)
      return;
    size_t back_base = form_addr(info.tag, idx, 0);
    size_t cache_base = linear_caches_addr(idx, way, 0);
    for (size_t i = 0; i < size_block; i++)
    {
      lower.write(back_base + i, caches.at(cache_base + i));
    }
  }

  size_t form_addr(size_t tag, size_t idx, size_t offset) const
  {
    return (tag << len_index | idx) << len_offset | offset;
  }
};

#endif
