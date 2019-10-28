#ifndef ASIO_TIMER_ASIO_TIMER_H_
#define ASIO_TIMER_ASIO_TIMER_H_

#include <chrono>
#include <functional>

#include "asio.hpp"
#include "asio/steady_timer.hpp"

namespace asiotimer {

template <typename TimeUnit>
class TimerManager {
 public:
  struct TimerItem {
    TimerItem(asio::io_service & io_service, int seconds,
              const std::function<void ()> & f) : timer(io_service),
                                                  duration(seconds),
                                                  func(f) {}
    asio::steady_timer timer;
    TimeUnit duration;
    std::function<void ()> func;
  };

  TimerManager(asio::io_service & io_service) : io_service_(io_service) {}
  TimerManager(TimerManager &) = delete;
  TimerManager & operator = (const TimerManager &) = delete;

  template <typename T>
  void Add(T * obj, const unsigned int & duration, void (T::* mem_func)()) {
    std::function<void ()> func = std::bind(mem_func, obj);
    this->items_.push_back(std::make_shared<TimerItem>(
                               this->io_service_,
                               duration,
                               func));
  }

  void Run() {
    for (auto & item : this->items_) {
      asio::steady_timer & timer = item->timer;
      const TimeUnit & duration = item->duration;
      const std::function<void ()> & func = item->func;
      TimerLoop(timer, duration, func);
    }
  }

 protected:
  void TimerLoop(asio::steady_timer & timer,
                 const TimeUnit & duration,
                 const std::function<void ()> & func) {
    timer.expires_from_now(duration);
    timer.async_wait(
        [this, &timer, duration, func] (const asio::error_code &) {
          func();
          TimerLoop(timer, duration, func);
        });
  }

 private:
  asio::io_service & io_service_;
  std::vector< std::shared_ptr<TimerItem> > items_;
};

}    // namespace asiotimer

#endif    // ASIO_TIMER_ASIO_TIMER_H_
