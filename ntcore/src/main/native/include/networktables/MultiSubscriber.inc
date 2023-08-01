// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "networktables/MultiSubscriber.h"

namespace nt {

inline MultiSubscriber::MultiSubscriber(
    NetworkTableInstance inst, std::span<const std::string_view> prefixes,
    const PubSubOptions& options)
    : m_handle{::nt::SubscribeMultiple(inst.GetHandle(), prefixes, options)} {}

inline MultiSubscriber::MultiSubscriber(MultiSubscriber&& rhs)
    : m_handle{rhs.m_handle} {
  rhs.m_handle = 0;
}

inline MultiSubscriber& MultiSubscriber::operator=(MultiSubscriber&& rhs) {
  if (m_handle != 0) {
    ::nt::UnsubscribeMultiple(m_handle);
  }
  m_handle = rhs.m_handle;
  rhs.m_handle = 0;
  return *this;
}

inline MultiSubscriber::~MultiSubscriber() {
  if (m_handle != 0) {
    ::nt::UnsubscribeMultiple(m_handle);
  }
}

}  // namespace nt
