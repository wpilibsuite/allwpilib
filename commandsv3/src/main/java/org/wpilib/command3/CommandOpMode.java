package org.wpilib.command3;

import org.wpilib.opmode.OpMode;

public class CommandOpMode implements OpMode {
  @Override
  public void periodic() {
    Scheduler.getDefault().run();
  }
}
