/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Tty.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

std::shared_ptr<Tty> Tty::Create(Loop& loop, uv_file fd, bool readable) {
  auto h = std::make_shared<Tty>(private_init{});
  int err = uv_tty_init(loop.GetRaw(), h->GetRaw(), fd, readable ? 1 : 0);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

}  // namespace uv
}  // namespace wpi
