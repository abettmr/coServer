#ifndef __SESSION_H__
#define __SESSION_H__

#include "co_util.h"
#include "coserver.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <cstdint>
#include <cstring>
#include <memory>
#include <queue>

#define MESG_SIZE 4096

class coServer;

struct messageNode {
    int32_t size_=0;
    char* data_ = nullptr;
    messageNode(int32_t size = MESG_SIZE):data_(new char[size]{}){}
    messageNode(const char *data, int32_t size):messageNode(size) {
        memcpy(data_, data, size_);
        size_ = size;
    }
    ~messageNode() { delete[] data_; }
};

class coSession: public std::enable_shared_from_this<coSession>{
    public:
    coSession(io_context &ioc, coServer *coserver);

    ~coSession() {
        close();
    }

    tcp::socket& getSock() {
        return sk_;
    }
    
    uint64_t getID() { return id_; }

    void start();

    void close();
    void send(const char *data, int32_t size);
    void send(const std::string &data) { send(data.c_str(), data.size()); };
    private:
    coServer *co_server_ = nullptr;
    tcp::socket sk_;
    std::shared_ptr<messageNode> recv_node_;
    std::queue<std::shared_ptr<messageNode>> send_queue_;
    std::mutex mtx_;
    uint64_t id_ = 0;
    static uint64_t number_;
    bool stop_ = false;
    awaitable<void> coWrite(messageNode &mesg);
    io_context &ioc_;
};

#endif
