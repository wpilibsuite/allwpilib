// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.util.Color;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

/**
 * 2D representation of arms, elevators, and general mechanisms.
 *
 * <p>Mechanisms consist of joints and ligament nodes. Obtain the root joint by calling {@link
 * #getRoot(String, double, double)} and add ligaments by traversing the tree with {@link
 * MechanismJoint2d#getLigament(String)} and {@link MechanismLigament2d#getEnd()}.
 *
 * <p>Example code for an elevator with a wrist:
 *
 * <pre><code>
 *   Mechanism2d mech = new Mechanism2d(100, 200);
 * MechanismLigament2d elev = mech.getRoot("elevator", 50, 0).getLigament("elev");
 * MechanismLigament2d wrist = elev.getEnd().getLigament("wrist");
 * elev.setAngle(90);
 * elev.setLength(70);
 * wrist.setAngle(-90);
 * wrist.setLength(20);
 * </code></pre>
 *
 * @see MechanismLigament2d
 * @see MechanismJoint2d
 */
public class Mechanism2d implements Sendable {
  private static final String kBackgroundColor = "backgroundColor";
  private NetworkTable m_table;
  private final Map<String, MechanismJoint2d> m_roots;
  private final double m_width;
  private final double m_height;
  private String m_color = "#"; // TODO: decide on default color

  /**
   * Create a new Mechanism2d window with the given dimensions.
   *
   * @param width the window width
   * @param height the window height
   */
  public Mechanism2d(double width, double height) {
    m_roots = new HashMap<>();
    m_width = width;
    m_height = height;
  }

  /**
   * Get or create a root in this Mechanism2d window with the given name and position.
   *
   * @param name the root name
   * @param x the root x coordinate
   * @param y the root y coordinate
   * @return a new root joint object, or the existing one with the given name.
   * @implNote If a root with the given name already exists, the given x and y coordinates are not
   *     used.
   */
  public MechanismJoint2d getRoot(String name, double x, double y) {
    var existing = m_roots.get(name);
    if (existing != null) return existing;

    var root = new MechanismJoint2d(name, x, y);
    m_roots.put(name, root);
    root.update(m_table.getSubTable(name));
    return root;
  }

  /**
   * Set the Mechanism2d window background color.
   *
   * @param color the new background color
   */
  public void setBackgroundColor(Color color) {
    this.m_color =
        String.format("#%02X%02X%02X", (int) color.red, (int) color.green, (int) color.blue);
    if (m_table != null) {
      m_table.getEntry(kBackgroundColor).setString(m_color);
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Mechanism2d");
    m_table = builder.getTable();
    m_table.getEntry("dims").setDoubleArray(new double[] {m_width, m_height});
    m_table.getEntry(kBackgroundColor).setString(m_color);
    synchronized (this) {
      for (Entry<String, MechanismJoint2d> entry : m_roots.entrySet()) {
        String name = entry.getKey();
        MechanismJoint2d root = entry.getValue();
        synchronized (root) {
          root.update(m_table.getSubTable(name));
        }
      }
    }
  }
}
