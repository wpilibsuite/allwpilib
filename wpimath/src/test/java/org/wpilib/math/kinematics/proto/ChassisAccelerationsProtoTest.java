// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.proto.detail.ProtobufChassisAccelerations;

class ChassisAccelerationsProtoTest {
  private static final ChassisAccelerations DATA =
      new ChassisAccelerations(2.29, 2.2, 0.3504);

  @Test
  void testRoundtrip() {
    ProtobufChassisAccelerations proto = ChassisAccelerations.proto.createMessage();
    ChassisAccelerations.proto.pack(proto, DATA);

    ChassisAccelerations data = ChassisAccelerations.proto.unpack(proto);
    assertEquals(DATA.ax, data.ax);
    assertEquals(DATA.ay, data.ay);
    assertEquals(DATA.alpha, data.alpha);
  }
}
