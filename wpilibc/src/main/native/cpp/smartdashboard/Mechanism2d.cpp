// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/Mechanism2d.h"

#include <memory>
#include <string_view>

#include <networktables/NTSendableBuilder.h>

using namespace frc;

static constexpr std::string_view kBackgroundColor = "backgroundColor";
static constexpr std::string_view kDims = "dims";

Mechanism2d::Mechanism2d(double width, double height,
                         const Color8Bit& backgroundColor)
    : m_width{width}, m_height{height} {
  SetBackgroundColor(backgroundColor);
}

MechanismRoot2d* Mechanism2d::GetRoot(std::string_view name, double x,
                                      double y) {
  auto [it, isNew] =
      m_roots.try_emplace(name, name, x, y, MechanismRoot2d::private_init{});
  if (isNew && m_table) {
    it->second.Update(m_table->GetSubTable(name));
  }
  return &it->second;
}

void Mechanism2d::SetBackgroundColor(const Color8Bit& color) {
  m_color = color.HexString();
  if (m_colorPub) {
    m_colorPub.Set(m_color);
  }
}

void Mechanism2d::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("Mechanism2d");

  std::scoped_lock lock(m_mutex);
  m_table = builder.GetTable();
  m_dimsPub = m_table->GetDoubleArrayTopic(kDims).Publish();
  m_dimsPub.Set({{m_width, m_height}});
  m_colorPub = m_table->GetStringTopic(kBackgroundColor).Publish();
  m_colorPub.Set(m_color);
  for (auto& entry : m_roots) {
    entry.second.Update(m_table->GetSubTable(entry.first));
  }
}
