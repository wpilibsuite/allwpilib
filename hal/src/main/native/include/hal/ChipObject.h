// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

#include <stdint.h>

#include <FRC_FPGA_ChipObject/RoboRIO_FRC_ChipObject_Aliases.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/nInterfaceGlobals.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAI.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAO.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAccel.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAccumulator.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAlarm.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAnalogTrigger.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tBIST.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tCounter.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tDIO.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tDMA.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tDutyCycle.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tEncoder.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tGlobal.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tHMB.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tInterrupt.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tLED.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tPWM.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tPower.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tRelay.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tSPI.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tSysWatchdog.h>
#include <FRC_FPGA_ChipObject/tDMAChannelDescriptor.h>
#include <FRC_FPGA_ChipObject/tDMAManager.h>
#include <FRC_FPGA_ChipObject/tInterruptManager.h>
#include <FRC_FPGA_ChipObject/tSystem.h>
#include <FRC_FPGA_ChipObject/tSystemInterface.h>

namespace hal {
using namespace nFPGA;
using namespace nRoboRIO_FPGANamespace;
}  // namespace hal

#pragma GCC diagnostic pop
