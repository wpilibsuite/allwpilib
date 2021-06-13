// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "networktables/NTSendableBuilder.h"

using namespace nt;

NTSendableBuilder::BackendKind NTSendableBuilder::GetBackendKind() const {
  return kNetworkTables;
}
