// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.MecanumDriveWheelAccelerations;

class MecanumDriveWheelAccelerationsStructTest {
  private static final MecanumDriveWheelAccelerations DATA =
      new MecanumDriveWheelAccelerations(2.29, 17.4, 4.4, 0.229);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(MecanumDriveWheelAccelerations.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    MecanumDriveWheelAccelerations.struct.pack(buffer, DATA);
    buffer.rewind();

    MecanumDriveWheelAccelerations data = MecanumDriveWheelAccelerations.struct.unpack(buffer);
    assertEquals(DATA.frontLeft, data.frontLeft);
    assertEquals(DATA.frontRight, data.frontRight);
    assertEquals(DATA.rearLeft, data.rearLeft);
    assertEquals(DATA.rearRight, data.rearRight);
  }
}
