#ifndef MIPSEMU_POINTER_H
#define MIPSEMU_POINTER_H

#include <iterator>
#include <cassert>
#include "../types.h"
#include "bits.h"

namespace mips {

  /**
   * A wrapper iterator for word_t (bitset<32>)
   * @param T the value refered by this Pointer
   * @param memory_t the container of data. Must be accessible via operator[](size_t)
   * @param unit_t the unit of the container. T().size() must be divisible by unit_t().size()
   */
  template <
    typename value_type,
    typename memory_t,
    typename unit_t
  >
  class Pointer: public std::iterator<
    std::random_access_iterator_tag,
    /* value_type = */ value_type,
    /* difference_type = */ long long,
    /* pointer = */ word_t,
    /* reference = */ value_type
  > {
    using difference_type = long long;
    using pointer = word_t;
    using reference = value_type;

    const size_t value_size = value_type().size();
    const size_t unit_size = unit_t().size();
    const size_t STEP = value_size / unit_size;

  protected:
    memory_t &memory;
    size_t addr;

  public:
    Pointer(memory_t &memory, pointer addr)
      :memory(memory), addr(addr.to_ulong()) {
      assert(STEP > 0);
      assert(value_size % unit_size == 0);
      this->addr = this->addr / STEP * STEP;
    }

    void jump(const jmp_addr_t &jaddr) {
      addr |= jaddr.to_ulong();
    }

    value_type operator* () {
      unsigned long value = 0;
      auto base = std::begin(memory) + addr;
      return bits::join<unit_t, value_type>(base, base + STEP);
    }

    Pointer& operator++ () {
      addr += STEP;
      return *this;
    }

    Pointer operator++ (int) {
      Pointer current(*this);
      addr += STEP;
      return current;
    }

    Pointer operator-- () {
      addr -= STEP;
      return *this;
    }

    Pointer operator-- (int) {
      Pointer current(*this);
      addr -= STEP;
      return current;
    }

    difference_type operator- (const Pointer &other) {
      return reinterpret_cast<difference_type>(addr) - reinterpret_cast<difference_type>(other.addr);
    }

    Pointer operator+ (difference_type offset) {
      return Pointer(memory, addr + offset * STEP);
    }

    Pointer operator- (difference_type offset) {
      return Pointer(memory, addr - offset * STEP);
    }

    bool operator== (const Pointer &other) {
      return memory == other.memory && addr == other.addr;
    }

    bool operator< (const Pointer &other) {
      return addr < other.addr;
    }

    bool operator!= (const Pointer &other) { return !operator==(other); }
    bool operator> (const Pointer &other) { return !operator<(other) && !operator==(other); }
    bool operator<= (const Pointer &other) { return operator<(other) || operator==(other); }
    bool operator>= (const Pointer &other) { return !operator<(other); }

  };

}

#endif //MIPSEMU_POINTER_H
