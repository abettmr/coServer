#ifndef __CO_SERVER_H__
#define __CO_SERVER_H__

#include "co_util.h"
#include "cosession.h"
#include "iocontext_pool.h"
#include "singleton.h"
#include <boost/asio/detached.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <sys/types.h>
#include <system_error>
#include <unordered_map>
#include "co_util.h"

class coSession;

class coServer/*: public singleton<ioContextPool>*/{
    std::vector<std::shared_ptr<coSession>> sessions_;
    std::unordered_map<uint64_t, std::shared_ptr<coSession>> map_;
    tcp::acceptor acceptor_;
    uint32_t port_=-1;
    bool stop_=false;
    std::shared_ptr<coSession> sock_;
    std::mutex mtx_;
    public:
    coServer(io_context &ioc, uint32_t port);
    ~coServer();
    void clear(uint64_t id) { std::lock_guard<std::mutex> lg(mtx_); map_.erase(id); }
    private:
    void startAccept();

    void handleAccept(const boost::system::error_code &ec);

    awaitable<void> startAccept(int);
    // awaitable<void> startAccept() {
    //     co_spawn(acceptor_.get_executor(),
    //     [&]()->awaitable<void>{
    //             auto sock_ptr = make_shared<tcp::socket>(ioc_pool_->getIOContext());
    //             try {
    //                 co_await acceptor_.async_accept(*sock_ptr, use_awaitable);
    //                 sessions_.push_back(sock_ptr);
    //                 std::cout << "Client connected: " << sock_ptr->remote_endpoint() << std::endl;
    //                 co_spawn(acceptor_.get_executor(), startAccept(), boost::asio::detached);
    //                 co_return;
    //             } catch (const std::exception &e) {
    //                 sock_ptr.reset();
    //                 std::cout << e.what() << std::endl;
    //                 co_return;
    //             }
    //         },
    //     boost::asio::detached
    //     );
    //     co_return;
    // }

};

#endif
