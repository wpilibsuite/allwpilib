// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class ComputerVisionUtilTest {
  @Test
  void testCalculateDistanceToTarget() {
    var cameraHeight = 1;
    var targetHeight = 3;
    var cameraPitch = Units.degreesToRadians(0);
    var targetPitch = Units.degreesToRadians(30);
    var targetYaw = Units.degreesToRadians(0);

    var distanceAlongGround =
        ComputerVisionUtil.calculateDistanceToTarget(
            cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
    Assertions.assertEquals(3.464, distanceAlongGround, 0.01);

    cameraHeight = 1;
    targetHeight = 2;
    cameraPitch = Units.degreesToRadians(20);
    targetPitch = Units.degreesToRadians(-10);

    distanceAlongGround =
        ComputerVisionUtil.calculateDistanceToTarget(
            cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
    Assertions.assertEquals(5.671, distanceAlongGround, 0.01);

    cameraHeight = 3;
    targetHeight = 1;
    cameraPitch = Units.degreesToRadians(0);
    targetPitch = Units.degreesToRadians(-30);

    distanceAlongGround =
        ComputerVisionUtil.calculateDistanceToTarget(
            cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
    Assertions.assertEquals(3.464, distanceAlongGround, 0.01);

    cameraHeight = 1;
    targetHeight = 3;
    cameraPitch = Units.degreesToRadians(0);
    targetPitch = Units.degreesToRadians(30);
    targetYaw = Units.degreesToRadians(30);

    distanceAlongGround =
        ComputerVisionUtil.calculateDistanceToTarget(
            cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
    Assertions.assertEquals(4, distanceAlongGround, 0.01);
  }

  @Test
  void testEstimateFieldToRobot() {
    var cameraHeight = 1;
    var targetHeight = 3;
    var cameraPitch = 0;
    var targetPitch = Units.degreesToRadians(30);
    var targetYaw = new Rotation2d();
    var gyroAngle = new Rotation2d();
    var fieldToTarget = new Pose3d();
    var cameraToRobot = new Transform3d();

    var distanceAlongGround =
        ComputerVisionUtil.calculateDistanceToTarget(
            cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw.getRadians());
    var range = Math.hypot(distanceAlongGround, targetHeight - cameraHeight);
    var fieldToRobot =
        ComputerVisionUtil.estimateFieldToRobot(
            ComputerVisionUtil.estimateCameraToTarget(
                new Translation3d(range, new Rotation3d(0.0, targetPitch, targetYaw.getRadians())),
                fieldToTarget,
                gyroAngle),
            fieldToTarget,
            cameraToRobot);

    Assertions.assertEquals(-3.464, fieldToRobot.getX(), 0.1);
    Assertions.assertEquals(0, fieldToRobot.getY(), 0.1);
    Assertions.assertEquals(2.0, fieldToRobot.getZ(), 0.1);
    Assertions.assertEquals(0, Units.radiansToDegrees(fieldToRobot.getRotation().getZ()), 0.1);

    gyroAngle = Rotation2d.fromDegrees(-30);

    distanceAlongGround =
        ComputerVisionUtil.calculateDistanceToTarget(
            cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw.getRadians());
    range = Math.hypot(distanceAlongGround, targetHeight - cameraHeight);
    fieldToRobot =
        ComputerVisionUtil.estimateFieldToRobot(
            ComputerVisionUtil.estimateCameraToTarget(
                new Translation3d(range, new Rotation3d(0.0, targetPitch, targetYaw.getRadians())),
                fieldToTarget,
                gyroAngle),
            fieldToTarget,
            cameraToRobot);

    Assertions.assertEquals(-3.0, fieldToRobot.getX(), 0.1);
    Assertions.assertEquals(1.732, fieldToRobot.getY(), 0.1);
    Assertions.assertEquals(2.0, fieldToRobot.getZ(), 0.1);
    Assertions.assertEquals(-30.0, Units.radiansToDegrees(fieldToRobot.getRotation().getZ()), 0.1);
  }
}
