// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

public interface TunableBackend {
  /**
   * Adds a tunable integer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value
   * @return Tunable
   */
  IntTunable addInt(String name, int defaultValue);

  /**
   * Adds a tunable object.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value (may be null)
   * @return Tunable
   */
  <T> Tunable<T> addObject(String name, T defaultValue);

  /**
   * Adds a tunable object with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value (may be null)
   * @param struct struct serializer
   * @return Tunable
   */
  <T> Tunable<T> addStruct(String name, T defaultValue, Struct<T> struct);

  /**
   * Adds a tunable object with a Protobuf serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value (may be null)
   * @param proto protobuf serializer
   * @return Tunable
   */
  <T> Tunable<T> addProtobuf(String name, T defaultValue, Protobuf<T, ?> proto);

  /** Updates all tunable values and calls callbacks where appropriate. */
  void update();
}
