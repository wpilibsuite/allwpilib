// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <FRC_FPGA_ChipObject/fpgainterfacecapi/NiFpga.h>

namespace hal {
namespace init {
[[nodiscard]] int InitializeFPGA();
}  // namespace init

using HAL_NiFpga_ReserveIrqContextFunc =
    NiFpga_Status (*)(NiFpga_Session session, NiFpga_IrqContext* context);

extern HAL_NiFpga_ReserveIrqContextFunc HAL_NiFpga_ReserveIrqContext;

using HAL_NiFpga_UnreserveIrqContextFunc =
    NiFpga_Status (*)(NiFpga_Session session, NiFpga_IrqContext context);

extern HAL_NiFpga_UnreserveIrqContextFunc HAL_NiFpga_UnreserveIrqContext;

using HAL_NiFpga_WaitOnIrqsFunc = NiFpga_Status (*)(
    NiFpga_Session session, NiFpga_IrqContext context, uint32_t irqs,
    uint32_t timeout, uint32_t* irqsAsserted, NiFpga_Bool* timedOut);

extern HAL_NiFpga_WaitOnIrqsFunc HAL_NiFpga_WaitOnIrqs;

using HAL_NiFpga_AcknowledgeIrqsFunc = NiFpga_Status (*)(NiFpga_Session session,
                                                         uint32_t irqs);

extern HAL_NiFpga_AcknowledgeIrqsFunc HAL_NiFpga_AcknowledgeIrqs;

using HAL_NiFpga_OpenHmbFunc = NiFpga_Status (*)(const NiFpga_Session session,
                                                 const char* memoryName,
                                                 size_t* memorySize,
                                                 void** virtualAddress);

extern HAL_NiFpga_OpenHmbFunc HAL_NiFpga_OpenHmb;

using HAL_NiFpga_CloseHmbFunc = NiFpga_Status (*)(const NiFpga_Session session,
                                                  const char* memoryName);

extern HAL_NiFpga_CloseHmbFunc HAL_NiFpga_CloseHmb;
}  // namespace hal
