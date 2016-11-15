#ifndef MIPSEMU_INTERFACE_H
#define MIPSEMU_INTERFACE_H

#include <vector>

template <typename T>
class Chip {
public:
  virtual void process() = 0;
};


template <typename T>
class Interface {
public:
  T dangling_feet = T();

protected:
  T& data;

  Chip& chip;

public:
  Interface(Chip& chip, T& data): chip(chip), data(data) {}

  virtual void flush() = 0;
};


template <typename T>
class Input: public Interface<T> {
public:
  Input(Chip &chip, T &data = dangling_feet) : Interface(chip, data) {}

  const T& read() const { return data; }

  void set_data_ref(T& data) {
    this->data = data;
  }

  void flush() {
    // TODO: make it async
    chip.process();
  }
};


template <typename T>
class Output: public Interface<T> {
private:
  std::vector<Input<T>&> inputs;

public:
  Output(Chip &chip, T &data) : Interface(chip, data) {}

  Output& attach(Input& input) {
    input.set_data_ref(data);
    inputs.push_back(input);
    return *this;
  }

  void flush() {
    // TODO: make it async
    for (auto input : inputs) input.flush();
  }
};


class ClockSignal {
public:
  static ClockSignal pos_edge;
  static ClockSignal neg_edge;
  static ClockSignal pos_flat;
  static ClockSignal neg_flat;

  bool operator== (const ClockSignal& other) {
    return (void*)this == (void*)&other;
  }
};


template <typename T>
class FlipFlop: public Chip {
private:
  T sample;
  Output<T> out;
  Input<T> in;
  Input<ClockSignal> clock_in;

public:
  FlipFlop()
    : out(Output(*this, sample))
    , in(*this)
    , clock_in(*this)
  {}

  void flush() {
    if (clock_in.read() == ClockSignal::pos_edge) {
      sample = in.read();
      out.flush();
    }
  }
};

#endif //MIPSEMU_INTERFACE_H
