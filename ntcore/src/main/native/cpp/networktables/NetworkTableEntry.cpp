// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/ntcore/NetworkTableEntry.hpp"

#include "wpi/ntcore/NetworkTableInstance.hpp"
#include "wpi/ntcore/Topic.hpp"

using namespace wpi::nt;

NetworkTableInstance NetworkTableEntry::GetInstance() const {
  return NetworkTableInstance{GetInstanceFromHandle(m_handle)};
}

Topic NetworkTableEntry::GetTopic() const {
  return Topic{::wpi::nt::GetTopicFromHandle(m_handle)};
}
