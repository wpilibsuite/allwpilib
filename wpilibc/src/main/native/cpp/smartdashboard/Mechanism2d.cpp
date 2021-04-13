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
  for (auto&& [name, root] : m_roots) {
    std::scoped_lock lock2(root.m_mutex);
    root.UpdateEntries(m_table->GetSubTable(name));
  }
}
