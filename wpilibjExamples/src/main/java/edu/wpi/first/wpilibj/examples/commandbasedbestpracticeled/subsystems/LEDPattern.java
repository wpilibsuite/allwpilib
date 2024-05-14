// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.util.Color;

/**
 * STUB INTERFACE FOR DEMO PURPOSES
 * 
 * See https://github.com/wpilibsuite/allwpilib/pull/6344
 */
@FunctionalInterface
public interface LEDPattern {
  void applyTo();

  default LEDPattern breathe(Measure<Time> period) {
    return () -> {};
  }

  default LEDPattern blink(Measure<Time> onTime) {
    return () -> {};
  }

  static LEDPattern solid(Color color) {
    return () -> {};
  }
}


/*
import static edu.wpi.first.units.Units.Seconds;
 */
