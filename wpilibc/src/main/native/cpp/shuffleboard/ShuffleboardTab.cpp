// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ShuffleboardTab.h"

#include <memory>

#include <wpi/json.h>

using namespace frc;

static constexpr std::string_view kSmartDashboardType = "ShuffleboardLayout";

ShuffleboardTab::ShuffleboardTab(ShuffleboardRoot& root, std::string_view title)
    : ShuffleboardValue(title), ShuffleboardContainer(title), m_root(root) {}

ShuffleboardRoot& ShuffleboardTab::GetRoot() {
  return m_root;
}

void ShuffleboardTab::BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                                std::shared_ptr<nt::NetworkTable> metaTable) {
  auto tabTable = parentTable->GetSubTable(GetTitle());
  tabTable->GetEntry(".type").SetString(kSmartDashboardType);
  tabTable->GetEntry(".type").GetTopic().SetProperty("SmartDashboard",
                                                     kSmartDashboardType);
  for (auto& component : GetComponents()) {
    component->BuildInto(tabTable,
                         metaTable->GetSubTable(component->GetTitle()));
  }
}
