#ifndef MIPS_YANGLIU_CHIP_1122_1355
#define MIPS_YANGLIU_CHIP_1122_1355

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <bitset>
#include <map>
#include <future>
#include "sync/LogicClock.h"

namespace mips
{

/******************************************************
 * Output
 ******************************************************/
class Chip;
template <size_t> class Input;

template <size_t len>
class Output
{
  friend class Input<len>;

  mutable std::vector<Input<len> const*> receivers;

public:
  using output_type = std::bitset<len>;

protected:
  Chip const &owner;
  std::atomic<output_type> output;

public:
  Output(Chip &owner) : owner(owner) {}

  void write(output_type const &new_output);

  bool is_dangling() const
  {
    return receivers.empty();
  }

protected:
  output_type read() const
  {
    return output;
  }

  void notify_all_receivers(sync::clock_type term);

  void add_receiver(Input<len> const *receiver)
  {
    receivers.push_back(receiver);
  }
};

/******************************************************
 * Input
 ******************************************************/

// Provides synchronization with Chips
class InputBase
{
protected:
  Chip const &owner;

  sync::clock_type latest_term;

public:
  InputBase(Chip const &owner) : owner(owner) {}

  sync::clock_type get_clock() const
  {
    return latest_term;
  }
};

template <size_t len>
class Input : public InputBase
{
  friend class Output<len>;

public:
  using input_type = std::bitset<len>;

protected:
  Output<len> *source = nullptr;

public:
  Input(Chip const &owner) : InputBase(owner) {}

  input_type read() const
  {
    return source->read();
  }

  bool is_dangling() const
  {
    return source == nullptr;
  }
  
  void attach_to(Output<len> &output)
  {
    source = &output;
    source->add_receiver(this);
  }

protected:
  void notify(sync::clock_type term);
};

/******************************************************
 * Chip
 ******************************************************/
struct ComputeLogic
{
  virtual void operator()(Chip const &chip) = 0;
};

template <typename C>
struct Computation : public ComputeLogic
{
  std::function<void(C const&)> compute;

  Computation(std::function<void(C const &)> && compute)
      : compute(compute) {}

  template <typename F>
  Computation(F f) : compute(std::function<void(C const &)>(f)) {}

  void operator()(Chip const &chip) override
  {
    compute(static_cast<C const&>(chip));
  }
};

#define INPUT(name, len) Input<len> name{*this}
#define OUTPUT(name, len) Output<len> name{*this}

class Chip : private sync::LogicClock
{
  template <size_t> friend class Input;

  mutable std::mutex chip_access;
  mutable std::condition_variable message_arrival;

protected:
  std::vector<InputBase const*> sync_inputs;
  INPUT(power_in, 1);

public:
  ComputeLogic &compute;

  using sync::LogicClock::get_clock;

  void power_on(Output<1> &power);

protected:
  Chip(
      ComputeLogic &compute,
      std::vector<InputBase const *> sync_inputs)
      : compute(compute), sync_inputs(sync_inputs) {}

  void notify(sync::clock_type term) const;
};

/******************************************************
 * message flow
 ******************************************************/

template <size_t len>
void Output<len>::write(output_type const &new_output)
{
  output = new_output;
  notify_all_receivers(owner.get_clock());
}

template <size_t len>
void Output<len>::notify_all_receivers(sync::clock_type term)
{
  for (Input<len> const *r : receivers)
    r->notify(term);
}

template <size_t len>
void Input<len>::notify(sync::clock_type term)
{
  if (term <= latest_term)
    return;
  latest_term = term;
  owner.notify(term);
}

void Chip::notify(sync::clock_type term) const
{
  if (term > get_clock())
    message_arrival.notify_one();
}

void Chip::power_on(Output<1> &power)
{
  power_in.attach_to(power);
  while (true)
  {
    std::unique_lock<std::mutex> lk{chip_access};
    sync::clock_type term = get_clock();
    message_arrival.wait(lk, [this, &term] {
      return power_in.read().none() ||
             std::all_of(
               sync_inputs.begin(), sync_inputs.end(),
               [&term](auto pin) { return pin->get_clock() > term; });
    });
    if (power_in.read().none())
      break;
    std::async(std::launch::async, [this] { compute(*this); });
  }
}

} /* ns mips */
#endif
