// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/Mechanism2d.hpp"

#include <string_view>

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi;

static constexpr std::string_view kBackgroundColor = "backgroundColor";
static constexpr std::string_view kDims = "dims";

Mechanism2d::Mechanism2d(double width, double height,
                         const wpi::util::Color8Bit& backgroundColor)
    : m_width{width}, m_height{height} {
  SetBackgroundColor(backgroundColor);
}

MechanismRoot2d* Mechanism2d::GetRoot(std::string_view name, double x,
                                      double y) {
  auto [it, isNew] =
      m_roots.try_emplace(name, name, x, y, MechanismRoot2d::private_init{});
  return &it->second;
}

void Mechanism2d::SetBackgroundColor(const wpi::util::Color8Bit& color) {
  m_color = color.HexString();
}

void Mechanism2d::LogTo(wpi::TelemetryTable& table) const {
  std::scoped_lock lock(m_mutex);
  table.Log(kDims, {m_width, m_height});
  table.Log(kBackgroundColor, m_color);
  for (auto& entry : m_roots) {
    table.Log(entry.first, entry.second);
  }
}

std::string_view Mechanism2d::GetTelemetryType() const {
  return "Mechanism2d";
}
