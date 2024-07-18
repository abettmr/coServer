#include "coserver.h"
#include "cosession.h"
#include "iocontext_pool.h"
#include <mutex>
#include <thread>

coServer::coServer(io_context &ioc, uint32_t port)
    : acceptor_(ioc, tcp::endpoint(tcp::v4(), port)), port_(port) {
  std::cout << "listening on port: " << port << '\n';
  startAccept();
  // co_spawn(acceptor_.get_executor(), startAccept(), boost::asio::detached);
}

coServer::~coServer() {
  stop_ = true;
  for (size_t i = 0; i < sessions_.size(); i++) {
    // sessions_[i].stop();
    sessions_[i].reset();
  }
  std::cout << "~coServer()" << std::endl;
}

void coServer::startAccept() {
  sock_ = make_shared<coSession>(ioContextPool::getInst().getIOContext(), this);
  acceptor_.async_accept(
      sock_->getSock(),
      std::bind(&coServer::handleAccept, this, std::placeholders::_1));
}

void coServer::handleAccept(const boost::system::error_code &ec) {
  if (!ec) {
    {
        std::lock_guard<std::mutex> lg(mtx_);
        map_[sock_->getID()] = sock_;
    }
    // sock_->send("Hola!\n");
    sock_->start();
    std::cout << "Client connected: " << sock_->getSock().remote_endpoint()
              << std::endl;
  } else {
    std::cout << ec.what() << '\n';
    return;
  }
  startAccept();
}

awaitable<void> coServer::startAccept(int) {
  while (!stop_) {
    auto sock_ptr =
        make_shared<coSession>(ioContextPool::getInst().getIOContext(), this);
    try {
      co_await acceptor_.async_accept(sock_->getSock(), use_awaitable);
      sessions_.push_back(sock_ptr);
      std::cout << "Client connected: " << sock_ptr->getSock().remote_endpoint()
                << std::endl;
    } catch (const std::exception &e) {
      sock_ptr.reset();
      std::cout << e.what() << std::endl;
      co_return;
    }
  }
  co_return;
}