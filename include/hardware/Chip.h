#ifndef MIPS_YANGLIU_HARDWARE_1121_1754
#define MIPS_YANGLIU_HARDWARE_1121_1754

#include <bitset>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>
#include <map>
#include "sync/LogicClock.h"

namespace mips
{

using term_type = sync::clock_type;

/**********************************************************
 * Chip
 **********************************************************/
class SyncInputBase;
class UnsyncInputBase;
class SyncOutputBase;
class Chip : private sync::LogicClock
{
public:
  void notify(term_type write_term) const {//TODO
  }

//protected:
  void sync_with(SyncOutputBase *) {//TODO
  // add to sync_list
  }
};

}
#endif
