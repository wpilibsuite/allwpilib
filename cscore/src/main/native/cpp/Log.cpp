// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Log.h"

void cs::NamedLogV(wpi::Logger& logger, unsigned int level, const char* file,
                   unsigned int line, std::string_view name,
                   fmt::string_view format, fmt::format_args args) {
  fmt::memory_buffer out;
  fmt::format_to(fmt::appender{out}, "{}: ", name);
  fmt::vformat_to(fmt::appender{out}, format, args);
  out.push_back('\0');
  logger.DoLog(level, file, line, out.data());
}
