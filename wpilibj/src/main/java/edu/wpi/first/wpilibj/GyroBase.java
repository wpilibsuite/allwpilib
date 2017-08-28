/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.interfaces.Gyro;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;

/**
 * GyroBase is the common base class for Gyro implementations such as AnalogGyro.
 */
public abstract class GyroBase extends SensorBase implements Gyro, PIDSource, LiveWindowSendable {
  private PIDSourceType m_pidSource = PIDSourceType.kDisplacement;

  /**
   * Set which parameter of the gyro you are using as a process control variable. The Gyro class
   * supports the rate and displacement parameters
   *
   * @param pidSource An enum to select the parameter.
   */
  @Override
  public void setPIDSourceType(PIDSourceType pidSource) {
    m_pidSource = pidSource;
  }

  @Override
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Get the output of the gyro for use with PIDControllers. May be the angle or rate depending on
   * the set PIDSourceType
   *
   * @return the output according to the gyro
   */
  @Override
  public double pidGet() {
    switch (m_pidSource) {
      case kRate:
        return getRate();
      case kDisplacement:
        return getAngle();
      default:
        return 0.0;
    }
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  @Override
  public String getSmartDashboardType() {
    return "Gyro";
  }

  private NetworkTable m_table;
  private NetworkTableEntry m_valueEntry;

  @Override
  public void initTable(NetworkTable subtable) {
    m_table = subtable;
    if (m_table != null) {
      m_valueEntry = m_table.getEntry("Value");
      updateTable();
    } else {
      m_valueEntry = null;
    }
  }

  @Override
  public NetworkTable getTable() {
    return m_table;
  }

  @Override
  public void updateTable() {
    if (m_valueEntry != null) {
      m_valueEntry.setDouble(getAngle());
    }
  }

  @Override
  public void startLiveWindowMode() {
  }

  @Override
  public void stopLiveWindowMode() {
  }
}
