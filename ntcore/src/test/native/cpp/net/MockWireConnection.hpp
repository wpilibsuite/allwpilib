// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/raw_ostream.h>

#include "gmock/gmock.h"
#include "net/WireConnection.h"

namespace nt::net {

class MockWireConnection : public WireConnection {
 public:
  MOCK_METHOD(unsigned int, GetVersion, (), (const, override));

  MOCK_METHOD(void, SendPing, (uint64_t time), (override));

  MOCK_METHOD(bool, Ready, (), (const, override));

  int WriteText(wpi::function_ref<void(wpi::raw_ostream& os)> writer) override {
    std::string text;
    wpi::raw_string_ostream os{text};
    writer(os);
    return DoWriteText(text);
  }
  int WriteBinary(
      wpi::function_ref<void(wpi::raw_ostream& os)> writer) override {
    std::vector<uint8_t> binary;
    wpi::raw_uvector_ostream os{binary};
    writer(os);
    return DoWriteBinary(binary);
  }

  void SendText(wpi::function_ref<void(wpi::raw_ostream& os)> writer) override {
    std::string text;
    wpi::raw_string_ostream os{text};
    writer(os);
    DoSendText(text);
  }
  void SendBinary(
      wpi::function_ref<void(wpi::raw_ostream& os)> writer) override {
    std::vector<uint8_t> binary;
    wpi::raw_uvector_ostream os{binary};
    writer(os);
    DoSendBinary(binary);
  }

  MOCK_METHOD(int, DoWriteText, (std::string_view contents));
  MOCK_METHOD(int, DoWriteBinary, (std::span<const uint8_t> contents));

  MOCK_METHOD(void, DoSendText, (std::string_view contents));
  MOCK_METHOD(void, DoSendBinary, (std::span<const uint8_t> contents));

  MOCK_METHOD(int, Flush, (), (override));

  MOCK_METHOD(uint64_t, GetLastFlushTime, (), (const, override));
  MOCK_METHOD(uint64_t, GetLastReceivedTime, (), (const, override));

  MOCK_METHOD(void, StopRead, (), (override));
  MOCK_METHOD(void, StartRead, (), (override));

  MOCK_METHOD(void, Disconnect, (std::string_view reason), (override));
};

}  // namespace nt::net
