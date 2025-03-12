// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.telemetry.TelemetryTable;

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
  private double m_x;
  private double m_y;

  /**
   * Package-private constructor for roots.
   *
   * @param name name
   * @param x x coordinate of root (provide only when constructing a root node)
   * @param y y coordinate of root (provide only when constructing a root node)
   */
  MechanismRoot2d(String name, double x, double y) {
    super(name);
    m_x = x;
    m_y = y;
  }

  /**
   * Set the root's position.
   *
   * @param x new x coordinate
   * @param y new y coordinate
   */
  public synchronized void setPosition(double x, double y) {
    m_x = x;
    m_y = y;
  }

  @Override
  public synchronized void updateTelemetry(TelemetryTable table, boolean first) {
    table.log("x", m_x);
    table.log("y", m_y);
    super.updateTelemetry(table, first);
  }
}
