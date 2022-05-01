// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class CoordinateSystemTest {
  @Test
  void testEDNtoNWU() {
    var edn1 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d());
    var nwu1 =
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(-Units.degreesToRadians(90.0), 0.0, -Units.degreesToRadians(90.0)));

    assertEquals(
        nwu1, CoordinateSystem.convert(edn1, CoordinateSystem.EDN(), CoordinateSystem.NWU()));
    assertEquals(
        edn1, CoordinateSystem.convert(nwu1, CoordinateSystem.NWU(), CoordinateSystem.EDN()));

    var edn2 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0));
    var nwu2 =
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(-Units.degreesToRadians(45.0), 0.0, -Units.degreesToRadians(90.0)));

    assertEquals(
        nwu2, CoordinateSystem.convert(edn2, CoordinateSystem.EDN(), CoordinateSystem.NWU()));
    assertEquals(
        edn2, CoordinateSystem.convert(nwu2, CoordinateSystem.NWU(), CoordinateSystem.EDN()));

    var edn3 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, Units.degreesToRadians(45.0), 0.0));
    var nwu3 =
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(-Units.degreesToRadians(90.0), 0.0, -Units.degreesToRadians(135.0)));

    assertEquals(
        nwu3, CoordinateSystem.convert(edn3, CoordinateSystem.EDN(), CoordinateSystem.NWU()));
    assertEquals(
        edn3, CoordinateSystem.convert(nwu3, CoordinateSystem.NWU(), CoordinateSystem.EDN()));

    var edn4 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, 0.0, Units.degreesToRadians(45.0)));
    var nwu4 =
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(
                -Units.degreesToRadians(90.0),
                Units.degreesToRadians(45.0),
                -Units.degreesToRadians(90.0)));

    assertEquals(
        nwu4, CoordinateSystem.convert(edn4, CoordinateSystem.EDN(), CoordinateSystem.NWU()));
    assertEquals(
        edn4, CoordinateSystem.convert(nwu4, CoordinateSystem.NWU(), CoordinateSystem.EDN()));
  }

  @Test
  void testEDNtoNED() {
    var edn1 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d());
    var ned1 =
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(Units.degreesToRadians(90.0), 0.0, Units.degreesToRadians(90.0)));

    assertEquals(
        ned1, CoordinateSystem.convert(edn1, CoordinateSystem.EDN(), CoordinateSystem.NED()));
    assertEquals(
        edn1, CoordinateSystem.convert(ned1, CoordinateSystem.NED(), CoordinateSystem.EDN()));

    var edn2 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0));
    var ned2 =
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(Units.degreesToRadians(135.0), 0.0, Units.degreesToRadians(90.0)));

    assertEquals(
        ned2, CoordinateSystem.convert(edn2, CoordinateSystem.EDN(), CoordinateSystem.NED()));
    assertEquals(
        edn2, CoordinateSystem.convert(ned2, CoordinateSystem.NED(), CoordinateSystem.EDN()));

    var edn3 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, Units.degreesToRadians(45.0), 0.0));
    var ned3 =
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(Units.degreesToRadians(90.0), 0.0, Units.degreesToRadians(135.0)));

    assertEquals(
        ned3, CoordinateSystem.convert(edn3, CoordinateSystem.EDN(), CoordinateSystem.NED()));
    assertEquals(
        edn3, CoordinateSystem.convert(ned3, CoordinateSystem.NED(), CoordinateSystem.EDN()));

    var edn4 = new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, 0.0, Units.degreesToRadians(45.0)));
    var ned4 =
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(
                Units.degreesToRadians(90.0),
                -Units.degreesToRadians(45.0),
                Units.degreesToRadians(90.0)));

    assertEquals(
        ned4, CoordinateSystem.convert(edn4, CoordinateSystem.EDN(), CoordinateSystem.NED()));
    assertEquals(
        edn4, CoordinateSystem.convert(ned4, CoordinateSystem.NED(), CoordinateSystem.EDN()));
  }
}
