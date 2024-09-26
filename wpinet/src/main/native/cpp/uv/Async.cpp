// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Async.h"

#include <memory>

#include "wpinet/uv/Loop.h"

namespace wpi::uv {

Async<>::~Async() noexcept {
  if (auto loop = m_loop.lock()) {
    Close();
  } else {
    ForceClosed();
  }
}

std::shared_ptr<Async<>> Async<>::Create(const std::shared_ptr<Loop>& loop) {
  if (loop->IsClosing()) {
    return nullptr;
  }
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

}  // namespace wpi::uv
