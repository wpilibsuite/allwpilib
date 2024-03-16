// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.DifferentialDriveWheelPositions;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class DifferentialDriveWheelPositionsStructTest {
  private static final DifferentialDriveWheelPositions DATA =
      new DifferentialDriveWheelPositions(1.74, 35.04);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(DifferentialDriveWheelPositions.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    DifferentialDriveWheelPositions.struct.pack(buffer, DATA);
    buffer.rewind();

    DifferentialDriveWheelPositions data = DifferentialDriveWheelPositions.struct.unpack(buffer);
    assertEquals(DATA.leftMeters, data.leftMeters);
    assertEquals(DATA.rightMeters, data.rightMeters);
  }
}
