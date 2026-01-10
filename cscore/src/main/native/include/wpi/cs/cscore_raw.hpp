// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <string_view>

#include "wpi/cs/VideoMode.hpp"
#include "wpi/cs/cscore_c.h"
#include "wpi/util/RawFrame.h"

namespace wpi::cs {

/**
 * @defgroup cscore_raw_func Raw Image Functions
 * @{
 */

CS_Source CreateRawSource(std::string_view name, bool isCv,
                          const VideoMode& mode, CS_Status* status);

CS_Sink CreateRawSink(std::string_view name, bool isCv, CS_Status* status);
CS_Sink CreateRawSinkCallback(std::string_view name, bool isCv,
                              std::function<void(uint64_t time)> processFrame,
                              CS_Status* status);

void PutSourceFrame(CS_Source source, const WPI_RawFrame& image,
                    CS_Status* status);
uint64_t GrabSinkFrame(CS_Sink sink, WPI_RawFrame& image, CS_Status* status);
uint64_t GrabSinkFrameTimeout(CS_Sink sink, WPI_RawFrame& image, double timeout,
                              CS_Status* status);
uint64_t GrabSinkFrameTimeoutLastTime(CS_Sink sink, WPI_RawFrame& image,
                                      double timeout, uint64_t lastFrameTime,
                                      CS_Status* status);

/** @} */

}  // namespace wpi::cs
