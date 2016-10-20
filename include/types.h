#ifndef MIPSEMU_TYPES_H
#define MIPSEMU_TYPES_H

#include <cstdio>
#include <bitset>

namespace mips {
  const size_t BYTE_SIZE = 8;
  const size_t SHORT_SIZE = 16;
  const size_t WORD_SIZE = 32;
  const size_t OP_SIZE = 6;
  const size_t IMMEDIATE_SIZE = 16;
  const size_t REG_ADDR_SIZE = 5;
  const size_t JMP_ADDR_SIZE = 26;

  using bit_t = std::bitset<1>;
  using byte_t = std::bitset<BYTE_SIZE>;
  using short_t = std::bitset<SHORT_SIZE>;
  using word_t = std::bitset<WORD_SIZE>;
  using op_t = std::bitset<OP_SIZE>;
  using reg_addr_t = std::bitset<REG_ADDR_SIZE>;
  using immediate_t = std::bitset<IMMEDIATE_SIZE>;
  using jmp_addr_t = std::bitset<JMP_ADDR_SIZE>;
}

#endif //MIPSEMU_TYPES_H
