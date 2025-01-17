// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class Pose3dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testRotateBy() {
    final double x = 1.0;
    final double y = 2.0;
    var initial =
        new Pose3d(
            new Translation3d(x, y, 0.0),
            new Rotation3d(
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(45.0)));

    double yaw = Units.degreesToRadians(5.0);
    var rotation = new Rotation3d(Units.degreesToRadians(0.0), Units.degreesToRadians(0.0), yaw);
    var rotated = initial.rotateBy(rotation);

    // Translation is rotated by CCW rotation matrix
    double c = Math.cos(yaw);
    double s = Math.sin(yaw);
    assertAll(
        () -> assertEquals(c * x - s * y, rotated.getX(), kEpsilon),
        () -> assertEquals(s * x + c * y, rotated.getY(), kEpsilon),
        () -> assertEquals(0.0, rotated.getZ(), kEpsilon),
        () -> assertEquals(0.0, rotated.getRotation().getX(), kEpsilon),
        () -> assertEquals(0.0, rotated.getRotation().getY(), kEpsilon),
        () ->
            assertEquals(
                initial.getRotation().getZ() + rotation.getZ(),
                rotated.getRotation().getZ(),
                kEpsilon));
  }

  @Test
  void testTransformByRotations() {
    var initialPose = Pose3d.kZero;

    var transform1 =
        new Transform3d(
            Translation3d.kZero,
            new Rotation3d(
                Units.degreesToRadians(90.0),
                Units.degreesToRadians(45.0),
                Units.degreesToRadians(0.0)));

    var transform2 =
        new Transform3d(
            Translation3d.kZero,
            new Rotation3d(
                Units.degreesToRadians(-90.0),
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0)));

    var transform3 =
        new Transform3d(
            Translation3d.kZero,
            new Rotation3d(
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(-45.0),
                Units.degreesToRadians(0.0)));

    // This sequence of rotations should diverge from the origin and eventually
    // return to it. When
    // each rotation occurs, it should be performed intrinsicly, i.e. 'from the
    // viewpoint' of and
    // with
    // the axes of the pose that is being transformed, just like how the translation
    // is done 'from
    // the
    // viewpoint' of the pose that is being transformed.
    var finalPose =
        initialPose.transformBy(transform1).transformBy(transform2).transformBy(transform3);

    assertAll(
        () ->
            assertEquals(
                finalPose.getRotation().getX(), initialPose.getRotation().getX(), kEpsilon),
        () ->
            assertEquals(
                finalPose.getRotation().getY(), initialPose.getRotation().getY(), kEpsilon),
        () ->
            assertEquals(
                finalPose.getRotation().getZ(), initialPose.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testTransformBy() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial =
        new Pose3d(
            new Translation3d(1.0, 2.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var transformation =
        new Transform3d(
            new Translation3d(5.0, 0.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(5.0)));

    var transformed = initial.plus(transformation);

    assertAll(
        () -> assertEquals(1.0 + 5.0 / Math.sqrt(2.0), transformed.getX(), kEpsilon),
        () -> assertEquals(2.0 + 5.0 / Math.sqrt(2.0), transformed.getY(), kEpsilon),
        () ->
            assertEquals(Units.degreesToRadians(50.0), transformed.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testRelativeTo() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial = new Pose3d(0.0, 0.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var last = new Pose3d(5.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));

    var finalRelativeToInitial = last.relativeTo(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), finalRelativeToInitial.getX(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getY(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testRotateAround() {
    var initial = new Pose3d(new Translation3d(5, 0, 0), Rotation3d.kZero);
    var point = Translation3d.kZero;

    var rotated = initial.rotateAround(point, new Rotation3d(0, 0, Math.PI));

    assertAll(
        () -> assertEquals(-5.0, rotated.getX(), kEpsilon),
        () -> assertEquals(0.0, rotated.getY(), kEpsilon),
        () -> assertEquals(Math.PI, rotated.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testEquality() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var one = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    var two = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var one = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    var two = new Pose3d(0.0, 1.524, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    assertNotEquals(one, two);
  }

  @Test
  void testMinus() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial = new Pose3d(0.0, 0.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var last = new Pose3d(5.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));

    final var transform = last.minus(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), transform.getX(), kEpsilon),
        () -> assertEquals(0.0, transform.getY(), kEpsilon),
        () -> assertEquals(0.0, transform.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testToMatrix() {
    var before =
        new Pose3d(
            1.0,
            2.0,
            3.0,
            new Rotation3d(
                Units.degreesToRadians(20.0),
                Units.degreesToRadians(30.0),
                Units.degreesToRadians(40.0)));
    var after = new Pose3d(before.toMatrix());

    assertEquals(before, after);
  }

  @Test
  void testToPose2d() {
    var pose =
        new Pose3d(
            1.0,
            2.0,
            3.0,
            new Rotation3d(
                Units.degreesToRadians(20.0),
                Units.degreesToRadians(30.0),
                Units.degreesToRadians(40.0)));
    var expected = new Pose2d(1.0, 2.0, new Rotation2d(Units.degreesToRadians(40.0)));

    assertEquals(expected, pose.toPose2d());
  }

  @Test
  void testComplexTwists() {
    var initial_poses =
        List.of(
            new Pose3d(
                new Translation3d(0.698303, -0.959096, 0.271076),
                new Rotation3d(new Quaternion(0.86403, -0.076866, 0.147234, 0.475254))),
            new Pose3d(
                new Translation3d(0.634892, -0.765209, 0.117543),
                new Rotation3d(new Quaternion(0.84987, -0.070829, 0.162097, 0.496415))),
            new Pose3d(
                new Translation3d(0.584827, -0.590303, -0.02557),
                new Rotation3d(new Quaternion(0.832743, -0.041991, 0.202188, 0.513708))),
            new Pose3d(
                new Translation3d(0.505038, -0.451479, -0.112835),
                new Rotation3d(new Quaternion(0.816515, -0.002673, 0.226182, 0.531166))),
            new Pose3d(
                new Translation3d(0.428178, -0.329692, -0.189707),
                new Rotation3d(new Quaternion(0.807886, 0.029298, 0.257788, 0.529157))));

    var final_poses =
        List.of(
            new Pose3d(
                new Translation3d(-0.230448, -0.511957, 0.198406),
                new Rotation3d(new Quaternion(0.753984, 0.347016, 0.409105, 0.379106))),
            new Pose3d(
                new Translation3d(-0.088932, -0.343253, 0.095018),
                new Rotation3d(new Quaternion(0.638738, 0.413016, 0.536281, 0.365833))),
            new Pose3d(
                new Translation3d(-0.107908, -0.317552, 0.133946),
                new Rotation3d(new Quaternion(0.653444, 0.417069, 0.465505, 0.427046))),
            new Pose3d(
                new Translation3d(-0.123383, -0.156411, -0.047435),
                new Rotation3d(new Quaternion(0.652983, 0.40644, 0.431566, 0.47135))),
            new Pose3d(
                new Translation3d(-0.084654, -0.019305, -0.030022),
                new Rotation3d(new Quaternion(0.620243, 0.429104, 0.479384, 0.44873))));

    final var eps = 1E-5;
    for (int i = 0; i < initial_poses.size(); i++) {
      var start = initial_poses.get(i);
      var end = final_poses.get(i);

      var twist = start.log(end);
      var start_exp = start.exp(twist);

      assertAll(
          () -> assertEquals(start_exp.getX(), end.getX(), eps),
          () -> assertEquals(start_exp.getY(), end.getY(), eps),
          () -> assertEquals(start_exp.getZ(), end.getZ(), eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getW(),
                  end.getRotation().getQuaternion().getW(),
                  eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getX(),
                  end.getRotation().getQuaternion().getX(),
                  eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getY(),
                  end.getRotation().getQuaternion().getY(),
                  eps),
          () ->
              assertEquals(
                  start_exp.getRotation().getQuaternion().getZ(),
                  end.getRotation().getQuaternion().getZ(),
                  eps));
    }
  }

  @Test
  void testTwistNaN() {
    var initial_poses =
        List.of(
            new Pose3d(
                new Translation3d(6.32, 4.12, 0.00),
                new Rotation3d(
                    new Quaternion(-0.9999999999999999, 0.0, 0.0, 1.9208309264993548E-8))),
            new Pose3d(
                new Translation3d(3.75, 2.95, 0.00),
                new Rotation3d(
                    new Quaternion(0.9999999999999793, 0.0, 0.0, 2.0352360299846772E-7))));
    var final_poses =
        List.of(
            new Pose3d(
                new Translation3d(6.33, 4.15, 0.00),
                new Rotation3d(
                    new Quaternion(-0.9999999999999999, 0.0, 0.0, 2.416890209039172E-8))),
            new Pose3d(
                new Translation3d(3.66, 2.93, 0.00),
                new Rotation3d(
                    new Quaternion(0.9999999999999782, 0.0, 0.0, 2.0859477994905617E-7))));

    for (int i = 0; i < initial_poses.size(); i++) {
      var start = initial_poses.get(i);
      var end = final_poses.get(i);

      var twist = start.log(end);
      assertAll(
          () -> assertFalse(((Double) twist.dx).isNaN()),
          () -> assertFalse(((Double) twist.dy).isNaN()),
          () -> assertFalse(((Double) twist.dz).isNaN()),
          () -> assertFalse(((Double) twist.rx).isNaN()),
          () -> assertFalse(((Double) twist.ry).isNaN()),
          () -> assertFalse(((Double) twist.rz).isNaN()));
    }
  }
}
