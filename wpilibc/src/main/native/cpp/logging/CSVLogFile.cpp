/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/CSVLogFile.h"

#include <chrono>

#include <wpi/raw_ostream.h>

using namespace frc;

CSVLogFile::CSVLogFile(wpi::StringRef filePrefix)
    : m_logFile(filePrefix, "csv") {}
