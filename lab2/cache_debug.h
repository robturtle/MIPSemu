#ifndef CS6133_YANGLIU_CACHE_DEBUG_1128_1534
#define CS6133_YANGLIU_CACHE_DEBUG_1128_1534

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

template <typename WayInfo>
void inspect_way_tags(std::vector<WayInfo> infos, size_t ways_per_index)
{
  if (ways_per_index == 1)
  {
    std::cout << "It's a directed mapped cache. Skipped.\n";
    return;
  }
  auto it = infos.begin(), end = infos.end();
  size_t index = 0;
  while (it != end)
  {
    std::ostringstream group_tags;
    group_tags << std::hex;
    group_tags << "index " << index << ": ";
    bool full = true;
    for (size_t i = 0; i < ways_per_index; i++, ++it)
    {
      if (!(*it).valid)
      {
        full = false;
      }
      if (i != 0) group_tags << " | ";
      group_tags << std::setw(10) << (*it).tag;
    }
    if (full) std::cout << group_tags.str() << '\n';
    index++;
  }
}

#endif
