// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/smartdashboard/Mechanism2d.h"

#include <ios>

#include <wpi/raw_ostream.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

Mechanism2d::Mechanism2d(double width, double height) : m_dims{width, height} {}

MechanismRoot2d& Mechanism2d::GetRoot(const wpi::StringRef name, double x, double y) {
  if (m_roots.count(name)) {
    return m_roots[name];
  }
  wpi::Twine& twine {name};
  MechanismRoot2d root {twine, x, y};
  m_roots.try_emplace(name, root);
  root.Update(m_table->GetSubTable(name));
  return root;
}

void Mechanism2d::SetBackgroundColor(const Color8Bit& color) {
  std::string buf;
  wpi::raw_string_ostream os{buf};
  os << "#";
  os << std::hex << (color.red << 16 | color.green << 8 | color.blue);
  if (m_table) {
    m_table->GetEntry(kBackgroundColor).SetString(buf);
  }
}

void Mechanism2d::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Mechanism2d");
  m_table = builder.GetTable();

  std::scoped_lock lock(m_mutex);
  for (auto& entry : m_roots) {
    auto& root = entry.getValue();
    std::scoped_lock lock2(root.m_mutex);
    root.Update(m_table->GetSubTable(entry.getKey()));
  }
}
