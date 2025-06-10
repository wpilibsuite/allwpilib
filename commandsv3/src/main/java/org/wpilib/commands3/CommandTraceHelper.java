// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

final class CommandTraceHelper {
  private CommandTraceHelper() {
    // Utility class
  }

  /**
   * Creates a modified stack trace where the trace of the scheduling code replaces the trace of
   * the internal scheduler logic.
   *
   * @param commandExceptionTrace The trace of the exception raised during command execution.
   * @param commandScheduleTrace The trace of when the command was scheduled.
   * @return A new array of stack trace elements.
   */
  public static StackTraceElement[] modifyTrace(
      StackTraceElement[] commandExceptionTrace,
      StackTraceElement[] commandScheduleTrace) {
    List<StackTraceElement> frames = new ArrayList<>();

    List<String> filteredClasses = List.of(
        Coroutine.class.getName(),
        Continuation.class.getName(),
        Scheduler.class.getName(),
        "org.wpilib.commands3.CommandBuilder$1",
        "jdk.internal.vm.Continuation"
    );

    boolean sawRun = false;
    for (var exceptionFrame : commandExceptionTrace) {
      if (!filteredClasses.contains(exceptionFrame.getClassName())) {
        frames.add(exceptionFrame);
      }

      // Inject the schedule trace immediately after the line of user code that called Scheduler.run
      if (sawRun) {
        // Inject a marker frame just so there's a distinction between the command's code and the
        // code that scheduled it, since they occur asynchronously
        frames.add(
            new StackTraceElement("=== Command Binding Trace ===", "", "", -1)
        );

        // Drop internal trigger frames, since they're not helpful for users.
        // The first frame here should be the line of user code that bound the command to a trigger.
        Stream.of(commandScheduleTrace)
            .dropWhile(frame -> {
              return frame.getClassName().equals(Trigger.class.getName()) ||
                         (frame.getClassName().equals(Scheduler.class.getName())
                              && frame.getMethodName().equals("schedule"));
            })
            .filter(frame -> {
              return !filteredClasses.contains(frame.getClassName());
            })
                .forEach(frames::add);
        break;
      }

      if (exceptionFrame.getClassName().equals("org.wpilib.commands3.Scheduler")
              && exceptionFrame.getMethodName().equals("run")) {
        sawRun = true;
      }
    }

    return frames.toArray(StackTraceElement[]::new);
  }
}
