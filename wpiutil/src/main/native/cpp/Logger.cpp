// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/Logger.h"

using namespace wpi;

void Logger::DoLog(unsigned int level, const char* file, unsigned int line,
                   const char* msg) {
  if (!m_func || level < m_min_level) {
    return;
  }
  m_func(level, file, line, msg);
}

void Logger::LogV(unsigned int level, const char* file, unsigned int line,
                  fmt::string_view format, fmt::format_args args) {
  if (!m_func || level < m_min_level) {
    return;
  }
  fmt::memory_buffer out;
  fmt::vformat_to(fmt::appender{out}, format, args);
  out.push_back('\0');
  m_func(level, file, line, out.data());
}
