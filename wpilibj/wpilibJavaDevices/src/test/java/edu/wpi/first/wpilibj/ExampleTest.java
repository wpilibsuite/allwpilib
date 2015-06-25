/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import static org.junit.Assert.*;
import org.junit.Test;

/**
 * All test classes should be well-documented as to what you are testing. Any
 * mocks that you create should be put in the same package as the class you are
 * mocking. Unit tests MUST not depend on any specific hardware platform, so any
 * dependencies must be mocked out.
 *$
 *
 * Tests are written in with standard JUnit syntax, and are executed with Junit
 * 4. Tests should be named the same as the unit under test, with "Test"
 * appended. So a test on the Jaguar class would be called JaguarTest.
 *$
 * @author Fredric Silberberg
 */
public class ExampleTest {

  /**
   * Individual tests should be documented, and naming should be
   * "name of thing (unit/function) being tested" + "Test". So a test ensuring
   * that an input called "motor" is null-checked correctly would be called
   * motorNullCheckTest.
   */
  @Test
  public void additionTest() {
    assertEquals(2 + 2, 4);
  }

  /**
   * Tests that expect exceptions should use standard junit expected syntax
   */
  @Test(expected = Exception.class)
  public void exceptionTest() throws Exception {
    throw new Exception("This is expected");
  }
}
