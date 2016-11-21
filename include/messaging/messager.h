#ifndef MESSAGING_YANGLIU_MESSAGER_1120
#define MESSAGING_YANGLIU_MESSAGER_1120

#include <shared_mutex>
#include <mutex>
#include <condition_variable>
#include <future>
#include <vector>
#include <thread>
#include "clock/logic_clock.h"

#include <iostream>

namespace messaging
{

struct Messager : protected clocks::LogicClock
{
  using clocks::LogicClock::clock_type;
  using clocks::LogicClock::get_clock;
  virtual ~Messager() {}
};

template <typename Msg>
class MessageAdaptor;

template <typename Msg>
class MessageSender : public Messager
{
  friend class MessageAdaptor<Msg>;

  Msg msg;
  mutable std::shared_timed_mutex msg_access;
  mutable std::vector<MessageAdaptor<Msg> const*> subscribers;

public:
  MessageSender(Msg const &msg) : msg(msg) {}
  MessageSender() : MessageSender(Msg()) {}

  Msg const &read() const
  {
    std::shared_lock<std::shared_timed_mutex> {msg_access};
    return msg;
  }

  void write(Msg const &new_msg, clock_type clock_value)
  {
    if (clock_value < get_clock()) return;

    {
      std::unique_lock<std::shared_timed_mutex>{msg_access};
      msg = new_msg;
      forward_clock();
    }
    for (auto s : subscribers)
        s->notify();
  }

private:
  void attach_to(MessageAdaptor<Msg> *adaptor) const
  {
    subscribers.push_back(adaptor);
  }
};

template <typename Msg>
class UnsyncMessageSender : public MessageSender<Msg>
{
private:
  void write(Msg const &new_msg, Messager::clock_type clock_value)
  {
    MessageSender<Msg>::write(new_msg, clock_value);
  }

public:
  void write(Msg const &new_msg)
  {
    write(new_msg, std::numeric_limits<Messager::clock_type>::max());
  }
};

class MessageHandlerBase : protected clocks::LogicClock
{
protected:
  template <typename Msg>
  friend class MessageAdaptor;

  mutable std::mutex entity_access;
  mutable std::condition_variable message_arrival;
  mutable std::vector<Messager const*> senders;

public:
  using LogicClock::get_clock;

  virtual void start(UnsyncMessageSender<bool> *terminator) = 0;

protected:
  virtual void process(clock_type const& clock_value) = 0;

private:
  void notify() const
  {
    message_arrival.notify_one();
  }

  void sync_with(Messager const *sender) const
  {
    senders.push_back(sender);
  }
};

template <typename Msg>
class MessageAdaptor
{
  friend class MessageHandler;

  MessageSender<Msg> const *sender;
  MessageHandlerBase const *handler;

public:
  MessageAdaptor(
      MessageHandlerBase const *handler,
      MessageSender<Msg> const *sender = nullptr)
      : handler(handler), sender(sender) {}

  Msg const &read() const { return sender->read(); }

  void notify() const { handler->notify(); }

  void attach_to(MessageSender<Msg> const *new_sender)
  {
    sender = new_sender;
    sender->attach_to(this);
    handler->sync_with(sender);
  }

  void attach_to(UnsyncMessageSender<Msg> const *new_sender)
  {
    sender = new_sender;
    sender->attach_to(this);
  }
};

class MessageHandler : public MessageHandlerBase
{
  MessageAdaptor<bool> power_line{this};

public:
  void start(UnsyncMessageSender<bool> *terminator) override
  {
    power_line.attach_to(terminator);
    while (true)
    {
      clock_type clock_value = get_clock();
      std::unique_lock<std::mutex> lk{entity_access};
      message_arrival.wait(lk, [this, &clock_value, &terminator] {
        return !terminator->read() ||
               std::all_of(
                   senders.begin(), senders.end(),
                   [&clock_value](auto s) { return s->get_clock() > clock_value; });
      });
      if (!terminator->read())
        break;
      forward_clock();
      std::async(std::launch::async, [this] { process(get_clock()); });
    }
  }
};

}

#endif
