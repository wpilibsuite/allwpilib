package edu.wpi.first.epilogue.logging;

import edu.wpi.first.util.struct.Struct;

/** Null data logger implementation that logs nothing. */
public class NullLogger implements DataLogger {
  @Override
  public DataLogger getSubLogger(String path) {
    // Since a sublogger would still log nothing and has no state, we can just return the same
    // null-logging implementation
    return this;
  }

  @Override
  public void log(String identifier, int value) {}

  @Override
  public void log(String identifier, long value) {}

  @Override
  public void log(String identifier, float value) {}

  @Override
  public void log(String identifier, double value) {}

  @Override
  public void log(String identifier, boolean value) {}

  @Override
  public void log(String identifier, byte[] value) {}

  @Override
  public void log(String identifier, int[] value) {}

  @Override
  public void log(String identifier, long[] value) {}

  @Override
  public void log(String identifier, float[] value) {}

  @Override
  public void log(String identifier, double[] value) {}

  @Override
  public void log(String identifier, boolean[] value) {}

  @Override
  public void log(String identifier, String value) {}

  @Override
  public void log(String identifier, String[] value) {}

  @Override
  public <S> void log(String identifier, S value, Struct<S> struct) {}

  @Override
  public <S> void log(String identifier, S[] value, Struct<S> struct) {}
}
