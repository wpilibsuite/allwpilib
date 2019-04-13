/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Prepare.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

std::shared_ptr<Prepare> Prepare::Create(Loop& loop) {
  auto h = std::make_shared<Prepare>(private_init{});
  int err = uv_prepare_init(loop.GetRaw(), h->GetRaw());
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Prepare::Start() {
  Invoke(&uv_prepare_start, GetRaw(), [](uv_prepare_t* handle) {
    Prepare& h = *static_cast<Prepare*>(handle->data);
    h.prepare();
  });
}

}  // namespace uv
}  // namespace wpi
