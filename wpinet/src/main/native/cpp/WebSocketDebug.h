// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>

// #define WPINET_WEBSOCKET_VERBOSE_DEBUG
// #define WPINET_WEBSOCKET_VERBOSE_DEBUG_CONTENT

#ifdef __clang__
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#ifdef WPINET_WEBSOCKET_VERBOSE_DEBUG
#define WS_DEBUG(format, ...) \
  ::fmt::print(FMT_STRING(format) __VA_OPT__(, ) __VA_ARGS__)
#else
#define WS_DEBUG(fmt, ...)
#endif
