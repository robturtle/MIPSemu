/*
Cache simulator
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <bitset>
using namespace std;
#include "storage/bits.h"
#include "storage/Storage.h"
#include "storage/Cache.h"
using namespace bits;
using namespace mips::storage;

int main(int argc, char const * const argv[])
{
  using mem_t = Storage</* bit length of one unit = */ 8, BigEndian>;
  using cache_t = Cache<mem_t, /* length of address = */ 32>;

  mem_t memory(2 << 20); // a 2MB RAM

  // create a 16KB cache upon this RAM
  cache_t cache{
      memory,
      /* size_block = */ 8,
      /* num_ways = */ 4,
      /* capacity = */ 16 << 10};

  using bin = std::bitset<32>;
  uint32_t addr = 0x12345678;
  cout << bin{addr} << endl << endl;
  cout << bin{cache.index(addr)} << endl
       << bin{cache.offset(addr)} << endl
       << bin{cache.tag(addr)} << endl;

  cache.read(0); // TODO implement Storage::begin() to make it compile
}
