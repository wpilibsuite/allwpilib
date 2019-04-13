/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Async.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

Async<>::~Async() noexcept {
  if (auto loop = m_loop.lock())
    Close();
  else
    ForceClosed();
}

std::shared_ptr<Async<>> Async<>::Create(const std::shared_ptr<Loop>& loop) {
  auto h = std::make_shared<Async>(loop, private_init{});
  int err = uv_async_init(loop->GetRaw(), h->GetRaw(), [](uv_async_t* handle) {
    Async& h = *static_cast<Async*>(handle->data);
    h.wakeup();
  });
  if (err < 0) {
    loop->ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

}  // namespace uv
}  // namespace wpi
