// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdio>
#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "wpi/ArrayRef.h"
#include "wpi/uv/Loop.h"
#include "wpi/uv/Pipe.h"
#include "wpi/uv/Timer.h"

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

  ~WebSocketTest() { Finish(); }

  void Finish() {
    loop->Walk([](uv::Handle& it) { it.Close(); });
  }

  static std::vector<uint8_t> BuildHeader(uint8_t opcode, bool fin,
                                          bool masking, uint64_t len);
  static std::vector<uint8_t> BuildMessage(uint8_t opcode, bool fin,
                                           bool masking,
                                           ArrayRef<uint8_t> data);
  static void AdjustMasking(MutableArrayRef<uint8_t> message);
  static const uint8_t testMask[4];

  std::shared_ptr<uv::Loop> loop;
  std::shared_ptr<uv::Pipe> clientPipe;
  std::shared_ptr<uv::Pipe> serverPipe;
};

}  // namespace wpi
