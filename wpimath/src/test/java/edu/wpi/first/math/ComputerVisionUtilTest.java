// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class ComputerVisionUtilTest {
  @Test
  void testCalculateDistanceToTarget() {
    var camHeight = 1;
    var targetHeight = 3;
    var camPitch = Units.degreesToRadians(0);
    var targetPitch = Units.degreesToRadians(30);
    var targetYaw = Units.degreesToRadians(0);
    var dist =
        ComputerVisionUtil.calculateDistanceToTarget(
            camHeight, targetHeight, camPitch, targetPitch, targetYaw);

    Assertions.assertEquals(3.464, dist, 0.01);

    camHeight = 1;
    targetHeight = 2;
    camPitch = Units.degreesToRadians(20);
    targetPitch = Units.degreesToRadians(-10);

    dist =
        ComputerVisionUtil.calculateDistanceToTarget(
            camHeight, targetHeight, camPitch, targetPitch, targetYaw);
    Assertions.assertEquals(5.671, dist, 0.01);

    camHeight = 3;
    targetHeight = 1;
    camPitch = Units.degreesToRadians(0);
    targetPitch = Units.degreesToRadians(-30);

    dist =
        ComputerVisionUtil.calculateDistanceToTarget(
            camHeight, targetHeight, camPitch, targetPitch, targetYaw);

    Assertions.assertEquals(3.464, dist, 0.01);

    camHeight = 1;
    targetHeight = 3;
    camPitch = Units.degreesToRadians(0);
    targetPitch = Units.degreesToRadians(30);
    targetYaw = Units.degreesToRadians(30);

    dist =
        ComputerVisionUtil.calculateDistanceToTarget(
            camHeight, targetHeight, camPitch, targetPitch, targetYaw);

    Assertions.assertEquals(4, dist, 0.01);
  }

  @Test
  void testEstimateFieldToRobot() {
    var camHeight = 1;
    var targetHeight = 3;
    var camPitch = 0;
    var targetPitch = Units.degreesToRadians(30);
    var targetYaw = new Rotation2d();
    var gyroAngle = new Rotation2d();
    var fieldToTarget = new Pose2d();
    var cameraToRobot = new Transform2d();

    var fieldToRobot =
        ComputerVisionUtil.estimateFieldToRobot(
            ComputerVisionUtil.estimateCameraToTarget(
                new Translation2d(
                    ComputerVisionUtil.calculateDistanceToTarget(
                        camHeight, targetHeight, camPitch, targetPitch, targetYaw.getRadians()),
                    targetYaw),
                fieldToTarget,
                gyroAngle),
            fieldToTarget,
            cameraToRobot);

    Assertions.assertEquals(-3.464, fieldToRobot.getX(), 0.1);
    Assertions.assertEquals(0, fieldToRobot.getY(), 0.1);
    Assertions.assertEquals(0, fieldToRobot.getRotation().getDegrees(), 0.1);

    gyroAngle = Rotation2d.fromDegrees(-30);

    fieldToRobot =
        ComputerVisionUtil.estimateFieldToRobot(
            ComputerVisionUtil.estimateCameraToTarget(
                new Translation2d(
                    ComputerVisionUtil.calculateDistanceToTarget(
                        camHeight, targetHeight, camPitch, targetPitch, targetYaw.getRadians()),
                    targetYaw),
                fieldToTarget,
                gyroAngle),
            fieldToTarget,
            cameraToRobot);

    Assertions.assertEquals(-3.0, fieldToRobot.getX(), 0.1);
    Assertions.assertEquals(1.732, fieldToRobot.getY(), 0.1);
    Assertions.assertEquals(-30.0, fieldToRobot.getRotation().getDegrees(), 0.1);
  }
}
