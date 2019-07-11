/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Idle.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

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

}  // namespace uv
}  // namespace wpi
