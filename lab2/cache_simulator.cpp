#include "Cache.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

/********************************************************
 * Storage Types
 ********************************************************/
enum CacheAccessResult
{
  NoAccess = 0,
  ReadHit,
  ReadMiss,
  WriteHit,
  WriteMiss
};

struct MemMock
{
  char read(size_t addr) { return 0xff; }
  void write(size_t addr, char value) {}
};

typedef Cache<MemMock> L2_t;
typedef Cache<L2_t> L1_t;

/********************************************************
 * Cache Config
 ********************************************************/
struct CacheConfig
{
  size_t size_block;
  size_t ways_per_index;
  size_t capacity;
};

istream &operator>>(istream &in, CacheConfig &config)
{
  string dummy;
  in >> dummy >> config.size_block >> config.ways_per_index >> config.capacity;
  config.capacity <<= 10;
  return in;
}

/********************************************************
 * Trace Entry
 ********************************************************/
struct TraceEntry
{
  bool is_read;
  size_t addr;
};

istream &operator>>(istream &in, TraceEntry &entry)
{
  char rw;
  in >> rw >> hex >> entry.addr >> dec;
  entry.is_read = rw == 'R';
  return in;
}

/********************************************************
 * Errors
 ********************************************************/
void usage(char const *const app)
{
  cout << "USAGE: " << app << " CACHE_CONFIG.TXT TRACE.TXT" << endl;
  exit(1);
}

template <typename File>
void check_fstream(File &ref, char const *const fname)
{
  if (!ref.is_open())
  {
    cout << "ERROR: Cannot open file " << fname << endl;
    exit(2);
  }
}

/********************************************************
 * Main
 ********************************************************/
int main(int argc, char const *const argv[])
{
  if (argc < 3)
    usage(argv[0]);

  CacheConfig c1, c2;
  ifstream config_in{argv[1]};
  check_fstream(config_in, argv[1]);
  config_in >> c1 >> c2;

  MemMock memory;
  L2_t l2{memory, c2.size_block, c2.ways_per_index, c2.capacity};
  L1_t l1{l2, c1.size_block, c1.ways_per_index, c1.capacity};

  ifstream traces{argv[2]};
  check_fstream(traces, argv[2]);
  string outfname{(ostringstream() << argv[2] << ".out").str()};
  ofstream traceout{outfname};
  check_fstream(traceout, outfname.c_str());

  TraceEntry entry;
  while (traces.good())
  {
    traces >> entry;
    if (traces.eof())
      break;
    
    size_t addr = entry.addr;
    if (entry.is_read)
    {
      traceout << (l1.hit(addr) ? ReadHit : ReadMiss) << ' ';
      if (l1.hit(addr)) traceout << NoAccess << '\n';
      else traceout << (l2.hit(addr) ? ReadHit : ReadMiss) << '\n';
      l1.read(addr);
    }
    else
    {
      traceout << (l1.hit(addr) ? WriteHit : WriteMiss) << ' ';
      if (l1.hit(addr)) traceout << NoAccess << '\n';
      else traceout << (l2.hit(addr) ? WriteHit : WriteMiss) << '\n';
      l1.write(addr, 0xaa);
    }
  }
}
