// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.sendable.SendableBuilder;
import java.util.ArrayList;
import java.util.Collection;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** A sendable builder implementation that sends data to a {@link EpilogueBackend}. */
@SuppressWarnings("PMD.CouplingBetweenObjects") // most methods simply delegate to the backend
public class LogBackedSendableBuilder implements SendableBuilder {
  private final EpilogueBackend m_backend;
  private final Collection<Runnable> m_updates = new ArrayList<>();

  /**
   * Creates a new sendable builder that delegates writes to an underlying backend.
   *
   * @param backend the backend to write the sendable data to
   */
  public LogBackedSendableBuilder(EpilogueBackend backend) {
    this.m_backend = backend;
  }

  @Override
  public void setSmartDashboardType(String type) {
    m_backend.log(".type", type);
  }

  @Override
  public void setActuator(boolean value) {
    // ignore
  }

  @Override
  public void setSafeState(Runnable func) {
    // ignore
  }

  @Override
  public void addBooleanProperty(String key, BooleanSupplier getter, BooleanConsumer setter) {
    m_updates.add(() -> m_backend.log(key, getter.getAsBoolean()));
  }

  @Override
  public void publishConstBoolean(String key, boolean value) {
    m_backend.log(key, value);
  }

  @Override
  public void addIntegerProperty(String key, LongSupplier getter, LongConsumer setter) {
    m_updates.add(() -> m_backend.log(key, getter.getAsLong()));
  }

  @Override
  public void publishConstInteger(String key, long value) {
    m_backend.log(key, value);
  }

  @Override
  public void addFloatProperty(String key, FloatSupplier getter, FloatConsumer setter) {
    m_updates.add(() -> m_backend.log(key, getter.getAsFloat()));
  }

  @Override
  public void publishConstFloat(String key, float value) {
    m_backend.log(key, value);
  }

  @Override
  public void addDoubleProperty(String key, DoubleSupplier getter, DoubleConsumer setter) {
    m_updates.add(() -> m_backend.log(key, getter.getAsDouble()));
  }

  @Override
  public void publishConstDouble(String key, double value) {
    m_backend.log(key, value);
  }

  @Override
  public void addStringProperty(String key, Supplier<String> getter, Consumer<String> setter) {
    if (getter != null) {
      m_updates.add(() -> m_backend.log(key, getter.get()));
    }
  }

  @Override
  public void publishConstString(String key, String value) {
    m_backend.log(key, value);
  }

  @Override
  public void addBooleanArrayProperty(
      String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter) {
    if (getter != null) {
      m_updates.add(() -> m_backend.log(key, getter.get()));
    }
  }

  @Override
  public void publishConstBooleanArray(String key, boolean[] value) {
    m_backend.log(key, value);
  }

  @Override
  public void addIntegerArrayProperty(
      String key, Supplier<long[]> getter, Consumer<long[]> setter) {
    if (getter != null) {
      m_updates.add(() -> m_backend.log(key, getter.get()));
    }
  }

  @Override
  public void publishConstIntegerArray(String key, long[] value) {
    m_backend.log(key, value);
  }

  @Override
  public void addFloatArrayProperty(
      String key, Supplier<float[]> getter, Consumer<float[]> setter) {
    if (getter != null) {
      m_updates.add(() -> m_backend.log(key, getter.get()));
    }
  }

  @Override
  public void publishConstFloatArray(String key, float[] value) {
    m_backend.log(key, value);
  }

  @Override
  public void addDoubleArrayProperty(
      String key, Supplier<double[]> getter, Consumer<double[]> setter) {
    if (getter != null) {
      m_updates.add(() -> m_backend.log(key, getter.get()));
    }
  }

  @Override
  public void publishConstDoubleArray(String key, double[] value) {
    m_backend.log(key, value);
  }

  @Override
  public void addStringArrayProperty(
      String key, Supplier<String[]> getter, Consumer<String[]> setter) {
    if (getter != null) {
      m_updates.add(() -> m_backend.log(key, getter.get()));
    }
  }

  @Override
  public void publishConstStringArray(String key, String[] value) {
    m_backend.log(key, value);
  }

  @Override
  public void addRawProperty(
      String key, String typeString, Supplier<byte[]> getter, Consumer<byte[]> setter) {
    if (getter != null) {
      m_updates.add(() -> m_backend.log(key, getter.get()));
    }
  }

  @Override
  public void publishConstRaw(String key, String typeString, byte[] value) {
    m_backend.log(key, value);
  }

  @Override
  public BackendKind getBackendKind() {
    return BackendKind.kUnknown;
  }

  @Override
  public boolean isPublished() {
    return true;
  }

  @Override
  public void update() {
    for (Runnable update : m_updates) {
      update.run();
    }
  }

  @Override
  public void clearProperties() {
    m_updates.clear();
  }

  @Override
  public void addCloseable(AutoCloseable closeable) {
    // Ignore
  }

  @Override
  public void close() throws Exception {
    clearProperties();
  }
}
