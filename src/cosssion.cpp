#include "cosession.h"
#include "coserver.h"
#include <boost/asio/read.hpp>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unistd.h>

uint64_t coSession::number_ = 0;

coSession::coSession(io_context &ioc, coServer *coserver)
    : ioc_(ioc), sk_(ioc), co_server_(coserver),
      recv_node_(std::make_shared<messageNode>()) {}

void coSession::close() { stop_ = true; sk_.close(); co_server_->clear(id_); }

void coSession::start() {
  auto shared_this = shared_from_this();
  co_spawn(
    ioc_,
      [this, shared_this = std::move(shared_this)]() -> awaitable<void> {
        try {
          while (!stop_) {
            // int32_t recv_len = boost::asio::read(sk_,
            //     boost::asio::buffer(recv_node_->data_, MESG_SIZE)
            //     /*boost::asio::use_awaitable*/);
            int32_t recv_len = sk_.read_some(boost::asio::buffer(recv_node_->data_, MESG_SIZE));
            recv_node_->size_ = recv_len;
            recv_node_->data_[recv_len] = 0;
            printf("Received: %d\n", recv_len);
            send(recv_node_->data_, recv_node_->size_);
          }
        } catch (const std::exception &e) {
            std::cout << e.what() << '\n';
            close();
            co_return;
        }
      },
      boost::asio::detached);
}

void coSession::send(const char *data, int32_t size) {
  bool empty;
  {
    std::unique_lock<std::mutex> lg(mtx_);
    empty = send_queue_.empty();
    send_queue_.push(std::make_shared<messageNode>(data, size));
  }
  if (empty) {
    co_spawn(
      sk_.get_executor(),
      [&]() -> awaitable<void> {
        while (!send_queue_.empty()) {
          co_await coWrite(*send_queue_.front());
          {
            std::unique_lock<std::mutex> lg(mtx_);
            send_queue_.pop();
          }
        }
        co_return;
      },
      boost::asio::detached);
  }
}

awaitable<void> coSession::coWrite(messageNode &mesg) {
  co_await boost::asio::async_write(sk_, boost::asio::buffer(mesg.data_, mesg.size_),
                                use_awaitable);
  co_return;
}