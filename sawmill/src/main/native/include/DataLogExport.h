// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/DataLogReaderThread.h>

namespace sawmill {
    struct DataLogRecord {
        const wpi::log::StartRecordData entryData;
        wpi::log::DataLogRecord record;
    };
}