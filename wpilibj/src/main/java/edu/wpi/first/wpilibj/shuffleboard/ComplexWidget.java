// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilderImpl;

/**
 * A Shuffleboard widget that handles a {@link Sendable} object such as a motor controller or
 * sensor.
 */
public final class ComplexWidget extends ShuffleboardWidget<ComplexWidget> {
  private final Sendable m_sendable;
  private SendableBuilderImpl m_builder;

  ComplexWidget(ShuffleboardContainer parent, String title, Sendable sendable) {
    super(parent, title);
    m_sendable = sendable;
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    if (m_builder == null) {
      m_builder = new SendableBuilderImpl();
      m_builder.setTable(parentTable.getSubTable(getTitle()));
      m_sendable.initSendable(m_builder);
      m_builder.startListeners();
    }
    m_builder.update();
  }

  /**
   * Enables user control of this widget in the Shuffleboard application. This method is
   * package-private to prevent users from enabling control themselves. Has no effect if the
   * sendable is not marked as an actuator with {@link SendableBuilder#setActuator}.
   */
  void enableIfActuator() {
    if (m_builder.isActuator()) {
      m_builder.startLiveWindowMode();
    }
  }

  /**
   * Disables user control of this widget in the Shuffleboard application. This method is
   * package-private to prevent users from enabling control themselves. Has no effect if the
   * sendable is not marked as an actuator with {@link SendableBuilder#setActuator}.
   */
  void disableIfActuator() {
    if (m_builder.isActuator()) {
      m_builder.stopLiveWindowMode();
    }
  }
}
