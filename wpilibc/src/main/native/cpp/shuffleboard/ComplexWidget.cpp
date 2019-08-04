/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/ComplexWidget.h"

#include "frc/smartdashboard/Sendable.h"

using namespace frc;

ComplexWidget::ComplexWidget(ShuffleboardContainer& parent,
                             const wpi::Twine& title, Sendable& sendable)
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
