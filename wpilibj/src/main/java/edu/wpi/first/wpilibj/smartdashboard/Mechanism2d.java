package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.util.Color;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

public class Mechanism2d implements Sendable {
  private static final String BACKGROUND_COLOR = "backgroundColor";
  private NetworkTable m_table;
  private final Map<String, MechanismJoint2d> m_roots;
  private final double m_width;
  private final double m_height;
  private String m_color = "#";

  public Mechanism2d(double width, double height) {
    m_roots = new HashMap<>();
    m_width = width;
    m_height = height;
  }

  public void setBackgroundColor(Color color) {
    this.m_color =
        String.format("#%02X%02X%02X", (int) color.red, (int) color.green, (int) color.blue);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Mechanism2d");
    m_table = builder.getTable();
    m_table.getEntry("dims").setDoubleArray(new double[] {m_width, m_height});
    m_table.getEntry(BACKGROUND_COLOR).setString(m_color);
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

  private synchronized void flush() {
    if (this.m_table != null) {}
  }

  public MechanismJoint2d getRoot(String name, double x, double y) {
    var root = new MechanismJoint2d(name, x, y);
    m_roots.put(name, root);
    return root;
  }
}
