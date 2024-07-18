
#ifndef __IOCONTEXT_POOL_H__
#define __IOCONTEXT_POOL_H__

#include "co_util.h"
#include "singleton.h"
#include <memory>
#include <sys/types.h>
#include <thread>
#include <vector>

using work = io_context::work;
using workPtr = std::unique_ptr<work>;

class ioContextPool : public singleton<ioContextPool> {
  std::vector<io_context> context_vec_;
  std::vector<workPtr> works_;
  std::vector<std::thread> thread_vec_;
  uint16_t index_ = 0;
  uint16_t num_ = 0;

public:
  ioContextPool(int32_t size);

  io_context &getIOContext();
  void stop();
  static ioContextPool& getInst();
  ~ioContextPool()=default;
};

#endif
