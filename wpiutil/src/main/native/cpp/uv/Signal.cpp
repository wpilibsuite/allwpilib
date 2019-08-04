/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Signal.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

std::shared_ptr<Signal> Signal::Create(Loop& loop) {
  auto h = std::make_shared<Signal>(private_init{});
  int err = uv_signal_init(loop.GetRaw(), h->GetRaw());
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Signal::Start(int signum) {
  Invoke(&uv_signal_start, GetRaw(),
         [](uv_signal_t* handle, int signum) {
           Signal& h = *static_cast<Signal*>(handle->data);
           h.signal(signum);
         },
         signum);
}

}  // namespace uv
}  // namespace wpi
