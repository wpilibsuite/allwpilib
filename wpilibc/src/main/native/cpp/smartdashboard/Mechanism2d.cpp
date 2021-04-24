// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/Mechanism2d.h"

#include <cstdio>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

static constexpr char kBackgroundColor[] = "backgroundColor";
static constexpr char kDims[] = "dims";

Mechanism2d::Mechanism2d(double width, double height, const Color8Bit& backgroundColor)
    : m_width{width}, m_height{height} {
  SetBackgroundColor(backgroundColor);
}

MechanismRoot2d* Mechanism2d::GetRoot(wpi::StringRef name, double x, double y) {
  auto& obj = m_roots[name];
  if (obj) {
    return obj.get();
  }
  obj = std::make_unique<MechanismRoot2d>(name, x, y,
                                          MechanismRoot2d::private_init{});
  MechanismRoot2d* ex = obj.get();
  if (m_table) {
    ex->Update(m_table->GetSubTable(name));
  }
  return ex;
}

void Mechanism2d::SetBackgroundColor(const Color8Bit& color) {
  std::snprintf(m_color, sizeof(m_color), "#%02X%02X%02X", color.red, color.green,
                color.blue);
  if (m_table) {
    m_table->GetEntry(kBackgroundColor).SetString(m_color);
  }
}

void Mechanism2d::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Mechanism2d");
  m_table = builder.GetTable();

  m_table->GetEntry(kDims).SetDoubleArray({m_width, m_height});
  m_table->GetEntry(kBackgroundColor).SetString(m_color);

  std::scoped_lock lock(m_mutex);
  for (const auto& entry : m_roots) {
    const auto& root = entry.getValue().get();
    root->Update(m_table->GetSubTable(entry.getKey()));
  }
}
