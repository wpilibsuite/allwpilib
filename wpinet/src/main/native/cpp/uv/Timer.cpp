// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Timer.h"

#include <functional>
#include <memory>
#include <utility>

#include "wpinet/uv/Loop.h"

namespace wpi::uv {

std::shared_ptr<Timer> Timer::Create(Loop& loop) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<Timer>(private_init{});
  int err = uv_timer_init(loop.GetRaw(), h->GetRaw());
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Timer::SingleShot(Loop& loop, Time timeout, std::function<void()> func) {
  auto h = Create(loop);
  if (!h) {
    return;
  }
  h->timeout.connect([theTimer = h.get(), f = std::move(func)]() {
    f();
    theTimer->Close();
  });
  h->Start(timeout);
}

void Timer::Start(Time timeout, Time repeat) {
  if (IsLoopClosing()) {
    return;
  }
  Invoke(
      &uv_timer_start, GetRaw(),
      [](uv_timer_t* handle) {
        Timer& h = *static_cast<Timer*>(handle->data);
        h.timeout();
      },
      timeout.count(), repeat.count());
}

}  // namespace wpi::uv
