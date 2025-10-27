// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "ntcore_cpp.h"

namespace nt {

class IConnectionList {
 public:
  virtual ~IConnectionList() = default;

  virtual int AddConnection(const ConnectionInfo& info) = 0;
  virtual void RemoveConnection(int handle) = 0;
  virtual void ClearConnections() = 0;
  virtual std::vector<ConnectionInfo> GetConnections() const = 0;
  virtual bool IsConnected() const = 0;
};

}  // namespace nt
