package edu.wpi.first.util;

import java.lang.ref.Cleaner;

public class WpiCleaner {

  private WpiCleaner() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  private static Cleaner instance;

  public synchronized static Cleaner getInstance() {
    if (instance == null) {
      instance = Cleaner.create();
    }
    return instance;
  }
}
