// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class CoordinateSystemTest {
  private void checkPose3dConvert(
      Pose3d poseFrom, Pose3d poseTo, CoordinateSystem coordFrom, CoordinateSystem coordTo) {
    // "from" to "to"
    assertEquals(
        poseTo.getTranslation(),
        CoordinateSystem.convert(poseFrom.getTranslation(), coordFrom, coordTo));
    assertEquals(
        poseTo.getRotation(), CoordinateSystem.convert(poseFrom.getRotation(), coordFrom, coordTo));
    assertEquals(poseTo, CoordinateSystem.convert(poseFrom, coordFrom, coordTo));

    // "to" to "from"
    assertEquals(
        poseFrom.getTranslation(),
        CoordinateSystem.convert(poseTo.getTranslation(), coordTo, coordFrom));
    assertEquals(
        poseFrom.getRotation(), CoordinateSystem.convert(poseTo.getRotation(), coordTo, coordFrom));
    assertEquals(poseFrom, CoordinateSystem.convert(poseTo, coordTo, coordFrom));
  }

  private void checkTransform3dConvert(
      Transform3d transformFrom,
      Transform3d transformTo,
      CoordinateSystem coordFrom,
      CoordinateSystem coordTo) {
    // "from" to "to"
    assertEquals(
        transformTo.getTranslation(),
        CoordinateSystem.convert(transformFrom.getTranslation(), coordFrom, coordTo));
    assertEquals(transformTo, CoordinateSystem.convert(transformFrom, coordFrom, coordTo));

    // "to" to "from"
    assertEquals(
        transformFrom.getTranslation(),
        CoordinateSystem.convert(transformTo.getTranslation(), coordTo, coordFrom));
    assertEquals(transformFrom, CoordinateSystem.convert(transformTo, coordTo, coordFrom));
  }

  @Test
  void testPose3dEDNtoNWU() {
    // No rotation from EDN to NWU
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, Rotation3d.kZero),
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(Units.degreesToRadians(-90.0), 0.0, Units.degreesToRadians(-90.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());

    // 45° roll from EDN to NWU
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0)),
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(Units.degreesToRadians(-45.0), 0.0, Units.degreesToRadians(-90.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());

    // 45° pitch from EDN to NWU
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, Units.degreesToRadians(45.0), 0.0)),
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(Units.degreesToRadians(-90.0), 0.0, Units.degreesToRadians(-135.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());

    // 45° yaw from EDN to NWU
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, 0.0, Units.degreesToRadians(45.0))),
        new Pose3d(
            3.0,
            -1.0,
            -2.0,
            new Rotation3d(
                Units.degreesToRadians(-90.0),
                Units.degreesToRadians(45.0),
                Units.degreesToRadians(-90.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());
  }

  @Test
  void testPose3dEDNtoNED() {
    // No rotation from EDN to NED
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, Rotation3d.kZero),
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(Units.degreesToRadians(90.0), 0.0, Units.degreesToRadians(90.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());

    // 45° roll from EDN to NED
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0)),
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(Units.degreesToRadians(135.0), 0.0, Units.degreesToRadians(90.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());

    // 45° pitch from EDN to NED
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, Units.degreesToRadians(45.0), 0.0)),
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(Units.degreesToRadians(90.0), 0.0, Units.degreesToRadians(135.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());

    // 45° yaw from EDN to NED
    checkPose3dConvert(
        new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.0, 0.0, Units.degreesToRadians(45.0))),
        new Pose3d(
            3.0,
            1.0,
            2.0,
            new Rotation3d(
                Units.degreesToRadians(90.0),
                Units.degreesToRadians(-45.0),
                Units.degreesToRadians(90.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());
  }

  @Test
  void testTransform3dEDNtoNWU() {
    // No rotation from EDN to NWU
    checkTransform3dConvert(
        new Transform3d(new Translation3d(1.0, 2.0, 3.0), Rotation3d.kZero),
        new Transform3d(new Translation3d(3.0, -1.0, -2.0), Rotation3d.kZero),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());

    // 45° roll from EDN to NWU
    checkTransform3dConvert(
        new Transform3d(
            new Translation3d(1.0, 2.0, 3.0),
            new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0)),
        new Transform3d(
            new Translation3d(3.0, -1.0, -2.0),
            new Rotation3d(0.0, Units.degreesToRadians(-45.0), 0.0)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());

    // 45° pitch from EDN to NWU
    checkTransform3dConvert(
        new Transform3d(
            new Translation3d(1.0, 2.0, 3.0),
            new Rotation3d(0.0, Units.degreesToRadians(45.0), 0.0)),
        new Transform3d(
            new Translation3d(3.0, -1.0, -2.0),
            new Rotation3d(0.0, 0.0, Units.degreesToRadians(-45.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());

    // 45° yaw from EDN to NWU
    checkTransform3dConvert(
        new Transform3d(
            new Translation3d(1.0, 2.0, 3.0),
            new Rotation3d(0.0, 0.0, Units.degreesToRadians(45.0))),
        new Transform3d(
            new Translation3d(3.0, -1.0, -2.0),
            new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU());
  }

  @Test
  void testTransform3dEDNtoNED() {
    // No rotation from EDN to NED
    checkTransform3dConvert(
        new Transform3d(new Translation3d(1.0, 2.0, 3.0), Rotation3d.kZero),
        new Transform3d(new Translation3d(3.0, 1.0, 2.0), Rotation3d.kZero),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());

    // 45° roll from EDN to NED
    checkTransform3dConvert(
        new Transform3d(
            new Translation3d(1.0, 2.0, 3.0),
            new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0)),
        new Transform3d(
            new Translation3d(3.0, 1.0, 2.0),
            new Rotation3d(0.0, Units.degreesToRadians(45.0), 0.0)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());

    // 45° pitch from EDN to NED
    checkTransform3dConvert(
        new Transform3d(
            new Translation3d(1.0, 2.0, 3.0),
            new Rotation3d(0.0, Units.degreesToRadians(45.0), 0.0)),
        new Transform3d(
            new Translation3d(3.0, 1.0, 2.0),
            new Rotation3d(0.0, 0.0, Units.degreesToRadians(45.0))),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());

    // 45° yaw from EDN to NED
    checkTransform3dConvert(
        new Transform3d(
            new Translation3d(1.0, 2.0, 3.0),
            new Rotation3d(0.0, 0.0, Units.degreesToRadians(45.0))),
        new Transform3d(
            new Translation3d(3.0, 1.0, 2.0),
            new Rotation3d(Units.degreesToRadians(45.0), 0.0, 0.0)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED());
  }
}
