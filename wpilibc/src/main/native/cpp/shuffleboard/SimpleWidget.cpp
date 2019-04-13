/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/SimpleWidget.h"

#include "frc/shuffleboard/Shuffleboard.h"
#include "frc/shuffleboard/ShuffleboardLayout.h"
#include "frc/shuffleboard/ShuffleboardTab.h"

using namespace frc;

SimpleWidget::SimpleWidget(ShuffleboardContainer& parent,
                           const wpi::Twine& title)
    : ShuffleboardValue(title), ShuffleboardWidget(parent, title), m_entry() {}

nt::NetworkTableEntry SimpleWidget::GetEntry() {
  if (!m_entry) {
    ForceGenerate();
  }
  return m_entry;
}

void SimpleWidget::BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                             std::shared_ptr<nt::NetworkTable> metaTable) {
  BuildMetadata(metaTable);
  if (!m_entry) {
    m_entry = parentTable->GetEntry(GetTitle());
  }
}

void SimpleWidget::ForceGenerate() {
  ShuffleboardContainer* parent = &GetParent();

  while (parent->m_isLayout) {
    parent = &(static_cast<ShuffleboardLayout*>(parent)->GetParent());
  }

  auto& tab = *static_cast<ShuffleboardTab*>(parent);
  tab.GetRoot().Update();
}
