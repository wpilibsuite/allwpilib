// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Log.hpp"

#include <format>
#include <iterator>
#include <vector>

void wpi::cs::NamedLogV(wpi::util::Logger& logger, unsigned int level,
                        const char* file, unsigned int line,
                        std::string_view name, std::string_view format,
                        std::format_args args) {
  std::vector<char> out;
  std::format_to(std::back_inserter(out), "{}: ", name);
  std::vformat_to(std::back_inserter(out), format, args);
  out.push_back('\0');
  logger.DoLog(level, file, line, out.data());
}
