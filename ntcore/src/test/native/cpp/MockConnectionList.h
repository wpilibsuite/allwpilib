// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "IConnectionList.h"
#include "gmock/gmock.h"

namespace nt {

class MockConnectionList : public IConnectionList {
 public:
  MOCK_METHOD(int, AddConnection, (const ConnectionInfo& info), (override));
  MOCK_METHOD(void, RemoveConnection, (int handle), (override));
  MOCK_METHOD(void, ClearConnections, (), (override));
  MOCK_METHOD(std::vector<ConnectionInfo>, GetConnections, (),
              (const, override));
  MOCK_METHOD(bool, IsConnected, (), (const, override));
};

}  // namespace nt
