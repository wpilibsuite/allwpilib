/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.util.BaseSystemNotInitializedException;

public class RobotState {
  private static Interface impl;

  public static void SetImplementation(Interface i) {
    impl = i;
  }

  public static boolean isDisabled() {
    if (impl != null) {
      return impl.isDisabled();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isEnabled() {
    if (impl != null) {
      return impl.isEnabled();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isOperatorControl() {
    if (impl != null) {
      return impl.isOperatorControl();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isAutonomous() {
    if (impl != null) {
      return impl.isAutonomous();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isTest() {
    if (impl != null) {
      return impl.isTest();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  interface Interface {
    boolean isDisabled();

    boolean isEnabled();

    boolean isOperatorControl();

    boolean isAutonomous();

    boolean isTest();
  }
}
