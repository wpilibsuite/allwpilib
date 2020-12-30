// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import java.util.logging.Logger;
import org.junit.Test;

/**
 * This class is designated to allow for simple testing of the library without the overlying testing
 * framework. This test is NOT run as a normal part of the testing process and must be explicitly
 * selected at runtime by using the 'quick' argument. This test should never be committed with
 * changes to it but can be used during development to aid in feature testing.
 */
public class QuickTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(QuickTest.class.getName());

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.test.AbstractComsSetup#getClassLogger()
   */
  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void test() {}
}
