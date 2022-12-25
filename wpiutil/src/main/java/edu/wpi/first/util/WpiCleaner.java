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
