#ifndef MIPSEMU_MEMORY_H
#define MIPSEMU_MEMORY_H

#include <iostream>
#include <string>
#include <vector>
#include "../types.h"
#include "Pointer.h"
#include "bits.h"

namespace mips {

  class Memory {
    friend std::istream& operator>> (std::istream& in, Memory& memory);

  public:
    using value_type = word_t;
    using pointer_type = word_t;
    using difference_type = size_t;
    using reference_type = word_t;

    using unit_t = byte_t;
    using memory_t = std::vector<unit_t>;

    const size_t STEP = value_type().size() / unit_t().size();

  protected:
    memory_t memory;

  public:
    value_type read(pointer_type addr) const {
      auto base = std::begin(memory) + addr.to_ulong();
      return bits::join<unit_t, value_type>(base, base + STEP);
    }

    void write(pointer_type addr, value_type value) {
      auto base = std::begin(memory) + addr.to_ulong();
      std::vector<unit_t> partitions = bits::split<value_type, unit_t>(value);
      std::copy(partitions.begin(), partitions.end(), base);
    }

    value_type memoryAccess(pointer_type addr, value_type writeData, bit_t readMem, bit_t writeMem) {
      size_t code = (readMem.to_ulong() << 1) | writeMem.to_ulong();
      switch (code) {
        case 2:
          return read(addr);
        case 1:
          write(addr, writeData);
          return writeData;
        default:
          throw std::invalid_argument("readMem and writeMem flags must be exclusively set");
      }
    }

    template <typename T>
    class iterator: public Pointer<
      T,
      memory_t,
      unit_t
    > {
    public:
      iterator(memory_t &memory, word_t addr): Pointer<T, memory_t, unit_t>(memory, addr) {}
    };

    template <typename T>
    iterator<T> begin() { return iterator<T>(memory, 0); }
    iterator<value_type> begin() { return begin<value_type>(); }

    template <typename T>
    iterator<T> end() { return iterator<T>(memory, memory.size()); }
    iterator<value_type> end() { return end<value_type>(); }

  };

  class DataMem: public Memory {};

  class InsMem: public Memory {};

}
#endif //MIPSEMU_MEMORY_H
