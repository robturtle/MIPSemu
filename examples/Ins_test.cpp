#include <iostream>
#include <fstream>
#include "../include/Instruction.h"

using namespace std;
using namespace mips;

int main(void) {
  bitset<32> bs("00100101111010010010100000111111");
  cout << bs << endl;

  RIns i((uint32_t)bs.to_ulong());
  cout << i.opcode() << ' ';
  cout << i.rs() << ' ';
  cout << i.rt() << ' ';
  cout << i.rd() << ' ';
  cout << i.shamt() << ' ';
  cout << i.funct() << endl;

  IIns ii((uint32_t)bs.to_ulong());
  cout << ii.opcode() << ' ';
  cout << ii.rs() << ' ';
  cout << ii.rt() << ' ';
  cout << ii.immediate() << endl;

  JIns ji((uint32_t)bs.to_ulong());
  cout << ji.opcode() << ' ';
  cout << ji.address() << endl;

}
