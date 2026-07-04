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

class DSGamepadChooserTest {
  @BeforeEach
  void resetDriverStation() {
    DriverStationSim.resetData();
    DriverStationDisplay.setMode(DriverStationDisplay.Mode.Line);
  }

  @Test
  void addOptionsSelectsFirstOptionByDefault() {
    var chooser = new DSGamepadChooser(0);

    DSGamepadChooser.GamepadSelectable selectable =
        chooser.addOptions("Auto", List.of("Left", "Center", "Right"));

    assertEquals("Left", selectable.getSelected());
    assertEquals("Left", chooser.getSelected("Auto"));
    assertEquals(0, chooser.getSelectedIndex("Auto"));
    assertIterableEquals(List.of("Auto"), chooser.getSelectableNames());
    assertEquals(selectable, chooser.getSelectedSelectable());
  }

  @Test
  void addIntegerOptionsCreatesRange() {
    var chooser = new DSGamepadChooser(0);

    DSGamepadChooser.GamepadSelectable selectable = chooser.addIntegerOptions("Delay", -2, 3, 2);

    assertIterableEquals(List.of("-2", "0", "2", "3"), selectable.getOptions());
    assertEquals("-2", selectable.getSelected());
    assertEquals(-2, chooser.getSelectedInteger("Delay"));
  }

  @Test
  void addDoubleOptionsCreatesRange() {
    var chooser = new DSGamepadChooser(0);

    DSGamepadChooser.GamepadSelectable selectable =
        chooser.addDoubleOptions("Speed", 0.0, 1.0, 0.3);

    assertIterableEquals(List.of("0.0", "0.3", "0.6", "0.9", "1.0"), selectable.getOptions());
    assertEquals("0.0", selectable.getSelected());
    assertEquals(0.0, chooser.getSelectedDouble("Speed"));
  }

  @Test
  void addDoubleOptionsDoesNotDuplicateExactMaximumWithDifferentScale() {
    var chooser = new DSGamepadChooser(0);

    DSGamepadChooser.GamepadSelectable selectable =
        chooser.addDoubleOptions("Speed", 0.25, 1.0, 0.25);

    assertIterableEquals(List.of("0.25", "0.50", "0.75", "1.00"), selectable.getOptions());
  }

  @Test
  void dpadMovesBetweenSelectablesAndOptions() {
    Gamepad gamepad = new Gamepad(0);
    GamepadSim sim = new GamepadSim(gamepad);
    var chooser = new DSGamepadChooser(gamepad);
    DSGamepadChooser.GamepadSelectable auto =
        chooser.addOptions("Auto", List.of("Left", "Center", "Right"));
    DSGamepadChooser.GamepadSelectable delay = chooser.addIntegerOptions("Delay", 0, 2, 1);

    tap(sim, chooser, Gamepad.Button.DPAD_RIGHT);
    assertEquals("Center", auto.getSelected());
    assertEquals("0", delay.getSelected());
    assertEquals("Auto", chooser.getSelectedSelectable().getName());

    tap(sim, chooser, Gamepad.Button.DPAD_DOWN);
    assertEquals("Delay", chooser.getSelectedSelectable().getName());

    tap(sim, chooser, Gamepad.Button.DPAD_RIGHT);
    assertEquals("1", delay.getSelected());
    assertEquals("Center", auto.getSelected());

    tap(sim, chooser, Gamepad.Button.DPAD_LEFT);
    tap(sim, chooser, Gamepad.Button.DPAD_LEFT);
    assertEquals("2", delay.getSelected());

    tap(sim, chooser, Gamepad.Button.DPAD_UP);
    assertEquals("Auto", chooser.getSelectedSelectable().getName());

    tap(sim, chooser, Gamepad.Button.DPAD_LEFT);
    tap(sim, chooser, Gamepad.Button.DPAD_LEFT);
    assertEquals("Right", auto.getSelected());
  }

  @Test
  void rejectsInvalidSelectables() {
    var chooser = new DSGamepadChooser(0);

    assertThrows(IllegalArgumentException.class, () -> chooser.addOptions("", List.of("A")));
    assertThrows(IllegalArgumentException.class, () -> chooser.addOptions("Empty", List.of()));
    assertThrows(IllegalArgumentException.class, () -> chooser.addIntegerOptions("Bad", 0, 2, 0));
    assertThrows(
        IllegalArgumentException.class, () -> chooser.addDoubleOptions("Bad", 0.0, 1.0, 0.0));

    chooser.addOptions("Auto", List.of("A"));
    assertThrows(IllegalArgumentException.class, () -> chooser.addOptions("Auto", List.of("B")));
    assertThrows(IllegalArgumentException.class, () -> chooser.getSelected("Missing"));
  }

  private static void tap(GamepadSim sim, DSGamepadChooser chooser, Gamepad.Button button) {
    sim.setButton(button, true);
    sim.notifyNewData();
    chooser.update();

    sim.setButton(button, false);
    sim.notifyNewData();
    chooser.update();
  }
}
