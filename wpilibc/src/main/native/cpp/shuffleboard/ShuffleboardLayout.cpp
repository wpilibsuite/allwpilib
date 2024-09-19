// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ShuffleboardLayout.h"

#include <memory>

#include <wpi/json.h>

using namespace frc;

static constexpr std::string_view kSmartDashboardType = "ShuffleboardLayout";

ShuffleboardLayout::ShuffleboardLayout(ShuffleboardContainer& parent,
                                       std::string_view title,
                                       std::string_view type)
    : ShuffleboardValue(title),
      ShuffleboardComponent(parent, title, type),
      ShuffleboardContainer(title) {
  m_isLayout = true;
}

void ShuffleboardLayout::BuildInto(
    std::shared_ptr<nt::NetworkTable> parentTable,
    std::shared_ptr<nt::NetworkTable> metaTable) {
  BuildMetadata(metaTable);
  auto table = parentTable->GetSubTable(GetTitle());
  table->GetEntry(".type").SetString(kSmartDashboardType);
  table->GetEntry(".type").GetTopic().SetProperty("SmartDashboard",
                                                  kSmartDashboardType);
  for (auto& component : GetComponents()) {
    component->BuildInto(table, metaTable->GetSubTable(component->GetTitle()));
  }
}
