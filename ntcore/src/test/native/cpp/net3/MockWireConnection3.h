// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <vector>

#include <wpi/raw_ostream.h>

#include "gmock/gmock.h"
#include "net3/WireConnection3.h"

namespace nt::net3 {

class MockWireConnection3 : public WireConnection3 {
 public:
  MockWireConnection3() : m_os{m_data} {}

  MOCK_METHOD(bool, Ready, (), (const, override));

  Writer Send() override { return {m_os, *this}; }

  MOCK_METHOD(void, Data, (std::span<const uint8_t> data));

  MOCK_METHOD(void, Flush, (), (override));

  MOCK_METHOD(uint64_t, GetLastFlushTime, (), (const, override));

  MOCK_METHOD(void, StopRead, (), (override));
  MOCK_METHOD(void, StartRead, (), (override));

  MOCK_METHOD(void, Disconnect, (std::string_view reason), (override));

 protected:
  void FinishSend() override {
    Data(m_data);
    m_data.resize(0);
  }

 private:
  std::vector<uint8_t> m_data;
  wpi::raw_uvector_ostream m_os;
};

}  // namespace nt::net3
