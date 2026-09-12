// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

/**
 * A benchmark for measuring runtime of `new Throwable()` and `new Throwable().getStackTrace()`. The
 * long line of nested methods is intended to replicate calling `Scheduler.getDefault().schedule()`
 * in actual robot code (where the callsite is likely to be highly nested).
 */
public final class StackTraceGatheringBenchmark {
  public static void run(boolean getStackTrace) {
    method1(getStackTrace);
  }

  private static void method1(boolean getStackTrace) {
    method2(getStackTrace);
  }

  private static void method2(boolean getStackTrace) {
    method3(getStackTrace);
  }

  private static void method3(boolean getStackTrace) {
    method4(getStackTrace);
  }

  @SuppressWarnings("PMD.UselessPureMethodCall")
  private static void method4(boolean getStackTrace) {
    var throwable = new Throwable();
    if (getStackTrace) {
      throwable.getStackTrace();
    }
  }

  private StackTraceGatheringBenchmark() {
    throw new UnsupportedOperationException("This is a utility class.");
  }
}
