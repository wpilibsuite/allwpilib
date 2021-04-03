package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;

import java.util.HashMap;
import java.util.Map;

public class Mechanism2d implements Sendable {
  private NetworkTable m_table;
  private final Map<String, MechanismLigament2d> m_ligaments;

  public Mechanism2d(double width, double length) {
    m_ligaments = new HashMap<>();

  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Mechanism2d");
    m_table = builder.getTable();

    synchronized (this) {
      for (MechanismLigament2d obj : m_ligaments.values()) {
        synchronized (obj) {
          obj.initTable(m_table.getSubTable(obj.m_name));
          subtable.getEntry()
        }
      }
    }
  }

}