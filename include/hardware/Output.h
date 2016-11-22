#ifndef MIPS_YANGLIU_OUTPUT_1122_1355
#define MIPS_YANGLIU_OUTPUT_1122_1355

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <bitset>
#include "sync/LogicClock.h"
#include "hardware/Chip.h"

namespace mips
{

using term_type = sync::clock_type;

template <typename> class OutputImpl;
template <typename> class SyncOutputImpl;
template <typename> class UnsyncOutputImpl;
/*
 * Provides atomic read/write and mechanism to add/notify receivers
 */
class OutputBase
{
  mutable std::mutex output_access;
  mutable std::vector<Chip const*> receivers;

public:
  virtual ~OutputBase() {}

  template <typename T>
  void write(T const &new_output, term_type write_term)
  {
    if (auto output = dynamic_cast<SyncOutputImpl<T>*>(this))
    {
      std::lock_guard<std::mutex> {output_access};
      output->write(new_output, write_term);
    }
    else if (auto output = dynamic_cast<UnsyncOutputImpl<T>*>(this))
    {
      std::lock_guard<std::mutex> {output_access};
      output->write(new_output);
    }
    else
    {
      throw std::string("cannot write to output");
    }
  }

protected:
  template <typename T>
  T const &read() const
  {
    if (auto output = dynamic_cast<OutputImpl<T>*>(this))
    {
      std::lock_guard<std::mutex> {output_access};
      return output->read();
    }
    else
    {
      throw std::string("cannot read from output");
    }
  }

  void attach_to(Chip const &chip) const
  {
    receivers.push_back(&chip);
  }

  void notify_all(term_type write_term)
  {
    for (auto r : receivers)
      r->notify(write_term);
  }
};

template <typename T>
class OutputImpl : public OutputBase
{
protected:
  T output;

public:
  T const &read() const
  {
    return output;
  }
};

template <typename> class SyncInputImpl;
template <typename T>
class SyncOutputImpl
    : public OutputImpl<T>
    , private sync::LogicClock
{
  friend class SyncInputImpl<T>;

protected:
  using OutputBase::notify_all;
  using OutputImpl<T>::output;

public:
  term_type term() const { return get_clock(); }

  void write(T const &new_output, term_type write_term)
  {
    if (write_term <= term())
      return;
    
    output = new_output;
    forward_clock();
    notify_all(write_term);
  }
};

template <typename> class UnsyncInputImpl;
template <typename T>
class UnsyncOutputImpl : public OutputImpl<T>
{
  friend class UnsyncInputImpl<T>;

protected:
  using OutputBase::notify_all;
  using OutputImpl<T>::output;

public:
  void write(T const &new_output)
  {
    output = new_output;
    notify_all(std::numeric_limits<term_type>::max());
  }
};

template <size_t len>
class SyncOutput : public SyncOutputImpl<std::bitset<len>>
{
};

template <size_t len>
class UnsyncOutput : public UnsyncOutputImpl<std::bitset<len>>
{
};

class Power : public UnsyncOutputImpl<bool>
{
public:
  Power() { turn_on(); }
  void turn_on() { write(true); }
  void turn_off() { write(false); }
};

}
#endif
