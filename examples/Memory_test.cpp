#include <iostream>
#include "memory/Memory.h"
#include "memory/stream_loader.h"

using namespace std;
using namespace mips;

int main(void) {
  InsMem imem;
  memory::loadFromFile("imem.txt", imem);
  cout << "imem: " << endl;
  for (auto val : imem)
    cout << val << endl;

  DataMem dmem;
  memory::loadFromFile("dmem.txt", dmem);
  cout << "dmem: " << endl;
  for (auto val : dmem)
    cout << val << endl;

  dmem.write(4, Memory::value_type("10001111000011110000111100001111"));

  Memory::iterator<byte_t> bit = dmem.begin<byte_t>(), bend = dmem.end<byte_t>();

  cout << "dmem (in byte mode): " << endl;
  for (auto i = bit; i != bend; ++i)
    cout << *i << endl;

  auto send = dmem.end<short_t>();
  cout << "dmem (in short mode): " << endl;
  for (auto i = dmem.begin<short_t>(); i != send; ++i)
    cout << *i << endl;

}
