// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.wpilibj.RobotBase;

/** A SendableChooserSim allows you to select a SendableChooser's option from code. */
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
   * @param inst The NetworkTable instance.
   * @param path The path where the SendableChooser is published.
   */
  public SendableChooserSim(NetworkTableInstance inst, String path) {
    if (RobotBase.isSimulation()) {
      m_publisher = inst.getStringTopic(path + "selected").publish();
    }
  }

  @Override
  public void close() {
    m_publisher.close();
  }

  public void setSelected(String option) {
    m_publisher.set(option);
  }
}
