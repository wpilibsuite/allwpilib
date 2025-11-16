// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "gmock/gmock.h"
#include "wpi/util/Logger.hpp"

namespace wpi {

class MockLogger : public wpi::util::Logger,
                   public ::testing::MockFunction<void(
                       unsigned int level, std::string_view file,
                       unsigned int line, std::string_view msg)> {
 public:
  MockLogger() {
    SetLogger([this](unsigned int level, const char* file, unsigned int line,
                     const char* msg) { Call(level, file, line, msg); });
  }
};

}  // namespace wpi
