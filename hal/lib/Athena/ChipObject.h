/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

#include <stdint.h>

#include "FRC_FPGA_ChipObject/RoboRIO_FRC_ChipObject_Aliases.h"
#include "FRC_FPGA_ChipObject/tDMAChannelDescriptor.h"
#include "FRC_FPGA_ChipObject/tDMAManager.h"
#include "FRC_FPGA_ChipObject/tInterruptManager.h"
#include "FRC_FPGA_ChipObject/tSystem.h"
#include "FRC_FPGA_ChipObject/tSystemInterface.h"

#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/nInterfaceGlobals.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAccel.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAccumulator.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAI.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAlarm.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAnalogTrigger.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tAO.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tBIST.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tCounter.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tDIO.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tDMA.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tEncoder.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tGlobal.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tInterrupt.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tPower.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tPWM.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tRelay.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tSPI.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tSysWatchdog.h"

// FIXME: these should not be here!
using namespace nFPGA;
using namespace nRoboRIO_FPGANamespace;
#pragma GCC diagnostic pop
