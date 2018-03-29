#include <iostream>
#include <chrono>
#include "asio.hpp"
#include "asio/steady_timer.hpp"


struct TimerItem {
    TimerItem(asio::io_service & io_service, int seconds,
              const std::function<void ()> & f) : timer(io_service),
                                                  duration(seconds),
                                                  fun(f) {}

    asio::steady_timer timer;
    std::chrono::seconds duration;
    std::function<void ()> fun;
};


class TimerManager {
 public:
    TimerManager(asio::io_service & io_service) : io_service_(io_service) {}
    TimerManager(TimerManager &) = delete;
    TimerManager & operator = (const TimerManager &) = delete;

    void Add(const unsigned int & duration,
             const std::function<void ()> & fun) {
        this->items_.push_back(std::make_shared<TimerItem>(
                                   this->io_service_, duration, fun));
    }

    void Run() {
        for(auto & item : this->items_) {
            asio::steady_timer & timer = item->timer;
            const std::chrono::seconds & duration = item->duration;
            const std::function<void ()> & fun = item->fun;
            TimerLoop(timer, duration, fun);
        }
    }

 protected:
    void TimerLoop(asio::steady_timer & timer,
                   const std::chrono::seconds & duration,
                   const std::function<void ()> & fun) {
        timer.expires_from_now(duration);
        timer.async_wait([this, &timer, duration, fun](
                             const asio::error_code &) {
                           std::cout << "async_wait, " << duration.count()
                                     << std::endl;
                           TimerLoop(timer, duration, fun);
                         });
    }

 private:
    asio::io_service & io_service_;
    std::vector< std::shared_ptr<TimerItem> > items_;
};



void Timer1Sec() {
    std::cout << "Timer1Sec." << std::endl;
}

void Timer3Sec() {
    std::cout << "Timer3Sec." << std::endl;
}

class Server
{
 public:
    Server(asio::io_service & io_service) : timer_manager_(io_service) {}

    void Init() {
        //timer_manager_.Add(this, 1, &Server::Timer1Sec);
        //timer_manager_.Add(this, 3, &Server::Timer3Sec);
        timer_manager_.Add(1, Timer1Sec);
        timer_manager_.Add(3, Timer3Sec);
    }

    void Run() {
        timer_manager_.Run();
    }

    /*
    void Timer1Sec() {
        std::cout << "Timer1Sec." << std::endl;
    }

    void Timer3Sec() {
        std::cout << "Timer3Sec." << std::endl;
    }
    */
 private:
    TimerManager timer_manager_;
};


int main() {
    asio::io_service io_service;

    Server server(io_service);
    server.Init();
    server.Run();

    io_service.run();
}
