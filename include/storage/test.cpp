#include <iostream>
#include <bitset>
#include <vector>
#include "Storage.h"
using namespace std;
using namespace mips::storage;

int main(void) {
  Storage<8, 1024, BigEndian> imem = {1, 2, 3, 4, 5, 6, 7, 8};
  cout << imem.read<32>(0) << endl;
  cout << imem.read<16>(4) << endl;
  cout << imem.read<16>(6) << endl;
}
