// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.proto.Kinematics.ProtobufDifferentialDriveKinematics;
import org.junit.jupiter.api.Test;

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
