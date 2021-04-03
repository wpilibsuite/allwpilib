package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

public class MechanismJoint2d {
  private final Map<String, MechanismLigament2d> m_ligaments;
  private final String m_name;
  private final double[] m_pos;
  private NetworkTable m_table;

  /** Package-private constructor. */
  MechanismJoint2d(String name, double x, double y) {
    m_name = name;
    if (x == -1 && y == -1) {
      m_pos = null;
    } else {
      this.m_pos = new double[]{x, y};
    }
    m_ligaments = new HashMap<>();
  }

  public MechanismLigament2d getLigament(String name) {
    return new MechanismLigament2d(name);
  }

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
