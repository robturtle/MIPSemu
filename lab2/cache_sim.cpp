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

struct MemMock
{
  using unit_type = bitset<8>;

  unit_type read(size_t addr) { return 0xff; }

  void write(size_t addr, unit_type value) {}
};

using L2_t = Cache<MemMock, /* length of address = */ 32>;
using L1_t = Cache<L2_t, 32>;

struct CacheConfig
{
  size_t size_block;
  size_t num_ways;
  size_t capacity;
};

istream &operator>>(istream &in, CacheConfig &config)
{
  string dummy;
  in >> dummy >> config.size_block >> config.num_ways >> config.capacity;
  config.capacity <<= 10;
  return in;
}

struct TraceEntry
{
  bool is_read;
  size_t addr;
};

istream &operator>>(istream& in, TraceEntry &entry)
{
  char rw;
  in >> rw >> hex >> entry.addr >> dec;
  entry.is_read = rw == 'R';
  return in;
}

#ifndef NDEBUG
template <typename T>
void pcache(T c)
{
  cout << "offset: " << c.len_offset << endl
       << "index : " << c.len_index << endl
       << "tag   : " << c.len_tag << endl
       << "block size : " << c.size_block << endl
       << "ways       : " << c.num_ways << endl
       << "capacity   : " << c.capacity/1024 << "KB" << endl;
}
#endif

void cannot_open(char const * const fname)
{
  cout << "ERROR: Cannot open file " << fname << endl;
  exit(2);
}

void usage(char const * const app)
{
    cout << "USAGE: " << app << " CACHE_CONFIG.TXT TRACES.TXT" << endl;
    exit(1);
}

int main(int argc, char const *const argv[])
{
  if (argc < 3) usage(argv[0]);

  CacheConfig c1, c2;
  ifstream config_in{argv[1]};
  if (!config_in.is_open()) { cannot_open(argv[1]); }
  config_in >> c1 >> c2;

  MemMock memory;  
  L2_t l2{memory, c2.size_block, c2.num_ways, c2.capacity};
  L1_t l1{l2, c1.size_block, c1.num_ways, c1.capacity};

#ifndef NDEBUG
  cout << "L1:\n";
  pcache(l1);
  cout << "\nL2:\n";
  pcache(l2);
  cout << "\nLet's now tracing caches...\n";
#endif

  ifstream traces{argv[2]};
  if (!traces.is_open()) { cannot_open(argv[2]); }
  string outfname = (ostringstream() << argv[2] << ".out").str();
  ofstream traceout{outfname};
  if (!traceout.is_open()) { cannot_open(outfname.c_str()); }

  TraceEntry entry;
  while (traces.good())
  {
    traces >> entry;
    if (traces.eof()) break;
#ifndef NDEBUG
    cout << (entry.is_read ? "READ " : "WRITE") << " "
         << hex << entry.addr
         << dec << "(" << entry.addr << ")" << endl;
#endif
    if (entry.is_read)
    {
      l1.read(entry.addr);
    }
    else
    {
      l1.write(entry.addr, 0xaa);
    }
    cout << l1.last_cache_result() << ' ' << l2.last_cache_result() << '\n';
    traceout << l1.last_cache_result() << ' ' << l2.last_cache_result() << '\n';
    l1.clear_cache_result();
    l2.clear_cache_result();
  }
}
