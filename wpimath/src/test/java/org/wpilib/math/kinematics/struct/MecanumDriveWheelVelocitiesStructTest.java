// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.MecanumDriveWheelVelocities;

class MecanumDriveWheelVelocitiesStructTest {
  private static final MecanumDriveWheelVelocities DATA =
      new MecanumDriveWheelVelocities(2.29, 17.4, 4.4, 0.229);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(MecanumDriveWheelVelocities.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    MecanumDriveWheelVelocities.struct.pack(buffer, DATA);
    buffer.rewind();

    MecanumDriveWheelVelocities data = MecanumDriveWheelVelocities.struct.unpack(buffer);
    assertEquals(DATA.frontLeft, data.frontLeft);
    assertEquals(DATA.frontRight, data.frontRight);
    assertEquals(DATA.rearLeft, data.rearLeft);
    assertEquals(DATA.rearRight, data.rearRight);
  }
}
