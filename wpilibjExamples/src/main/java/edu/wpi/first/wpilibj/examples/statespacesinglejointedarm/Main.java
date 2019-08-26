package edu.wpi.first.wpilibj.examples.statespacesinglejointedarm;

import edu.wpi.first.wpilibj.RobotBase;

public final class Main {

  private Main() {
  }

  public static void main(String[] args) {
    RobotBase.startRobot(Robot::new);
  }
}
