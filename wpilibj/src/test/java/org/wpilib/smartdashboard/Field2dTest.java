// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.HolonomicSample;
import org.wpilib.math.trajectory.HolonomicTrajectory;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

class Field2dTest {
  private MockTunableBackend m_mock;

  @BeforeEach
  void setUp() {
    m_mock = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", m_mock);
  }

  @AfterEach
  void tearDown() {
    TunableRegistry.reset();
  }

  @Test
  void dashboardEditsRobotPoseThroughTunable() {
    Field2d field = new Field2d();
    Tunables.publish("field", field);

    Pose2d pose = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(30.0));
    m_mock.setArray("/field/Robot", new Pose2d[] {pose});
    TunableRegistry.update();

    assertEquals(pose, field.getRobotPose());
  }

  @Test
  void dashboardEditsObjectPosesThroughTunable() {
    Field2d field = new Field2d();
    final FieldObject2d existing = field.getObject("target");
    Tunables.publish("field", field);

    Pose2d first = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(30.0));
    Pose2d second = new Pose2d(3.0, 4.0, Rotation2d.fromDegrees(60.0));

    m_mock.setArray("/field/target", new Pose2d[] {first, second});
    TunableRegistry.update();

    assertEquals(List.of(first, second), existing.getPoses());
  }

  @Test
  void dashboardEditsObjectCreatedAfterTunablePublish() {
    Field2d field = new Field2d();
    Tunables.publish("field", field);
    final FieldObject2d late = field.getObject("target");

    Pose2d pose = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(30.0));

    m_mock.setArray("/field/target", new Pose2d[] {pose});
    TunableRegistry.update();

    assertEquals(List.of(pose), late.getPoses());
  }

  @Test
  void fieldObjectPoseTunablesUseChangeNotifications() {
    Field2d field = new Field2d();
    Tunables.publish("field", field);

    var tunable = m_mock.getTunable("/field/Robot");

    assertEquals(TunableConfig.Polling.GET_ON_CHANGE, tunable.getConfig().getPolling());
    assertFalse(tunable.hasChanged());
  }

  @Test
  void localObjectMutationsNotifyTunableBackend() {
    Field2d field = new Field2d();
    final FieldObject2d object = field.getObject("target");
    Tunables.publish("field", field);

    var tunable = m_mock.getTunable("/field/target");
    Pose2d first = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(30.0));

    tunable.resetChanged();
    object.setPose(first);
    assertTrue(tunable.hasChanged());

    Pose2d second = new Pose2d(3.0, 4.0, Rotation2d.fromDegrees(60.0));

    tunable.resetChanged();
    object.setPoses(List.of(first, second));
    assertTrue(tunable.hasChanged());

    tunable.resetChanged();
    object.setPoses(first, second);
    assertTrue(tunable.hasChanged());

    tunable.resetChanged();
    object.setTrajectory(
        new HolonomicTrajectory(
            List.of(
                new HolonomicSample(
                    0.0, first, new ChassisVelocities(), new ChassisAccelerations()),
                new HolonomicSample(
                    1.0, second, new ChassisVelocities(), new ChassisAccelerations()))));
    assertTrue(tunable.hasChanged());
    assertEquals(List.of(first, second), object.getPoses());
  }

  @Test
  void closeRemovesPublishedTunables() {
    Field2d field = new Field2d();
    field.getObject("target");
    Tunables.publish("field", field);
    Tunables.publish("fieldAlias", field);

    Pose2d staleEdit = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(30.0));
    m_mock.setArray("/field/Robot", new Pose2d[] {staleEdit});
    field.close();
    TunableRegistry.update();

    assertEquals(Pose2d.ZERO, field.getRobotPose());

    assertThrows(
        IllegalArgumentException.class,
        () -> m_mock.setArray("/field/Robot", new Pose2d[] {staleEdit}));
    assertThrows(
        IllegalArgumentException.class,
        () -> m_mock.setArray("/field/target", new Pose2d[] {staleEdit}));
    assertThrows(
        IllegalArgumentException.class,
        () -> m_mock.setArray("/fieldAlias/Robot", new Pose2d[] {staleEdit}));

    Field2d replacement = new Field2d();
    Tunables.publish("field", replacement);
    Tunables.publish("fieldAlias", new Field2d());

    Pose2d replacementPose = new Pose2d(3.0, 4.0, Rotation2d.fromDegrees(60.0));
    m_mock.setArray("/field/Robot", new Pose2d[] {replacementPose});
    TunableRegistry.update();

    assertEquals(replacementPose, replacement.getRobotPose());
  }
}
