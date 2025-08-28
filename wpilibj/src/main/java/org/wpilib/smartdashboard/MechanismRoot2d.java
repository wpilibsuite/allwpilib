// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.NetworkTable;

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
  private DoublePublisher m_xPub;
  private double m_y;
  private DoublePublisher m_yPub;

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

  @Override
  public void close() {
    if (m_xPub != null) {
      m_xPub.close();
    }
    if (m_yPub != null) {
      m_yPub.close();
    }
    super.close();
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
    flush();
  }

  @Override
  protected synchronized void updateEntries(NetworkTable table) {
    if (m_xPub != null) {
      m_xPub.close();
    }
    m_xPub = table.getDoubleTopic("x").publish();
    if (m_yPub != null) {
      m_yPub.close();
    }
    m_yPub = table.getDoubleTopic("y").publish();
    flush();
  }

  private void flush() {
    if (m_xPub != null) {
      m_xPub.set(m_x);
    }
    if (m_yPub != null) {
      m_yPub.set(m_y);
    }
  }
}
