// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import org.junit.jupiter.api.extension.ExtensionContext;
import org.junit.jupiter.api.extension.TestWatcher;

public class DefaultTestWatcher implements TestWatcher {
  /**
   * Allows a failure to supply a custom status message to be displayed along with the stack trace.
   */
  @Override
  public void testFailed(ExtensionContext context, Throwable cause) {
    System.out.println(
        context.getTestClass().get().getName() + "." + context.getDisplayName() + " failed");
  }

  @Override
  public void testSuccessful(ExtensionContext context) {
    System.out.println(
        context.getTestClass().get().getName() + "." + context.getDisplayName() + " passed");
  }
}
