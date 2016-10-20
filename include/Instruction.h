#ifndef MIPSEMU_TYPES_H
#define MIPSEMU_TYPES_H

#include <bitset>
#include <vector>
#include <map>

namespace mips {

#define SET_FIELD_NAME(name, start, size) \
  std::bitset<size> name() { return value << (32 - start - size) >> (32 - size); }

  class Instruction {
  protected:
    uint32_t value;

    Instruction(): Instruction(0) {}

    Instruction(uint32_t value): value(value) {}

  public:
    SET_FIELD_NAME(opcode, 26, 6)
  };

  class RIns: public Instruction {
  public:
    RIns(): Instruction() {}
    RIns(uint32_t value): Instruction(value) {}

    SET_FIELD_NAME(rs, 21, 5)
    SET_FIELD_NAME(rt, 16, 5)
    SET_FIELD_NAME(rd, 11, 5)
    SET_FIELD_NAME(shamt, 6, 5)
    SET_FIELD_NAME(funct, 0, 6)
  };

  class IIns: public Instruction {
  public:
    IIns(): Instruction() {}
    IIns(uint32_t value): Instruction(value) {}

    SET_FIELD_NAME(rs, 21, 5)
    SET_FIELD_NAME(rt, 16, 5)
    SET_FIELD_NAME(immediate, 0, 16)
  };

  class JIns: public Instruction {
  public:
    JIns(): Instruction() {}
    JIns(uint32_t value): Instruction(value) {}

    SET_FIELD_NAME(address, 0, 26)
  };

#undef SET_FIELD_NAME

}
#endif //MIPSEMU_TYPES_H
