// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ComplexWidget.h"

#include "frc/smartdashboard/Sendable.h"

using namespace frc;

ComplexWidget::ComplexWidget(ShuffleboardContainer& parent,
                             std::string_view title, Sendable& sendable)
    : ShuffleboardValue(title),
      ShuffleboardWidget(parent, title),
      m_sendable(sendable) {}

void ComplexWidget::EnableIfActuator() {
  if (m_builder.IsActuator()) {
    m_builder.StartLiveWindowMode();
  }
}

void ComplexWidget::DisableIfActuator() {
  if (m_builder.IsActuator()) {
    m_builder.StopLiveWindowMode();
  }
}

void ComplexWidget::BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                              std::shared_ptr<nt::NetworkTable> metaTable) {
  BuildMetadata(metaTable);
  if (!m_builderInit) {
    m_builder.SetTable(parentTable->GetSubTable(GetTitle()));
    m_sendable.InitSendable(m_builder);
    m_builder.StartListeners();
    m_builderInit = true;
  }
  m_builder.UpdateTable();
}
