// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.numbers.N3;

/** A class representing a coordinate system axis within the NWU coordinate system. */
public class CoordinateAxis {
  private static final CoordinateAxis m_n = new CoordinateAxis(1.0, 0.0, 0.0);
  private static final CoordinateAxis m_s = new CoordinateAxis(-1.0, 0.0, 0.0);
  private static final CoordinateAxis m_e = new CoordinateAxis(0.0, -1.0, 0.0);
  private static final CoordinateAxis m_w = new CoordinateAxis(0.0, 1.0, 0.0);
  private static final CoordinateAxis m_u = new CoordinateAxis(0.0, 0.0, 1.0);
  private static final CoordinateAxis m_d = new CoordinateAxis(0.0, 0.0, -1.0);

  final Vector<N3> m_axis;

  /**
   * Constructs a coordinate system axis within the NWU coordinate system and normalizes it.
   *
   * @param x The x component.
   * @param y The y component.
   * @param z The z component.
   */
  public CoordinateAxis(double x, double y, double z) {
    double norm = Math.sqrt(x * x + y * y + z * z);
    m_axis = VecBuilder.fill(x / norm, y / norm, z / norm);
  }

  /**
   * Returns a coordinate axis corresponding to +X in the NWU coordinate system.
   *
   * @return A coordinate axis corresponding to +X in the NWU coordinate system.
   */
  public static CoordinateAxis N() {
    return m_n;
  }

  /**
   * Returns a coordinate axis corresponding to -X in the NWU coordinate system.
   *
   * @return A coordinate axis corresponding to -X in the NWU coordinate system.
   */
  public static CoordinateAxis S() {
    return m_s;
  }

  /**
   * Returns a coordinate axis corresponding to -Y in the NWU coordinate system.
   *
   * @return A coordinate axis corresponding to -Y in the NWU coordinate system.
   */
  public static CoordinateAxis E() {
    return m_e;
  }

  /**
   * Returns a coordinate axis corresponding to +Y in the NWU coordinate system.
   *
   * @return A coordinate axis corresponding to +Y in the NWU coordinate system.
   */
  public static CoordinateAxis W() {
    return m_w;
  }

  /**
   * Returns a coordinate axis corresponding to +Z in the NWU coordinate system.
   *
   * @return A coordinate axis corresponding to +Z in the NWU coordinate system.
   */
  public static CoordinateAxis U() {
    return m_u;
  }

  /**
   * Returns a coordinate axis corresponding to -Z in the NWU coordinate system.
   *
   * @return A coordinate axis corresponding to -Z in the NWU coordinate system.
   */
  public static CoordinateAxis D() {
    return m_d;
  }
}
