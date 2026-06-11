// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "wpi/util/Logger.hpp"

namespace wpi {

class MockLogger : public wpi::util::Logger {
 public:
  struct Message {
    unsigned int level;
    std::string file;
    unsigned int line;
    std::string msg;
  };

  MockLogger() {
    SetLogger([this](unsigned int level, const char* file, unsigned int line,
                     const char* msg) {
      messages.emplace_back(level, file, line, msg);
    });
  }

  bool HasMessage(std::string_view msg) const {
    for (const auto& message : messages) {
      if (message.msg == msg) {
        return true;
      }
    }
    return false;
  }

  bool HasMessage(unsigned int level, std::string_view msg) const {
    for (const auto& message : messages) {
      if (message.level == level && message.msg == msg) {
        return true;
      }
    }
    return false;
  }

  std::vector<Message> messages;
};

}  // namespace wpi
