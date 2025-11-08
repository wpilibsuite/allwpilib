// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.proto.Kinematics.ProtobufDifferentialDriveKinematics;

class DifferentialDriveKinematicsProtoTest {
  private static final DifferentialDriveKinematics DATA = new DifferentialDriveKinematics(1.74);

  @Test
  void testRoundtrip() {
    ProtobufDifferentialDriveKinematics proto = DifferentialDriveKinematics.proto.createMessage();
    DifferentialDriveKinematics.proto.pack(proto, DATA);

    DifferentialDriveKinematics data = DifferentialDriveKinematics.proto.unpack(proto);
    assertEquals(DATA.trackwidth, data.trackwidth);
  }
}
