// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sendable2/SendableSet.h"

using namespace wpi2;

std::vector<SendableTable> SendableSet::GetAll() const {
  std::vector<SendableTable> tables;
  tables.reserve(m_tables.size());
  ForEach([&](auto&& backend) { tables.emplace_back(backend); });
  return tables;
}

void SendableSet::Add(const SendableTable& table) {
  auto backend = table.GetBackend();
  for (auto&& backendWeak : m_tables) {
    if (backendWeak.lock() == backend) {
      return;
    }
  }
  m_tables.emplace_back(table.GetWeak());
}

void SendableSet::Remove(const SendableTable& table) {
  auto backend = table.GetBackend();
  std::erase_if(m_tables, [&](const auto& v) { return v.lock() == backend; });
}
