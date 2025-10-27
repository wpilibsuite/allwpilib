// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/NTSendable.hpp"

#include <wpi/util/sendable/SendableBuilder.hpp>

#include "wpi/nt/NTSendableBuilder.hpp"

using namespace nt;

void NTSendable::InitSendable(wpi::SendableBuilder& builder) {
  if (builder.GetBackendKind() == wpi::SendableBuilder::kNetworkTables) {
    InitSendable(static_cast<NTSendableBuilder&>(builder));
  }
}
