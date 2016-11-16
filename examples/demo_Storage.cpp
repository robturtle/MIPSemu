#include <iostream>
#include <bitset>
#include <vector>
#include "storage/Storage.h"
using namespace std;
using namespace mips::storage;

int main(void) {
  cout << "Initial memory data: 1, 2, 3, 4, 5, 6, 7, 8" << endl;
  Storage<8, 1024, BigEndian> imem = {1, 2, 3, 4, 5, 6, 7, 8};
  cout << "read in words 0" << endl;
  cout << imem.read<32>(0) << endl;
  cout << "read in half words 4 and 6" << endl;
  cout << imem.read<16>(4) << endl;
  cout << imem.read<16>(6) << endl;
  cout << "read in words 4" << endl;
  cout << imem.read<32>(bitset<11>(4)) << endl;
  cout << "read in word 8 (uninitialized value)" << endl;
  cout << imem.read<32>(8) << endl;

  cout << endl << "Write 0x030C30C0 at 8 ..." << endl;
  bitset<32> value = 0x030C30C0;
  cout << "read in bytes 8, 9, 10, 11" << endl;
  imem.write(8, value);
  for (int i = 8; i < 12; i++)
  {
    //cout << imem.read<8>(bitset<32>(i)) << endl;
    cout << imem.read<8>(i) << endl;
  }
}
