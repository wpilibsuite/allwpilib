// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.Test;

/** Tests to see if the Notifier is working properly. */
public class NotifierTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(NotifierTest.class.getName());
  private static int counter = 0;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void testStartPeriodicAndStop() {
    counter = 0;
    Notifier notifier = new Notifier(() -> ++counter);
    notifier.startPeriodic(1.0);

    Timer.delay(10.5);

    notifier.stop();
    assertEquals("Received " + counter + " notifications in 10.5 seconds\n", 10, counter);
    System.out.println("Received " + counter + " notifications in 10.5 seconds");

    Timer.delay(3.0);

    assertEquals("Received " + (counter - 10) + " notifications in 3 seconds\n", 10, counter);
    System.out.println("Received " + (counter - 10) + " notifications in 3 seconds");

    notifier.close();
  }

  @Test
  public void testStartSingle() {
    counter = 0;
    Notifier notifier = new Notifier(() -> ++counter);
    notifier.startSingle(1.0);

    Timer.delay(10.5);

    assertEquals("Received " + counter + " notifications in 10.5 seconds\n", 1, counter);
    System.out.println("Received " + counter + " notifications in 10.5 seconds");

    notifier.close();
  }
}
