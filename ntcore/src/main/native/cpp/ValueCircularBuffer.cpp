// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ValueCircularBuffer.h"

using namespace nt;

std::vector<Value> ValueCircularBuffer::ReadValue() {
  std::vector<Value> rv;
  rv.reserve(m_storage.size());
  for (auto&& val : m_storage) {
    rv.emplace_back(std::move(val));
  }
  m_storage.reset();
  return rv;
}
