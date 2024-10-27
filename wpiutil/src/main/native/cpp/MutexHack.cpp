// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifdef _WIN32
#include <threads.h>
#endif

namespace wpi::hack {
void CallACThreadFunction() {
#ifdef _WIN32
  mtx_t Mtx{};
  mtx_init(&Mtx, mtx_plain);
#endif
}
}  // namespace wpi::hack
