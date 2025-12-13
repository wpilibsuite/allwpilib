// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.logging;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;
import us.hebi.quickbuf.ProtoMessage;

/** Null backend implementation that logs nothing. */
public class NullBackend implements EpilogueBackend {
  /** Default constructor. */
  public NullBackend() {}

  @Override
  public EpilogueBackend getNested(String path) {
    // Since a nested backend would still log nothing and has no state, we can just return the same
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

  @Override
  public <P, M extends ProtoMessage<M>> void log(
      String identifier, P value, Protobuf<P, M> proto) {}
}
