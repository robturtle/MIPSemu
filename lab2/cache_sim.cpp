/*
Cache simulator
 */
#ifdef BIN_INSPECT
#include "util/BinInspect.h"
#endif

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
  cout << "offset: " << c.len_offset << '\n'
       << "index : " << c.len_index << '\n'
       << "tag   : " << c.len_tag << '\n'
       << "block size : " << c.size_block << '\n'
       << "ways       : " << c.num_ways << '\n'
       << "capacity   : " << c.capacity/1024 << "KB" << '\n';
}

template <typename T>
void paddr(T cache, size_t addr)
{
  cout << "tag = 0x" << cache.tag(addr) << " ";
  cout << "index = 0x" << cache.index(addr) << " ";
  cout << "offset = 0x" << cache.offset(addr) << "\n";
#ifdef BIN_INSPECT
  BinInspect t{cache.tag(addr), cache.len_tag};
  BinInspect i{cache.index(addr), cache.len_index};
  BinInspect o{cache.offset(addr), cache.len_offset};
  cout << BinInspect(addr, 32) << '\n';
  cout << (t|i|o) << '\n';
#endif
}

vector<string> const cache_result_names = {"NoAccess", "ReadHit", "ReadMiss", "WriteHit", "WriteMiss"};
#endif

void cannot_open(char const * const fname)
{
  cout << "ERROR: Cannot open file " << fname << '\n';
  exit(2);
}

void usage(char const * const app)
{
    cout << "USAGE: " << app << " CACHE_CONFIG.TXT TRACES.TXT" << '\n';
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
    if (entry.is_read)
    {
      l1.read(entry.addr);
    }
    else
    {
      l1.write(entry.addr, 0xaa);
    }
#ifndef NDEBUG
    cout << '\n';
    cout << (entry.is_read ? "READ " : "WRITE") << " "
         << hex << entry.addr << '\n';
    cout << "L1: ";
    paddr(l1, entry.addr);
    cout << "L2: ";
    paddr(l2, entry.addr);
    cout << "Cache Result:" << '\n';
    cout << cache_result_names.at(l1.last_cache_result()) << ' '
         << cache_result_names.at(l2.last_cache_result()) << endl;
#endif
    traceout << l1.last_cache_result() << ' ' << l2.last_cache_result() << '\n';
    l1.clear_cache_result();
    l2.clear_cache_result();
  }
}
