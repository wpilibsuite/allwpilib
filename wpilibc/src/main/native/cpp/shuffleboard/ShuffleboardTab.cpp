/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/ShuffleboardTab.h"

using namespace frc;

ShuffleboardTab::ShuffleboardTab(ShuffleboardRoot& root, wpi::StringRef title)
    : ShuffleboardValue(title), ShuffleboardContainer(title), m_root(root) {}

ShuffleboardRoot& ShuffleboardTab::GetRoot() { return m_root; }

void ShuffleboardTab::BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                                std::shared_ptr<nt::NetworkTable> metaTable) {
  auto tabTable = parentTable->GetSubTable(GetTitle());
  tabTable->GetEntry(".type").SetString("ShuffleboardTab");
  for (auto& component : GetComponents()) {
    component->BuildInto(tabTable,
                         metaTable->GetSubTable(component->GetTitle()));
  }
}
