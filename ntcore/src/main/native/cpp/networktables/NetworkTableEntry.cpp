// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/NetworkTableEntry.hpp"

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/Topic.hpp"

using namespace wpi::nt;

NetworkTableInstance NetworkTableEntry::GetInstance() const {
  return NetworkTableInstance{GetInstanceFromHandle(m_handle)};
}

Topic NetworkTableEntry::GetTopic() const {
  return Topic{::wpi::nt::GetTopicFromHandle(m_handle)};
}
