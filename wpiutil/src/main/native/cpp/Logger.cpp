// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Logger.hpp"

#include <format>
#include <iterator>
#include <vector>

using namespace wpi::util;

void Logger::DoLog(unsigned int level, const char* file, unsigned int line,
                   const char* msg) {
  if (!m_func || level < m_min_level) {
    return;
  }
  m_func(level, file, line, msg);
}

void Logger::LogV(unsigned int level, const char* file, unsigned int line,
                  std::string_view format, std::format_args args) {
  if (!m_func || level < m_min_level) {
    return;
  }
  std::vector<char> out;
  std::vformat_to(std::back_inserter(out), format, args);
  out.push_back('\0');
  m_func(level, file, line, out.data());
}
