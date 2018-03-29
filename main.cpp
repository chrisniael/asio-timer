#include <iostream>
#include <chrono>
#include "asio.hpp"
#include "asio/steady_timer.hpp"


struct TimerItem {
    TimerItem(asio::io_service & io, int seconds, const std::function<void ()> & f) : timer(io), duration(seconds), fun(f) {}

    asio::steady_timer timer;
    std::chrono::seconds duration;
    std::function<void ()> fun;
};


void timer(asio::steady_timer & t, const std::chrono::seconds & duration, const std::function<void ()> & fun) {

        t.expires_from_now(duration);
        t.async_wait([&t, duration, fun](const asio::error_code &) {
                std::cout << "async_wait, " << duration.count() << std::endl;
                fun();
                timer(t, duration, fun);
        });
}


void timer_2_sec() {
    std::cout << "timer_2_sec" << std::endl;
}

void timer_5_sec() {
    std::cout << "timer_5_sec" << std::endl;
}


int main() {
    asio::io_service io;

    std::vector< std::shared_ptr<TimerItem> > vec;
    vec.push_back(std::make_shared<TimerItem>(io, 2, timer_2_sec));
    vec.push_back(std::make_shared<TimerItem>(io, 5, timer_5_sec));

    for(auto & item : vec) {
        asio::steady_timer & t = item->timer;
        const std::chrono::seconds & duration = item->duration;
        const std::function<void ()> & fun = item->fun;
        timer(t, duration, fun);
    }

    io.run();
}
