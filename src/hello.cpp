#include "co_util.h"
#include "coserver.h"
#include "iocontext_pool.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/detail/error_code.hpp>
#include <csignal>
#include <iostream>

using boost::asio::use_awaitable;
using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;
using boost::asio::co_spawn;
using io_context = boost::asio::io_context;
using boost::asio::awaitable;
// void handleHttp(ioctx &ioc, const std::string &server, const std::string
// &path){
//     tcp::resolver resolver(ioc);
//     auto endpt = resolver.resolve(server, "http");
//     tcp::socket socket(ioc);
//     boost::asio::connect(socket, endpt);

// }

int main() {
  std::cout << "\n\
                     #####                                     \n\
       ####   ####  #     # ###### #####  #    # ###### #####  \n\
      #    # #    # #       #      #    # #    # #      #    # \n\
      #      #    #  #####  #####  #    # #    # #####  #    # \n\
      #      #    #       # #      #####  #    # #      #####  \n\
      #    # #    # #     # #      #   #   #  #  #      #   #  \n\
       ####   ####   #####  ###### #    #   ##   ###### #    # \n\
      \n\
      coServer started.\n";
  try {
    io_context ioc;
    io_context::work work(ioc);
    boost::asio::signal_set sigset(ioc, SIGTERM, SIGINT);
    // tcp::acceptor acpt(ioc, tcp::endpoint(tcp::v4(), 10010));
    // co_spawn(ioc, [&]()->awaitable<void>{
    //     boost::asio::ip::tcp::socket sock(ioc);
    //     co_await acpt.async_accept(sock, use_awaitable);
    // }, boost::asio::detached);
    ioContextPool &ioc_pool = ioContextPool::getInst();
    coServer csrv(ioc, 10086);
    sigset.async_wait([&](const boost::system::error_code &ec, int signal) {
      ioc.stop();
      ioc_pool.stop();
      if (!ec) {
        std::cout << "Signal received: " << signal << std::endl;
        exit(signal);
      } else {
        std::cout << ec.what() << std::endl;
        exit(ec.value());
      }
    });
    std::cout << "ok\n";
    ioc.run();
  } catch (const std::exception &e) {
    std::cout << e.what() << '\n';
    exit(1);
  }
}