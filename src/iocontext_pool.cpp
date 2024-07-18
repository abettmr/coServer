#include "iocontext_pool.h"
#include "co_util.h"
#include <cstdint>
#include <iostream>

ioContextPool::ioContextPool(int32_t size)
    : num_(size), context_vec_(size), works_(size) {
  for (int32_t i = 0; i < num_; i++)
    works_[i] = std::unique_ptr<work>(new work(context_vec_[i]));
  for (int32_t i = 0; i < num_; i++) {
    thread_vec_.emplace_back([i, this] {
        std::cout << "running " << i << '\n';
        context_vec_[i].run();
    });
  }
}

io_context &ioContextPool::getIOContext() {
  if (index_ == num_)
    index_ = 0;
  return context_vec_[index_++];
}

void ioContextPool::stop() {
  for (auto i = 0; i < num_; i++) {
    works_[i].reset();
    thread_vec_[i].join();
  }
}

ioContextPool& ioContextPool::getInst() {
  static ioContextPool inst(std::thread::hardware_concurrency() - 1);
  return inst;
}