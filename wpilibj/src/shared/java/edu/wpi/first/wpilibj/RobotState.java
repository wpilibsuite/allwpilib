/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.util.BaseSystemNotInitializedException;

@SuppressWarnings("JavadocMethod")
public class RobotState {
  private static Interface m_impl;

  @SuppressWarnings("MethodName")
  public static void SetImplementation(Interface implementation) {
    m_impl = implementation;
  }

  public static boolean isDisabled() {
    if (m_impl != null) {
      return m_impl.isDisabled();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isEnabled() {
    if (m_impl != null) {
      return m_impl.isEnabled();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isOperatorControl() {
    if (m_impl != null) {
      return m_impl.isOperatorControl();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isAutonomous() {
    if (m_impl != null) {
      return m_impl.isAutonomous();
    } else {
      throw new BaseSystemNotInitializedException(Interface.class, RobotState.class);
    }
  }

  public static boolean isTest() {
    if (m_impl != null) {
      return m_impl.isTest();
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
