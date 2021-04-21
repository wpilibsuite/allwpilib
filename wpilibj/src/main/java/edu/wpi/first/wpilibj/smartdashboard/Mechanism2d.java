// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

/**
 * Visual 2D representation of arms, elevators, and general mechanisms; through a node-based API.
 *
 * <p>A Mechanism2d object is published and contains at least one root node. Other nodes (such as
 * ligaments) are recursively based on other nodes.
 *
 * @see MechanismObject2d
 * @see MechanismLigament2d
 * @see MechanismRoot2d
 */
public final class Mechanism2d implements Sendable {
  private static final String kBackgroundColor = "backgroundColor";
  private NetworkTable m_table;
  private final Map<String, MechanismRoot2d> m_roots;
  private final double[] m_dims = new double[2];
  private String m_color = "#5050FF";

  /**
   * Create a new Mechanism2d window with the given dimensions.
   *
   * @param width the window width
   * @param height the window height
   */
  public Mechanism2d(double width, double height) {
    m_roots = new HashMap<>();
    m_dims[0] = width;
    m_dims[1] = height;
  }

  /**
   * Get or create a root in this Mechanism2d window with the given name and position.
   *
   * <p>If a root with the given name already exists, the given x and y coordinates are not used.
   *
   * @param name the root name
   * @param x the root x coordinate
   * @param y the root y coordinate
   * @return a new root joint object, or the existing one with the given name.
   */
  public synchronized MechanismRoot2d getRoot(String name, double x, double y) {
    var existing = m_roots.get(name);
    if (existing != null) {
      return existing;
    }

    var root = new MechanismRoot2d(name, x, y);
    m_roots.put(name, root);
    if (m_table != null) {
      root.update(m_table.getSubTable(name));
    }
    return root;
  }

  /**
   * Set the Mechanism2d window background color.
   *
   * @param color the new background color
   */
  public synchronized void setBackgroundColor(Color8Bit color) {
    this.m_color = String.format("#%02X%02X%02X", color.red, color.green, color.blue);
    if (m_table != null) {
      m_table.getEntry(kBackgroundColor).setString(m_color);
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Mechanism2d");
    m_table = builder.getTable();
    m_table.getEntry("dims").setDoubleArray(m_dims);
    m_table.getEntry(kBackgroundColor).setString(m_color);
    synchronized (this) {
      for (Entry<String, MechanismRoot2d> entry : m_roots.entrySet()) {
        String name = entry.getKey();
        MechanismRoot2d root = entry.getValue();
        synchronized (root) {
          root.update(m_table.getSubTable(name));
        }
      }
    }
  }
}
