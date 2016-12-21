#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

enum SaturatingCounterState
{
  StrongNotTaken = 0,
  WeakNotTaken,
  WeakTaken,
  StrongTaken
};

class SaturatingCounter
{
  int state;

public:
  SaturatingCounter() : state(StrongTaken) {}

  bool predict()
  {
    return state >> 1;
  }

  void update(bool last_is_taken)
  {
    if (!(state == StrongTaken && last_is_taken) &&
        !(state == StrongNotTaken && !last_is_taken))
    {
      int change = last_is_taken ? 1 : -1;
      if (state ^ (state + change))
        change *= 2;
      state += change;
    }
  }
};

template <typename File>
void check_fstream(File &ref, char const *const fname)
{
  if (!ref.is_open())
  {
    cout << "ERROR: Cannot open file " << fname << endl;
    exit(2);
  }
}

int main(int argc, char const *const argv[])
{
  ios_base::sync_with_stdio(false);
  if (argc != 3)
  {
    cout << "USAGE: " << argv[0] << " CONFIG.TXT TRACE.TXT";
    exit(1);
  }
  ifstream config{argv[1]};
  check_fstream(config, argv[1]);
  ifstream trace{argv[2]};
  check_fstream(trace, argv[2]);
  string outname = argv[2] + string(".out");
  ofstream out{outname};
  check_fstream(out, outname.c_str());

  size_t branch_index_size;
  config >> branch_index_size;
  size_t counter_size = 1 << branch_index_size;
  size_t mask = (1 << branch_index_size) - 1;
  vector<SaturatingCounter> counters{counter_size};

  size_t addr;
  bool taken;
  trace >> hex;
  while (true)
  {
    trace >> addr >> taken;
    if (trace.eof())
      break;
    size_t index = addr & mask;
    SaturatingCounter &counter = counters.at(index);
    out << counter.predict() << '\n';
    counter.update(taken);
  }
}
