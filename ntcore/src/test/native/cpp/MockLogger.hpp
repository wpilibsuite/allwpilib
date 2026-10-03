// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

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

  struct ExpectedMessage {
    unsigned int level;
    std::string_view msg;
  };

  void CheckMessages(std::initializer_list<ExpectedMessage> expected) const {
    REQUIRE(messages.size() == expected.size());

    auto expectedIt = expected.begin();
    for (const auto& message : messages) {
      CHECK(message.level == expectedIt->level);
      CHECK(std::string_view{message.msg} == expectedIt->msg);
      ++expectedIt;
    }
  }

  void CheckMessage(unsigned int level, std::string_view expected) const {
    CheckMessages({{level, expected}});
  }

  std::vector<Message> messages;
};

}  // namespace wpi
