#ifndef __CO_UTIL_H__
#define __CO_UTIL_H__

#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>

using boost::asio::use_awaitable;
using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;
using boost::asio::co_spawn;
using io_context = boost::asio::io_context;
using boost::asio::awaitable;

#endif