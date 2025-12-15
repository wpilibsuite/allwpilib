// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.util.Color8Bit;

/**
 * Visual 2D representation of arms, elevators, and general mechanisms through a node-based API.
 *
 * <p>A Mechanism2d object is published and contains at least one root node. A root is the anchor
 * point of other nodes (such as ligaments). Other nodes are recursively appended based on other
 * nodes.
 *
 * @see MechanismObject2d
 * @see MechanismLigament2d
 * @see MechanismRoot2d
 */
public final class Mechanism2d implements TelemetryLoggable, AutoCloseable {
  private final Map<String, MechanismRoot2d> m_roots;
  private final double[] m_dims = new double[2];
  private String m_color;

  /**
   * Create a new Mechanism2d with the given dimensions and default color (dark blue).
   *
   * <p>The dimensions represent the canvas that all the nodes are drawn on.
   *
   * @param width the width
   * @param height the height
   */
  public Mechanism2d(double width, double height) {
    this(width, height, new Color8Bit(0, 0, 32));
  }

  /**
   * Create a new Mechanism2d with the given dimensions.
   *
   * <p>The dimensions represent the canvas that all the nodes are drawn on.
   *
   * @param width the width
   * @param height the height
   * @param backgroundColor the background color. Defaults to dark blue.
   */
  public Mechanism2d(double width, double height, Color8Bit backgroundColor) {
    m_roots = new HashMap<>();
    m_dims[0] = width;
    m_dims[1] = height;
    setBackgroundColor(backgroundColor);
  }

  @Override
  public void close() {
    for (MechanismRoot2d root : m_roots.values()) {
      root.close();
    }
  }

  /**
   * Get or create a root in this Mechanism2d with the given name and position.
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
    return root;
  }

  /**
   * Set the Mechanism2d background color.
   *
   * @param color the new color
   */
  public synchronized void setBackgroundColor(Color8Bit color) {
    m_color = color.toHexString();
  }

  @Override
  public void logTo(TelemetryTable table) {
    synchronized (this) {
      table.log("dims", m_dims);
      table.log("backgroundColor", m_color);
      for (Entry<String, MechanismRoot2d> entry : m_roots.entrySet()) {
        table.log(entry.getKey(), entry.getValue());
      }
    }
  }

  @Override
  public String getTelemetryType() {
    return "Mechanism2d";
  }
}
