/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.test;

import org.junit.Test;

import java.util.logging.Logger;

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
  public void test() {

  }
}
