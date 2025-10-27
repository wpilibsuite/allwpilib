// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class MecanumDriveWheelPositionsStructTest {
  private static final MecanumDriveWheelPositions DATA =
      new MecanumDriveWheelPositions(17.4, 2.29, 22.9, 1.74);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(MecanumDriveWheelPositions.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    MecanumDriveWheelPositions.struct.pack(buffer, DATA);
    buffer.rewind();

    MecanumDriveWheelPositions data = MecanumDriveWheelPositions.struct.unpack(buffer);
    assertEquals(DATA.frontLeft, data.frontLeft);
    assertEquals(DATA.frontRight, data.frontRight);
    assertEquals(DATA.rearLeft, data.rearLeft);
    assertEquals(DATA.rearRight, data.rearRight);
  }
}
