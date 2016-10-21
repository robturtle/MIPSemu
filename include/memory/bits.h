#ifndef MIPSEMU_BITS_H
#define MIPSEMU_BITS_H

#include <bitset>
#include <cassert>
#include <vector>

namespace mips {

  namespace bits {
    template<typename FROM, typename TO, typename It = typename std::vector<FROM>::iterator>
    TO join(It begin, It end) {
      const size_t partSize = FROM().size();
      const size_t parts = TO().size() / partSize;
      assert(parts > 0);
      assert(end - begin == parts);
      unsigned long long val = 0;
      for (auto it = begin; it != end; ++it) val = (val << partSize) | (*it).to_ullong();
      return val;
    }

    template<typename FROM, typename TO>
    std::vector<TO> split(const FROM &whole) {
      const size_t partSize = TO().size();
      const size_t wholeSize = FROM().size();
      const size_t parts = wholeSize / partSize;
      assert(parts > 0);
      assert(wholeSize % partSize == 0);
      std::vector<TO> partitions;
      unsigned long long val = whole.to_ullong();
      for (int i = 0; i < parts; i++, val <<= partSize) {
        partitions.push_back(val >> (wholeSize - partSize));
      }
      return partitions;
    };

  }
}
#endif //MIPSEMU_BITS_H
