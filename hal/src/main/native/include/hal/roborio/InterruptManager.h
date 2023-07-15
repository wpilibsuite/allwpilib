// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <FRC_FPGA_ChipObject/fpgainterfacecapi/NiFpga.h>
#include <wpi/mutex.h>

#include "hal/ChipObject.h"
#include "hal/Types.h"

namespace hal {
class InterruptManager {
 public:
  static InterruptManager& GetInstance();
  static void Initialize(tSystemInterface* baseSystem);

  NiFpga_IrqContext GetContext() noexcept;
  void ReleaseContext(NiFpga_IrqContext context) noexcept;

  uint32_t WaitForInterrupt(NiFpga_IrqContext context, uint32_t mask,
                            bool ignorePrevious, uint32_t timeoutInMs,
                            int32_t* status);

 private:
  InterruptManager() = default;

  wpi::priority_mutex currentMaskMutex;
  uint32_t currentMask;
  NiFpga_Session fpgaSession;
};
}  // namespace hal
