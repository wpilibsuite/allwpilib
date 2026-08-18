// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.trajectory.TrajectorySample;

class TrajectorySampleProtoTest {
  private static final TrajectorySample DATA = new TrajectorySample(1.5);

  @Test
  void testRoundtrip() {
    var proto = TrajectorySample.proto.createMessage();
    TrajectorySample.proto.pack(proto, DATA);

    TrajectorySample data = TrajectorySample.proto.unpack(proto);
    assertEquals(DATA, data);
  }
}
