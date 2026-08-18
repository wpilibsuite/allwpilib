// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <deque>
#include <vector>

#include "IConnectionList.hpp"

namespace wpi::nt {

class MockConnectionList : public IConnectionList {
 public:
  int AddConnection(const ConnectionInfo& info) override {
    addConnectionCalls.emplace_back(info);
    if (!addConnectionReturns.empty()) {
      int rv = addConnectionReturns.front();
      addConnectionReturns.pop_front();
      return rv;
    }
    return static_cast<int>(addConnectionCalls.size());
  }

  void RemoveConnection(int handle) override {
    removeConnectionCalls.emplace_back(handle);
  }

  void ClearConnections() override { ++clearConnectionsCalls; }

  std::vector<ConnectionInfo> GetConnections() const override {
    return connections;
  }

  bool IsConnected() const override { return connected; }

  std::deque<int> addConnectionReturns;
  std::vector<ConnectionInfo> addConnectionCalls;
  std::vector<int> removeConnectionCalls;
  int clearConnectionsCalls = 0;
  std::vector<ConnectionInfo> connections;
  bool connected = false;
};

}  // namespace wpi::nt
