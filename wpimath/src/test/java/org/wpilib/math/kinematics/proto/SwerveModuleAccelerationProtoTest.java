// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModuleAcceleration;
import org.wpilib.math.kinematics.proto.detail.ProtobufSwerveModuleAcceleration;

class SwerveModuleAccelerationProtoTest {
  private static final SwerveModuleAcceleration DATA =
      new SwerveModuleAcceleration(22.9, new Rotation2d(3.3));

  @Test
  void testRoundtrip() {
    ProtobufSwerveModuleAcceleration proto = SwerveModuleAcceleration.proto.createMessage();
    SwerveModuleAcceleration.proto.pack(proto, DATA);

    SwerveModuleAcceleration data = SwerveModuleAcceleration.proto.unpack(proto);
    assertEquals(DATA.acceleration, data.acceleration);
    assertEquals(DATA.angle, data.angle);
  }
}
