/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilderImpl;

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
    m_builder.updateTable();
  }

}
