#include <iostream>
#include <chrono>

#include "asio.hpp"

#include "asio_timer.h"

class Server {
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
  asiotimer::TimerManager<std::chrono::seconds> timer_manager_;
};


int main() {
  asio::io_service io_service;

  Server server(io_service);
  server.Init();
  server.Run();

  io_service.run();
}
