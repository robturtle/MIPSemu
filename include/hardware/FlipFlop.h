#ifndef MIPS_YANGLIU_FLIPFLOP_1124_0545
#define MIPS_YANGLIU_FLIPFLOP_1124_0545

#include "hardware/Chip.h"

namespace mips
{

template <size_t len>
class FlipFlop : public Chip
{
public:
  INPUT(sample_in, len);
  OUTPUT(sample_out, len);
  INPUT(clock_in, 1);

private:
  std::bitset<len> sample;

  Computation<FlipFlop> logic{[](auto ff) {
    ff.sample = ff.sample_in.read();
    ff.sample_out.write(ff.sample);
  }};

public:
  FlipFlop() : Chip(logic, {&clock_in}) {}
};

class Power : public Chip
{
  Computation<Power> logic{[](auto){}};
public:
  OUTPUT(power_out, 1);

  Power() : Chip(logic, {})
  {
    power_out.write(1);
  }
};

struct SyncedChip {};

class Impulser : public Chip, public SyncedChip
{
public:
  INPUT(clock_in, 1);
  OUTPUT(clock_out, 1);

private:
  Computation<Impulser> logic{[this](auto clock) {
    clock.clock_out.write(1);
  }};

public:
  Impulser() : Chip(logic, {&clock_in}) {}
};

template <typename C>
struct DelayedComputation : public Computation<C>
{
  using Computation<C>::compute;
  Computation<C> &origin;
  std::chrono::milliseconds const delay;

  DelayedComputation(Computation<C> &origin, std::chrono::milliseconds const delay)
      : origin(origin), delay(delay)
  {
    compute = [this](auto c) {
      std::this_thread::sleep_for(this->delay);
      this->origin(c);
    };
  }
};

template <typename C>
struct KeySteppedComputation : public Computation<C>
{// TODO
};

} /* ns mips */

#endif
