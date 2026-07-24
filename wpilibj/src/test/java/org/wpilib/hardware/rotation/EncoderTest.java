// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.rotation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.EncoderJNI;
import org.wpilib.hardware.hal.HAL;

class EncoderTest {
  @Test
  void testDecodingScaleFactorUsesHal() {
    HAL.initialize();

    try (Encoder x1 = new Encoder(0, 1, false, Encoder.EncodingType.X1);
        Encoder x2 = new Encoder(2, 3, false, Encoder.EncodingType.X2);
        Encoder x4 = new Encoder(4, 5, false, Encoder.EncodingType.X4)) {
      assertEquals(
          EncoderJNI.getEncoderDecodingScaleFactor(x1.m_encoder), x1.getDecodingScaleFactor());
      assertEquals(
          EncoderJNI.getEncoderDecodingScaleFactor(x2.m_encoder), x2.getDecodingScaleFactor());
      assertEquals(
          EncoderJNI.getEncoderDecodingScaleFactor(x4.m_encoder), x4.getDecodingScaleFactor());
    }
  }
}
