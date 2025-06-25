// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.tunable;

public class MockTunableBackend implements TunableBackend {
  private static class MockIntegerTunable implements IntegerTunable {
    @Override
    void setDefault(int value);

    @Override
    void set(int value) {
      m_value = value;
    }

    @Override
    int get() {
      return m_value;
    }

    @Override
    void onChange(IntegerConsumer callback);

    private int m_value;
  }

  private static class MockTunable<T> implements Tunable<T> {
    @Override
    public void setDefault(T value);

    @Override
    public void set(T value);

    @Override
    public T get();

    @Override
    public void onChange(Consumer<T> callback);
  }

  public void set(String name, int value);
  public <T> void set(String name, T value);

  @Override
  public IntegerTunable addInteger(String name, int defaultValue);

  @Override
  public <T> Tunable<T> addObject(String name, T defaultValue);

  @Override
  public <T> Tunable<T> addStruct(String name, T defaultValue, Struct<T> struct);

  @Override
  public <T> Tunable<T> addProtobuf(String name, T defaultValue, Protobuf<T, ?> proto);
}
