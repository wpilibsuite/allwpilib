// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.util.Color8Bit;

/**
 * Ligament node on a Mechanism2d. A ligament can have its length changed (like an elevator) or
 * angle changed, like an arm.
 *
 * @see Mechanism2d
 */
public class MechanismLigament2d extends MechanismObject2d {
  private Rotation2d m_angle;
  private String m_color;
  private double m_length;
  private double m_weight;

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
  public final synchronized void setAngle(double degrees) {
    m_angle = Rotation2d.fromDegrees(degrees);
  }

  /**
   * Set the ligament's angle relative to its parent.
   *
   * @param angle the angle
   */
  public synchronized void setAngle(Rotation2d angle) {
    m_angle = angle;
  }

  /**
   * Get the ligament's angle relative to its parent.
   *
   * @return the angle in degrees
   */
  public synchronized Rotation2d getAngle() {
    return m_angle;
  }

  /**
   * Set the ligament's length.
   *
   * @param length the line length
   */
  public final synchronized void setLength(double length) {
    m_length = length;
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
  public final synchronized void setColor(Color8Bit color) {
    m_color = String.format("#%02X%02X%02X", color.red, color.green, color.blue);
  }

  /**
   * Get the ligament color.
   *
   * @return the color of the line
   */
  public synchronized Color8Bit getColor() {
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
  public final synchronized void setLineWeight(double weight) {
    m_weight = weight;
  }

  /**
   * Get the line thickness.
   *
   * @return the line thickness
   */
  public synchronized double getLineWeight() {
    return m_weight;
  }

  @Override
  public void updateTelemetry(TelemetryTable table) {
    table.log("angle", m_angle);
    table.log("length", m_length);
    table.log("color", m_color);
    table.log("weight", m_weight);
    super.updateTelemetry(table);
  }

  @Override
  public String getTelemetryType() {
    return "line";
  }
}
