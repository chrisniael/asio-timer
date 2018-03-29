#include <iostream>
#include <chrono>
#include "asio.hpp"
#include "asio/steady_timer.hpp"

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
    void Add(T * obj, const unsigned int & duration,
             void (T::* mem_func)()) {
        std::function<void ()> func = std::bind(mem_func, obj);
        this->items_.push_back(std::make_shared<TimerItem>(
                                   this->io_service_, duration, func));
    }

    void Run() {
        for(auto & item : this->items_) {
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
        timer.async_wait([this, &timer, duration, func](
                             const asio::error_code &) {
                           std::cout << "async_wait, " << duration.count()
                                     << std::endl;
                           TimerLoop(timer, duration, func);
                         });
    }

 private:
    asio::io_service & io_service_;
    std::vector< std::shared_ptr<TimerItem> > items_;
};


class Server
{
 public:
    Server(asio::io_service & io_service) : timer_manager_(io_service) {}

    void Init() {
        timer_manager_.Add(this, 1, &Server::Timer1Sec);
        timer_manager_.Add(this, 3, &Server::Timer3Sec);
    }

    void Run() {
        timer_manager_.Run();
    }

    void Timer1Sec() {
        std::cout << "Timer1Sec." << std::endl;
    }

    void Timer3Sec() {
        std::cout << "Timer3Sec." << std::endl;
    }
 private:
    TimerManager<std::chrono::seconds> timer_manager_;
};


int main() {
    asio::io_service io_service;

    Server server(io_service);
    server.Init();
    server.Run();

    io_service.run();
}
