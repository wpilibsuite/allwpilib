// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.util.Color8Bit;

/**
 * Ligament node on a Mechanism2d. A ligament can have its length changed (like an elevator) or
 * angle changed, like an arm.
 *
 * @see Mechanism2d
 */
public class MechanismLigament2d extends MechanismObject2d {
  private double m_angle;
  private NetworkTableEntry m_angleEntry;
  private String m_color;
  private NetworkTableEntry m_colorEntry;
  private double m_length;
  private NetworkTableEntry m_lengthEntry;
  private double m_weight;
  private NetworkTableEntry m_weightEntry;

  /**
   * Create a new ligament.
   *
   * @param name The ligament name.
   * @param length The ligament length.
   * @param angle The ligament angle in degrees.
   * @param lineWidth The ligament's line width.
   * @param color The ligament's color.
   */
  public MechanismLigament2d(
      String name, double length, double angle, double lineWidth, Color8Bit color) {
    super(name);
    setColor(color);
    setLength(length);
    setAngle(angle);
    setLineWeight(lineWidth);
  }

  /**
   * Create a new ligament with the default color (orange) and thickness (6).
   *
   * @param name The ligament's name.
   * @param length The ligament's length.
   * @param angle The ligament's angle relative to its parent in degrees.
   */
  public MechanismLigament2d(String name, double length, double angle) {
    this(name, length, angle, 10, new Color8Bit(235, 137, 52));
  }

  /**
   * Set the ligament's angle relative to its parent.
   *
   * @param degrees the angle in degrees
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
   * @return the angle in degrees
   */
  public synchronized double getAngle() {
    if (m_angleEntry != null) {
      m_angle = m_angleEntry.getDouble(0.0);
    }
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
    if (m_lengthEntry != null) {
      m_length = m_lengthEntry.getDouble(0.0);
    }
    return m_length;
  }

  /**
   * Set the ligament color.
   *
   * @param color the color of the line
   */
  public synchronized void setColor(Color8Bit color) {
    m_color = String.format("#%02X%02X%02X", color.red, color.green, color.blue);
    flush();
  }

  /**
   * Get the ligament color.
   *
   * @return the color of the line
   */
  public synchronized Color8Bit getColor() {
    if (m_colorEntry != null) {
      m_color = m_colorEntry.getString("");
    }
    int r = 0;
    int g = 0;
    int b = 0;
    if (m_color.length() == 7 && m_color.charAt(0) == '#') {
      try {
        r = Integer.parseInt(m_color.substring(1, 3), 16);
        g = Integer.parseInt(m_color.substring(3, 5), 16);
        b = Integer.parseInt(m_color.substring(5, 7), 16);
      } catch (NumberFormatException e) {
        r = 0;
        g = 0;
        b = 0;
      }
    }
    return new Color8Bit(r, g, b);
  }

  /**
   * Set the line thickness.
   *
   * @param weight the line thickness
   */
  public synchronized void setLineWeight(double weight) {
    m_weight = weight;
    flush();
  }

  /**
   * Get the line thickness.
   *
   * @return the line thickness
   */
  public synchronized double getLineWeight() {
    if (m_weightEntry != null) {
      m_weight = m_weightEntry.getDouble(0.0);
    }
    return m_weight;
  }

  @Override
  protected void updateEntries(NetworkTable table) {
    table.getEntry(".type").setString("line");
    m_angleEntry = table.getEntry("angle");
    m_lengthEntry = table.getEntry("length");
    m_colorEntry = table.getEntry("color");
    m_weightEntry = table.getEntry("weight");
    flush();
  }

  /** Flush latest data to NT. */
  private void flush() {
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
}
