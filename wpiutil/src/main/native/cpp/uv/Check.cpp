/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Check.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

std::shared_ptr<Check> Check::Create(Loop& loop) {
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
  Invoke(&uv_check_start, GetRaw(), [](uv_check_t* handle) {
    Check& h = *static_cast<Check*>(handle->data);
    h.check();
  });
}

}  // namespace uv
}  // namespace wpi
