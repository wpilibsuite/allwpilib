// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.lang.ref.Cleaner;

public final class WpiCleaner {

  private WpiCleaner() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  private static Cleaner instance;

  /**
   * Gets the global WPI cleaner instance.
   *
   * @return global cleaner instance.
   */
  public static synchronized Cleaner getInstance() {
    if (instance == null) {
      instance = Cleaner.create();
    }
    return instance;
  }
}
