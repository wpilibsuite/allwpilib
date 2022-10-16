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
  MockWireConnection() : m_text_os{m_text}, m_binary_os{m_binary} {}

  MOCK_METHOD(bool, Ready, (), (const, override));

  TextWriter SendText() override { return {m_text_os, *this}; }
  BinaryWriter SendBinary() override { return {m_binary_os, *this}; }

  MOCK_METHOD(void, Text, (std::string_view contents));
  MOCK_METHOD(void, Binary, (std::span<const uint8_t> contents));

  MOCK_METHOD(void, Flush, (), (override));

  MOCK_METHOD(void, Disconnect, (std::string_view reason), (override));

 protected:
  void StartSendText() override;
  void FinishSendText() override;
  void StartSendBinary() override {}
  void FinishSendBinary() override {
    Binary(m_binary);
    m_binary.resize(0);
  }

 private:
  std::string m_text;
  wpi::raw_string_ostream m_text_os;
  std::vector<uint8_t> m_binary;
  wpi::raw_uvector_ostream m_binary_os;
  bool m_in_text{false};
};

}  // namespace nt::net
