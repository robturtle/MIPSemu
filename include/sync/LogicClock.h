#ifndef MIPS_YANGLIU_LOGIC_CLOCK_1119
#define MIPS_YANGLIU_LOGIC_CLOCK_1119
#include <atomic>
#include <algorithm>

namespace sync
{
  using clock_type = uint64_t;

class LogicClock
{
  mutable std::atomic<uint64_t> clock_value;

public:
  LogicClock(): clock_value(0) {}

protected:
  uint64_t get_clock() const { return clock_value; }

  uint64_t update_clock(uint64_t other_clock) const
  {
    clock_value = std::max<clock_type>(clock_value, other_clock);
    return clock_value;
  }

  uint64_t forward_clock() const
  {
    return ++clock_value;
  }
};

}

#endif
