package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.util.Color;

public class MechanismLigament2d {
  private final String m_name;
  private double m_angle;
  private NetworkTableEntry m_angleEntry;
  private String m_color;
  private NetworkTableEntry m_colorEntry;
  private double m_length;
  private NetworkTableEntry m_lengthEntry;
  private double m_weight;
  private NetworkTableEntry m_weightEntry;
  private MechanismJoint2d m_end;

  /** Package-private constructor. */
  MechanismLigament2d(String name) {
    m_name = name;
  }

  void update(NetworkTable table) {
    table.getEntry(".type").setString("line");
    m_angleEntry = table.getEntry("angle");
    m_lengthEntry = table.getEntry("length");
    m_colorEntry = table.getEntry("color");
    m_weightEntry = table.getEntry("weight");
    flush();
    if (m_end != null) {
      m_end.update(table);
    }
  }

  private synchronized void flush() {
    if (m_angleEntry != null) {
      m_angleEntry.setDouble(m_angle);
    }
    if (m_lengthEntry != null) {
      m_lengthEntry.setDouble(m_length);
    }
    if (m_colorEntry != null) {
      m_colorEntry.setString(m_color);
    }
    if (m_weightEntry != null) {
      m_weightEntry.setDouble(m_weight);
    }
  }

  /**
   * Set the ligament's angle relative to its parent.
   *
   * @param degrees the angle, in degrees
   */
  public synchronized void setAngle(double degrees) {
    m_angle = degrees;
    flush();
  }

  /**
   * Set the ligament's angle relative to its parent.
   *
   * @param angle the angle
   */
  public synchronized void setAngle(Rotation2d angle) {
    setAngle(angle.getDegrees());
  }

  /**
   * Get the ligament's angle relative to its parent.
   *
   * @return the angle, in degrees
   */
  public synchronized double getAngle() {
    return m_angle;
  }

  /**
   * Set the ligament's length.
   *
   * @param length the line length
   */
  public synchronized void setLength(double length) {
    m_length = length;
    flush();
  }

  /**
   * Get the ligament length.
   *
   * @return the line length
   */
  public synchronized double getLength() {
    return m_length;
  }

  /**
   * Set the ligament color.
   *
   * @param color the color of the line
   */
  public synchronized void setColor(Color color) {
    m_color = String.format("#%02X%02X%02X", (int) color.red, (int) color.green, (int) color.blue);
    flush();
  }

  /**
   * Set the line thickness.
   *
   * @param weight the line thickness
   */
  public synchronized void setWeight(double weight) {
    m_weight = weight;
    flush();
  }

  /**
   * Get the joint at the end of this ligament.
   *
   * @return the MechanismJoint2d object representing the end of this ligament
   */
  public MechanismJoint2d getEnd() {
    if (m_end == null) {
      m_end = new MechanismJoint2d(m_name, -1, -1);
    }
    return m_end;
  }
}
