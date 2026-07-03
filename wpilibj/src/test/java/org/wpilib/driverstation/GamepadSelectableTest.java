// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertIterableEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.util.List;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.GamepadSim;

class GamepadSelectableTest {
  @BeforeEach
  void resetDriverStation() {
    DriverStationSim.resetData();
    DriverStationDisplay.setMode(DriverStationDisplay.Mode.Line);
  }

  @Test
  void addOptionsSelectsFirstOptionByDefault() {
    var selectable = new GamepadSelectable(0);

    GamepadSelectable.Chooser chooser =
        selectable.addOptions("Auto", List.of("Left", "Center", "Right"));

    assertEquals("Left", chooser.getSelected());
    assertEquals("Left", selectable.getSelected("Auto"));
    assertEquals(0, selectable.getSelectedIndex("Auto"));
    assertIterableEquals(List.of("Auto"), selectable.getChooserNames());
    assertEquals(chooser, selectable.getSelectedChooser());
  }

  @Test
  void addIntegerOptionsCreatesRange() {
    var selectable = new GamepadSelectable(0);

    GamepadSelectable.Chooser chooser = selectable.addIntegerOptions("Delay", -2, 3, 2);

    assertIterableEquals(List.of("-2", "0", "2", "3"), chooser.getOptions());
    assertEquals("-2", chooser.getSelected());
    assertEquals(-2, selectable.getSelectedInteger("Delay"));
  }

  @Test
  void addDoubleOptionsCreatesRange() {
    var selectable = new GamepadSelectable(0);

    GamepadSelectable.Chooser chooser = selectable.addDoubleOptions("Speed", 0.0, 1.0, 0.3);

    assertIterableEquals(List.of("0.0", "0.3", "0.6", "0.9", "1.0"), chooser.getOptions());
    assertEquals("0.0", chooser.getSelected());
    assertEquals(0.0, selectable.getSelectedDouble("Speed"));
  }

  @Test
  void dpadMovesBetweenChoosersAndOptions() {
    Gamepad gamepad = new Gamepad(0);
    GamepadSim sim = new GamepadSim(gamepad);
    var selectable = new GamepadSelectable(gamepad);
    GamepadSelectable.Chooser auto =
        selectable.addOptions("Auto", List.of("Left", "Center", "Right"));
    GamepadSelectable.Chooser delay = selectable.addIntegerOptions("Delay", 0, 2, 1);

    tap(sim, selectable, Gamepad.Button.DPAD_RIGHT);
    assertEquals("Center", auto.getSelected());
    assertEquals("0", delay.getSelected());
    assertEquals("Auto", selectable.getSelectedChooser().getName());

    tap(sim, selectable, Gamepad.Button.DPAD_DOWN);
    assertEquals("Delay", selectable.getSelectedChooser().getName());

    tap(sim, selectable, Gamepad.Button.DPAD_RIGHT);
    assertEquals("1", delay.getSelected());
    assertEquals("Center", auto.getSelected());

    tap(sim, selectable, Gamepad.Button.DPAD_LEFT);
    tap(sim, selectable, Gamepad.Button.DPAD_LEFT);
    assertEquals("2", delay.getSelected());

    tap(sim, selectable, Gamepad.Button.DPAD_UP);
    assertEquals("Auto", selectable.getSelectedChooser().getName());

    tap(sim, selectable, Gamepad.Button.DPAD_LEFT);
    tap(sim, selectable, Gamepad.Button.DPAD_LEFT);
    assertEquals("Right", auto.getSelected());
  }

  @Test
  void rejectsInvalidChoosers() {
    var selectable = new GamepadSelectable(0);

    assertThrows(IllegalArgumentException.class, () -> selectable.addOptions("", List.of("A")));
    assertThrows(IllegalArgumentException.class, () -> selectable.addOptions("Empty", List.of()));
    assertThrows(
        IllegalArgumentException.class, () -> selectable.addIntegerOptions("Bad", 0, 2, 0));
    assertThrows(
        IllegalArgumentException.class, () -> selectable.addDoubleOptions("Bad", 0.0, 1.0, 0.0));

    selectable.addOptions("Auto", List.of("A"));
    assertThrows(IllegalArgumentException.class, () -> selectable.addOptions("Auto", List.of("B")));
    assertThrows(IllegalArgumentException.class, () -> selectable.getSelected("Missing"));
  }

  private static void tap(GamepadSim sim, GamepadSelectable selectable, Gamepad.Button button) {
    sim.setButton(button, true);
    sim.notifyNewData();
    selectable.update();

    sim.setButton(button, false);
    sim.notifyNewData();
    selectable.update();
  }
}
