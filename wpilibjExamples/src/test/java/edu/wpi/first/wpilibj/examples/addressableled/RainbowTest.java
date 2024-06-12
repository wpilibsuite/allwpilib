// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.addressableled;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.AddressableLEDSim;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;
import org.junit.jupiter.api.Test;

class RainbowTest {
  @Test
  void rainbowPatternTest() {
    HAL.initialize(500, 0);
    try (var robot = new Robot()) {
      robot.robotInit();
      AddressableLEDSim ledSim = AddressableLEDSim.createForChannel(9);
      assertTrue(ledSim.getRunning());
      assertEquals(60, ledSim.getLength());

      var rainbowFirstPixelHue = 0;
      for (int iteration = 0; iteration < 100; iteration++) {
        robot.robotPeriodic();
        var data = ledSim.getData();
        for (int i = 0; i < 60; i++) {
          final var hue = (rainbowFirstPixelHue + (i * 180 / 60)) % 180;
          assertIndexColor(data, i, hue, 255, 128);
        }
        rainbowFirstPixelHue += 3;
        rainbowFirstPixelHue %= 180;
      }
    }
  }

  private void assertIndexColor(byte[] data, int index, int hue, int saturation, int value) {
    var color = new Color8Bit(Color.fromHSV(hue, saturation, value));
    int b = data[index * 4];
    int g = data[(index * 4) + 1];
    int r = data[(index * 4) + 2];
    int z = data[(index * 4) + 3];

    assertAll(
        () -> assertEquals(0, z),
        () -> assertEquals(color.red, r & 0xFF),
        () -> assertEquals(color.green, g & 0xFF),
        () -> assertEquals(color.blue, b & 0xFF));
  }
}
