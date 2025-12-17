// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.javacplugin;

import com.sun.source.util.JavacTask;
import com.sun.source.util.Plugin;

/**
 * A javac compiler plugin that adds compiler warnings for incorrect usage of WPILib types. Also
 * supports WPILib's custom annotations like @NoDiscard.
 */
public class WPILibJavacPlugin implements Plugin {
  @Override
  public String getName() {
    return "WPILib";
  }

  @Override
  public void init(JavacTask task, String... args) {
    task.addTaskListener(new ReturnValueUsedListener(task));
    task.addTaskListener(new MaxLengthDetector(task));
    task.addTaskListener(new CoroutineYieldInLoopDetector(task));
    task.addTaskListener(new CodeAfterCoroutineParkDetector(task));
    task.addTaskListener(new IncorrectCoroutineUseDetector(task));
  }

  @Override
  public boolean autoStart() {
    // autoStart means we don't need to manually pass -Xplugin:WPILib to the javac compiler args
    // for the plugin to run
    return true;
  }
}
