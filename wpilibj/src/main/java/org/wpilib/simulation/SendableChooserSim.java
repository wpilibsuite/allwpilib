// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.framework.RobotBase;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.StringPublisher;

/** Class that facilitates control of a SendableChooser's selected option in simulation. */
public class SendableChooserSim implements AutoCloseable {
  private StringPublisher m_publisher;

  /**
   * Constructs a SendableChooserSim.
   *
   * @param path The path where the SendableChooser is published.
   */
  public SendableChooserSim(String path) {
    this(NetworkTableInstance.getDefault(), path);
  }

  /**
   * Constructs a SendableChooserSim.
   *
   * @param inst The NetworkTables instance.
   * @param path The path where the SendableChooser is published.
   */
  public SendableChooserSim(NetworkTableInstance inst, String path) {
    if (RobotBase.isSimulation()) {
      m_publisher = inst.getStringTopic(path + "selected").publish();
    }
  }

  @Override
  public void close() {
    if (RobotBase.isSimulation()) {
      m_publisher.close();
    }
  }

  /**
   * Set the selected option.
   *
   * @param option The option.
   */
  public void setSelected(String option) {
    if (RobotBase.isSimulation()) {
      m_publisher.set(option);
    }
  }
}
