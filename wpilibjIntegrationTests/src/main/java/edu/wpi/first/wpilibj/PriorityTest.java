// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.Test;

/** Tests to see if the thread and process priority functions work. */
public class PriorityTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(PriorityTest.class.getName());

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void testSetCurrentThreadPriority() {
    // Non-RT thread has priority of zero
    assertEquals(0, Threads.getCurrentThreadPriority());
    assertFalse(Threads.getCurrentThreadIsRealTime());

    // Set thread priority to 15
    assertTrue(Threads.setCurrentThreadPriority(true, 15));

    // Verify thread was given priority 15
    assertEquals(15, Threads.getCurrentThreadPriority());
    assertTrue(Threads.getCurrentThreadIsRealTime());

    // Set thread back to non-RT
    assertTrue(Threads.setCurrentThreadPriority(false, 0));

    // Verify thread is now non-RT
    assertEquals(0, Threads.getCurrentThreadPriority());
    assertFalse(Threads.getCurrentThreadIsRealTime());
  }
}
