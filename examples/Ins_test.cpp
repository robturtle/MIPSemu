//
// Created by Yang Liu on 10/18/16.
//
#include <iostream>
#include <fstream>
#include "../include/Instruction.h"

using namespace std;
using namespace mips;

int main(void) {
  ifstream in("dmem.txt");

  bitset<32> bs("00100101111010010010100000111111");
  cout << bs << endl;

  RIns i((int)bs.to_ulong());
  cout << i.opcode() << ' ';
  cout << i.rs() << ' ';
  cout << i.rt() << ' ';
  cout << i.rd() << ' ';
  cout << i.shamt() << ' ';
  cout << i.funct() << endl;

  IIns ii((int)bs.to_ulong());
  cout << ii.opcode() << ' ';
  cout << ii.rs() << ' ';
  cout << ii.rt() << ' ';
  cout << ii.immediate() << endl;

  JIns ji((int)bs.to_ulong());
  cout << ji.opcode() << ' ';
  cout << ji.address() << endl;

}
