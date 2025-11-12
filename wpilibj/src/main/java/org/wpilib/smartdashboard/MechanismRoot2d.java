// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import org.wpilib.telemetry.TelemetryTable;

/**
 * Root Mechanism2d node.
 *
 * <p>A root is the anchor point of other nodes (such as ligaments).
 *
 * <p>Do not create objects of this class directly! Obtain instances from the {@link
 * Mechanism2d#getRoot(String, double, double)} factory method.
 *
 * <p>Append other nodes by using {@link #append(MechanismObject2d)}.
 */
public final class MechanismRoot2d extends MechanismObject2d {
  private final double[] m_location = new double[2];

  /**
   * Package-private constructor for roots.
   *
   * @param name name
   * @param x x coordinate of root (provide only when constructing a root node)
   * @param y y coordinate of root (provide only when constructing a root node)
   */
  MechanismRoot2d(String name, double x, double y) {
    super(name);
    m_location[0] = x;
    m_location[1] = y;
  }

  /**
   * Set the root's position.
   *
   * @param x new x coordinate
   * @param y new y coordinate
   */
  public synchronized void setPosition(double x, double y) {
    m_location[0] = x;
    m_location[1] = y;
  }

  @Override
  public void updateTelemetry(TelemetryTable table) {
    synchronized (this) {
      table.log("position", m_location);
      super.updateTelemetry(table);
    }
  }
}
