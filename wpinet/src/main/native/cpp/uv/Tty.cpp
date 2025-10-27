// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/uv/Tty.hpp"

#include <memory>

#include "wpi/net/uv/Loop.hpp"

namespace wpi::uv {

std::shared_ptr<Tty> Tty::Create(Loop& loop, uv_file fd, bool readable) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<Tty>(private_init{});
  int err = uv_tty_init(loop.GetRaw(), h->GetRaw(), fd, readable ? 1 : 0);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

}  // namespace wpi::uv
