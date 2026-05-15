// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib;

import org.wpilib.framework.RobotBase;

/** This is the executor to launch template projects. */
public final class Executor {
  private Executor() {}

  /** Main initialization function. */
  public static void main(String... args) throws Throwable {
    // Load the class file for the robot.
    String packagePath = args[0];
    ClassLoader classLoader = Thread.currentThread().getContextClassLoader();

    System.out.println("Loading robot class: " + packagePath);

    Class<?> robotClass = classLoader.loadClass(packagePath);

    System.out.println("Starting robot: " + robotClass.getName());

    RobotBase.startRobot(robotClass.asSubclass(RobotBase.class));
  }
}
