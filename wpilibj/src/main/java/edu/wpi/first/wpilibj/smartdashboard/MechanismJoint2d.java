// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

/**
 * Joint on a Mechanism2d.
 *
 * <p>Use {@link #getLigament(String)} to create a ligament based on this joint.
 *
 * @see Mechanism2d
 */
public class MechanismJoint2d {
  /** NT path relative to base table. */
  private final String m_path;
  /** Keys are the NT paths relative to this joint. */
  private final Map<String, MechanismLigament2d> m_ligaments = new HashMap<>();

  private final double[] m_pos;
  private NetworkTable m_table;

  /**
   * Package-private constructor for root joints.
   *
   * @param path NT path, relative to base table.
   * @param x x coordinate of root (provide only when constructing a root node)
   * @param y y coordinate of root (provide only when constructing a root node)
   */
  MechanismJoint2d(String path, double x, double y) {
    m_path = path;
    this.m_pos = new double[] {x, y};
  }

  /**
   * Package-private constructor for end joints.
   *
   * @param path NT path, relative to base table.
   */
  MechanismJoint2d(String path) {
    m_path = path;
    m_pos = null;
  }

  /**
   * Get or create a ligament based on this joint.
   *
   * @param name the ligament name.
   * @return a new ligament object, or the existing one with the given name.
   */
  public MechanismLigament2d getLigament(String name) {
    var existing = m_ligaments.get(name);
    if (existing != null) {
      return existing;
    }

    var ligament = new MechanismLigament2d(m_path + NetworkTable.PATH_SEPARATOR + name);
    m_ligaments.put(name, ligament);
    if (m_table != null) {
      ligament.update(m_table.getSubTable(name));
    }
    return ligament;
  }

  /**
   * Get the NetworkTables path to this joint.
   *
   * @return the NT path, relative to the base entry.
   */
  public String getPath() {
    return m_path;
  }

  /**
   * Update all cached entries with new ones.
   *
   * @param table the new table.
   */
  void update(NetworkTable table) {
    this.m_table = table;
    if (m_pos != null) {
      m_table.getEntry("pos").setDoubleArray(m_pos);
    }
    for (Entry<String, MechanismLigament2d> entry : m_ligaments.entrySet()) {
      String name = entry.getKey();
      MechanismLigament2d line = entry.getValue();
      synchronized (line) {
        line.update(m_table.getSubTable(name));
      }
    }
  }
}
