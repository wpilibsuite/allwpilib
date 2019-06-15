/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/AsyncExecutor.h"

namespace wpi {
namespace uv {
namespace detail {

AsyncExecutorHandle::~AsyncExecutorHandle() noexcept {
  if (auto loop = m_loop.lock())
    this->Close();
  else
    this->ForceClosed();
}

std::shared_ptr<AsyncExecutorHandle> AsyncExecutorHandle::Create(
    const std::shared_ptr<Loop>& loop) {
  auto h = std::make_shared<AsyncExecutorHandle>(loop, private_init{});
  int err = uv_async_init(loop->GetRaw(), h->GetRaw(), [](uv_async_t* handle) {
    auto& h = *static_cast<AsyncExecutorHandle*>(handle->data);
    std::unique_lock<wpi::mutex> lock(h.m_mutex);

    for (size_t i = 0; i < h.m_queue.size(); ++i) {
      auto v = h.m_queue[i];
      lock.unlock();
      v.func(v.factory, v.req);
      lock.lock();
    }
    h.m_queue.clear();
  });
  if (err < 0) {
    loop->ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

}  // namespace detail
}  // namespace uv
}  // namespace wpi
