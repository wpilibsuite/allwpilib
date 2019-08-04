/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Loop.h"

using namespace wpi::uv;

Loop::Loop(const private_init&) noexcept {
#ifndef _WIN32
  // Ignore SIGPIPE (see https://github.com/joyent/libuv/issues/1254)
  static bool once = []() {
    signal(SIGPIPE, SIG_IGN);
    return true;
  }();
  (void)once;
#endif
}

Loop::~Loop() noexcept {
  if (m_loop) {
    m_loop->data = nullptr;
    Close();
  }
}

std::shared_ptr<Loop> Loop::Create() {
  auto loop = std::make_shared<Loop>(private_init{});
  if (uv_loop_init(&loop->m_loopStruct) < 0) return nullptr;
  loop->m_loop = &loop->m_loopStruct;
  loop->m_loop->data = loop.get();
  return loop;
}

std::shared_ptr<Loop> Loop::GetDefault() {
  static std::shared_ptr<Loop> loop = std::make_shared<Loop>(private_init{});
  loop->m_loop = uv_default_loop();
  if (!loop->m_loop) return nullptr;
  loop->m_loop->data = loop.get();
  return loop;
}

void Loop::Close() {
  int err = uv_loop_close(m_loop);
  if (err < 0) ReportError(err);
}

void Loop::Walk(std::function<void(Handle&)> callback) {
  uv_walk(m_loop,
          [](uv_handle_t* handle, void* func) {
            auto& h = *static_cast<Handle*>(handle->data);
            auto& f = *static_cast<std::function<void(Handle&)>*>(func);
            f(h);
          },
          &callback);
}

void Loop::Fork() {
  int err = uv_loop_fork(m_loop);
  if (err < 0) ReportError(err);
}
