// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModulePosition;
import org.wpilib.math.proto.Kinematics.ProtobufSwerveModulePosition;
import org.junit.jupiter.api.Test;

class SwerveModulePositionProtoTest {
  private static final SwerveModulePosition DATA =
      new SwerveModulePosition(3.504, new Rotation2d(17.4));

  @Test
  void testRoundtrip() {
    ProtobufSwerveModulePosition proto = SwerveModulePosition.proto.createMessage();
    SwerveModulePosition.proto.pack(proto, DATA);

    SwerveModulePosition data = SwerveModulePosition.proto.unpack(proto);
    assertEquals(DATA.distance, data.distance);
    assertEquals(DATA.angle, data.angle);
  }
}
