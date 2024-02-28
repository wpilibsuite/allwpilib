// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.util.Color;
import org.junit.jupiter.api.Test;

class AddressableLEDBufferViewTest {
  @Test
  void singleLED() {
    var buffer = new AddressableLEDBuffer(10);
    var view = new AddressableLEDBufferView(buffer, 5, 5);
    var color = Color.kAqua;
    view.setLED(0, color);
    assertEquals(color, buffer.getLED(5));
    assertEquals(color, view.getLED(0));
  }

  @Test
  void segment() {
    var buffer = new AddressableLEDBuffer(10);
    var view = new AddressableLEDBufferView(buffer, 2, 8);
    view.setLED(0, Color.kAqua);
    assertEquals(Color.kAqua, buffer.getLED(2));

    view.setLED(6, Color.kAzure);
    assertEquals(Color.kAzure, buffer.getLED(8));
  }

  @Test
  void manualReversed() {
    var buffer = new AddressableLEDBuffer(10);
    var view = new AddressableLEDBufferView(buffer, 8, 2);

    // LED 0 in the view should write to LED 8 on the real buffer
    view.setLED(0, Color.kAqua);
    assertEquals(Color.kAqua, buffer.getLED(8));

    // .. and LED 6 in the view should write to LED 2 on the real buffer
    view.setLED(6, Color.kAzure);
    assertEquals(Color.kAzure, buffer.getLED(2));
  }

  @Test
  void fullManualReversed() {
    var buffer = new AddressableLEDBuffer(10);
    var view = new AddressableLEDBufferView(buffer, 9, 0);
    view.setLED(0, Color.kWhite);
    assertEquals(Color.kWhite, buffer.getLED(9));

    buffer.setLED(8, Color.kRed);
    assertEquals(Color.kRed, view.getLED(1));
  }

  @Test
  void reversed() {
    var buffer = new AddressableLEDBuffer(10);
    var view = new AddressableLEDBufferView(buffer, 0, 9).reversed();
    view.setLED(0, Color.kWhite);
    assertEquals(Color.kWhite, buffer.getLED(9));

    view.setLED(9, Color.kRed);
    assertEquals(Color.kRed, buffer.getLED(0));
  }
}
