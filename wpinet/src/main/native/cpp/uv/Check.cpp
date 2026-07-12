// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/uv/Check.hpp"

#include <memory>

#include "wpi/net/uv/Loop.hpp"

namespace wpi::net::uv {

std::shared_ptr<Check> Check::Create(Loop& loop) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<Check>(private_init{});
  int err = uv_check_init(loop.GetRaw(), h->GetRaw());
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Check::Start() {
  if (IsLoopClosing()) {
    return;
  }
  Invoke(&uv_check_start, GetRaw(), [](uv_check_t* handle) {
    Check& h = *static_cast<Check*>(handle->data);
    h.check();
  });
}

}  // namespace wpi::net::uv
