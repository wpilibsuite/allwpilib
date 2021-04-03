package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.geometry.Rotation2d;

public class MechanismLigament2d {
  private String m_name;
  private NetworkTableEntry m_angle;
  private NetworkTableEntry m_color;
  private NetworkTableEntry m_length;
  private NetworkTableEntry m_weight;

  /**
   * Package-private constructor.
   */
  MechanismLigament2d(String name) {
    m_name = name;
  }

  private void updateTableEntries(NetworkTable table) {
    table.getEntry(".type").setString("line");
    m_angle = table.getEntry("angle");
    m_color = table.getEntry("color");
    m_length = table.getEntry("length");
    m_weight = table.getEntry("weight");
  }

  public void setAngle(double degrees) {
    m_angle.setDouble(degrees); // TODO: make "unitialized-safe"
  }

  public void setAngle(Rotation2d angle) {
    setAngle(angle.getDegrees());
  }


}
