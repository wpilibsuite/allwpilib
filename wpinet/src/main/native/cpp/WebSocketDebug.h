// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/Logger.h>

#define WPINET_WEBSOCKET_VERBOSE_DEBUG_CONTENT

#ifdef __clang__
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#define WS_DEBUG(stream, format, ...)                              \
  if (auto logger_ = stream.GetLogger()) {                         \
    WPI_DEBUG4(*logger_, "WS: " format __VA_OPT__(, ) __VA_ARGS__) \
  }
