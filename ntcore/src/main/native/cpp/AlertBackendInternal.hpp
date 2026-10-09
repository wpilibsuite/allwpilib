// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/nt/ntcore_c.h"

namespace wpi::nt::detail {
// Detach publication before reset/destruction, without holding the publisher
// mutex while NT shuts down listener threads.
void DetachAlertBackendInstance(NT_Inst instance);
}  // namespace wpi::nt::detail
