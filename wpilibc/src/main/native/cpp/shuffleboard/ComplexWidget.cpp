// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ComplexWidget.h"

#include <memory>

#include <wpi/sendable/Sendable.h>

#include "frc/smartdashboard/SendableBuilderImpl.h"

using namespace frc;

ComplexWidget::ComplexWidget(ShuffleboardContainer& parent,
                             std::string_view title, wpi::Sendable& sendable)
    : ShuffleboardValue(title),
      ShuffleboardWidget(parent, title),
      m_sendable(sendable) {}

ComplexWidget::~ComplexWidget() = default;

void ComplexWidget::EnableIfActuator() {
  if (m_builder && static_cast<SendableBuilderImpl&>(*m_builder).IsActuator()) {
    static_cast<SendableBuilderImpl&>(*m_builder).StartLiveWindowMode();
  }
}

void ComplexWidget::DisableIfActuator() {
  if (m_builder && static_cast<SendableBuilderImpl&>(*m_builder).IsActuator()) {
    static_cast<SendableBuilderImpl&>(*m_builder).StopLiveWindowMode();
  }
}

void ComplexWidget::BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                              std::shared_ptr<nt::NetworkTable> metaTable) {
  BuildMetadata(metaTable);
  if (!m_builder) {
    m_builder = std::make_unique<SendableBuilderImpl>();
    static_cast<SendableBuilderImpl&>(*m_builder)
        .SetTable(parentTable->GetSubTable(GetTitle()));
    m_sendable.InitSendable(static_cast<SendableBuilderImpl&>(*m_builder));
    static_cast<SendableBuilderImpl&>(*m_builder).StartListeners();
  }
  m_builder->Update();
}
