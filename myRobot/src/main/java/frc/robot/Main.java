// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.DMA;
import edu.wpi.first.wpilibj.DMASample;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.RobotBase;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.util.Units;

public final class Main {
  private Main() {}

  /**
   * Main initialization function. Do not perform any initialization here.
   *
   * <p>If you change your main robot class, change the parameter type.
   */
  public static void main(String... args) {
    RobotBase.startRobot(MyRobot::new);
    // HAL.initialize(500, 0);

    // DMA dma = new DMA();
    // DMASample dmaSample = new DMASample();

    // AnalogInput ai = new AnalogInput(0);
    // DigitalOutput doo = new DigitalOutput(0);

    // dma.addAnalogInput(ai);
    // dma.setExternalTrigger(doo, false, true);
    // //dma.setTimedTrigger(Units.millisecondsToSeconds(10));

    // dma.startDMA(1023);

    // Timer.delay(0.1);

    // dma.setPause(true);

    // dmaSample.update(dma, Units.millisecondsToSeconds(1));

    // System.out.println(dmaSample.getRemaining());
  }
}
