// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdio>
#include <memory>
#include <span>
#include <vector>

#include "gtest/gtest.h"
#include "wpinet/uv/Loop.h"
#include "wpinet/uv/Pipe.h"
#include "wpinet/uv/Timer.h"

namespace wpi {

class WebSocketTest : public ::testing::Test {
 public:
  static const char* pipeName;

  static void SetUpTestCase();

  WebSocketTest() {
    loop = uv::Loop::Create();
    clientPipe = uv::Pipe::Create(loop);
    serverPipe = uv::Pipe::Create(loop);

    serverPipe->Bind(pipeName);

#if 0
    auto debugTimer = uv::Timer::Create(loop);
    debugTimer->timeout.connect([this] {
      std::printf("Active handles:\n");
      uv_print_active_handles(loop->GetRaw(), stdout);
    });
    debugTimer->Start(uv::Timer::Time{100}, uv::Timer::Time{100});
    debugTimer->Unreference();
#endif

    auto failTimer = uv::Timer::Create(loop);
    failTimer->timeout.connect([this] {
      loop->Stop();
      FAIL() << "loop failed to terminate";
    });
    failTimer->Start(uv::Timer::Time{1000});
    failTimer->Unreference();
  }

  ~WebSocketTest() override {
    Finish();
  }

  void Finish() {
    loop->Walk([](uv::Handle& it) { it.Close(); });
  }

  static std::vector<uint8_t> BuildHeader(uint8_t opcode, bool fin,
                                          bool masking, uint64_t len);
  static std::vector<uint8_t> BuildMessage(uint8_t opcode, bool fin,
                                           bool masking,
                                           std::span<const uint8_t> data);
  static void AdjustMasking(std::span<uint8_t> message);
  static const uint8_t testMask[4];

  std::shared_ptr<uv::Loop> loop;
  std::shared_ptr<uv::Pipe> clientPipe;
  std::shared_ptr<uv::Pipe> serverPipe;
};

}  // namespace wpi
