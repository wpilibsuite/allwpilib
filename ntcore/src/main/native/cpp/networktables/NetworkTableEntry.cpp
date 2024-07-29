// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "networktables/NetworkTableEntry.h"

#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"

using namespace nt;

NetworkTableInstance NetworkTableEntry::GetInstance() const {
  return NetworkTableInstance{GetInstanceFromHandle(m_handle)};
}

Topic NetworkTableEntry::GetTopic() const {
  return Topic{::nt::GetTopicFromHandle(m_handle)};
}
