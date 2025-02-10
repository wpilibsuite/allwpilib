// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import edu.wpi.first.math.proto.Kinematics.ProtobufSwerveModuleState;
import org.junit.jupiter.api.Test;

class SwerveModuleStateProtoTest {
  private static final SwerveModuleState DATA = new SwerveModuleState(22.9, new Rotation2d(3.3));

  @Test
  void testRoundtrip() {
    ProtobufSwerveModuleState proto = SwerveModuleState.proto.createMessage();
    SwerveModuleState.proto.pack(proto, DATA);

    SwerveModuleState data = SwerveModuleState.proto.unpack(proto);
    assertEquals(DATA.speed, data.speed);
    assertEquals(DATA.angle, data.angle);
  }
}
