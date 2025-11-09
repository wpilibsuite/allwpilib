// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.cleanup;

import java.lang.ref.Cleaner;
import java.lang.ref.Cleaner.Cleanable;

/** Cleaner object for WPILib objects. */
public final class WPICleaner {
  private WPICleaner() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  private static final Cleaner instance = Cleaner.create();

  /**
   * Register an object with the cleaner.
   *
   * @param object The object to register.
   * @param runnable The runnable to call on cleanup.
   * @return The registered Cleanable.
   */
  public static Cleanable register(Object object, Runnable runnable) {
    return instance.register(object, runnable);
  }
}
