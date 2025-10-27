// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Pose3dStructTest {
  private static final Pose3d DATA =
      new Pose3d(
          new Translation3d(1.1, 2.2, 1.1),
          new Rotation3d(new Quaternion(1.91, 0.3504, 3.3, 1.74)));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Pose3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Pose3d.struct.pack(buffer, DATA);
    buffer.rewind();

    Pose3d data = Pose3d.struct.unpack(buffer);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
