// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.DoubleArrayPublisher;
import edu.wpi.first.networktables.NTSendable;
import edu.wpi.first.networktables.NTSendableBuilder;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

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
public final class Mechanism2d implements NTSendable, AutoCloseable {
  private NetworkTable m_table;
  private final Map<String, MechanismRoot2d> m_roots;
  private final double[] m_dims = new double[2];
  private String m_color;
  private DoubleArrayPublisher m_dimsPub;
  private StringPublisher m_colorPub;

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
    if (m_dimsPub != null) {
      m_dimsPub.close();
    }
    if (m_colorPub != null) {
      m_colorPub.close();
    }
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
    if (m_table != null) {
      root.update(m_table.getSubTable(name));
    }
    return root;
  }

  /**
   * Set the Mechanism2d background color.
   *
   * @param color the new color
   */
  public synchronized void setBackgroundColor(Color8Bit color) {
    m_color = color.toHexString();
    if (m_colorPub != null) {
      m_colorPub.set(m_color);
    }
  }

  @Override
  public void initSendable(NTSendableBuilder builder) {
    builder.setSmartDashboardType("Mechanism2d");
    synchronized (this) {
      m_table = builder.getTable();
      if (m_dimsPub != null) {
        m_dimsPub.close();
      }
      m_dimsPub = m_table.getDoubleArrayTopic("dims").publish();
      m_dimsPub.set(m_dims);
      if (m_colorPub != null) {
        m_colorPub.close();
      }
      m_colorPub = m_table.getStringTopic("backgroundColor").publish();
      m_colorPub.set(m_color);
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
