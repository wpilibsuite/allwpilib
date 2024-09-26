// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/SimpleWidget.h"

#include <memory>

#include "frc/shuffleboard/Shuffleboard.h"
#include "frc/shuffleboard/ShuffleboardLayout.h"
#include "frc/shuffleboard/ShuffleboardTab.h"

using namespace frc;

SimpleWidget::SimpleWidget(ShuffleboardContainer& parent,
                           std::string_view title)
    : ShuffleboardValue(title), ShuffleboardWidget(parent, title), m_entry() {}

nt::GenericEntry* SimpleWidget::GetEntry() {
  if (!m_entry) {
    ForceGenerate();
  }
  return &m_entry;
}

nt::GenericEntry* SimpleWidget::GetEntry(std::string_view typeString) {
  if (!m_entry) {
    m_typeString = typeString;
    ForceGenerate();
  }
  return &m_entry;
}

void SimpleWidget::BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                             std::shared_ptr<nt::NetworkTable> metaTable) {
  BuildMetadata(metaTable);
  if (!m_entry) {
    m_entry = parentTable->GetTopic(GetTitle()).GetGenericEntry(m_typeString);
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
