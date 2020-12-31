// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/uv/Idle.h"

#include "wpi/uv/Loop.h"

namespace wpi::uv {

std::shared_ptr<Idle> Idle::Create(Loop& loop) {
  auto h = std::make_shared<Idle>(private_init{});
  int err = uv_idle_init(loop.GetRaw(), h->GetRaw());
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Idle::Start() {
  Invoke(&uv_idle_start, GetRaw(), [](uv_idle_t* handle) {
    Idle& h = *static_cast<Idle*>(handle->data);
    h.idle();
  });
}

}  // namespace wpi::uv
