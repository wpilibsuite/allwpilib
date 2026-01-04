// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.proto.Kinematics.ProtobufChassisVelocities;

class ChassisVelocitiesProtoTest {
  private static final ChassisVelocities DATA = new ChassisVelocities(2.29, 2.2, 0.3504);

  @Test
  void testRoundtrip() {
    ProtobufChassisVelocities proto = ChassisVelocities.proto.createMessage();
    ChassisVelocities.proto.pack(proto, DATA);

    ChassisVelocities data = ChassisVelocities.proto.unpack(proto);
    assertEquals(DATA.vx, data.vx);
    assertEquals(DATA.vy, data.vy);
    assertEquals(DATA.omega, data.omega);
  }
}
