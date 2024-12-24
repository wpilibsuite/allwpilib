// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.networktables.BooleanArrayPublisher;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.DoubleArrayPublisher;
import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.FloatArrayPublisher;
import edu.wpi.first.networktables.FloatPublisher;
import edu.wpi.first.networktables.IntegerArrayPublisher;
import edu.wpi.first.networktables.IntegerPublisher;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.Publisher;
import edu.wpi.first.networktables.RawPublisher;
import edu.wpi.first.networktables.StringArrayPublisher;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.networktables.StructArrayPublisher;
import edu.wpi.first.networktables.StructPublisher;
import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.Map;
import java.util.function.BooleanSupplier;

/**
 * A backend implementation that sends data over network tables. Be careful when using this, since
 * sending too much data may cause bandwidth or CPU starvation.
 */
public class NTBackend implements EpilogueBackend {
  private final NetworkTableInstance m_nt;
  private BooleanSupplier m_disabledSupplier;

  private final Map<String, Publisher> m_publishers = new HashMap<>();
  private final Map<String, NestedBackend> m_nestedBackends = new HashMap<>();

  /**
   * Creates a logging backend that sends information to NetworkTables.
   *
   * @param nt the NetworkTable instance to use to send data to
   */
  public NTBackend(NetworkTableInstance nt) {
    this.m_nt = nt;
  }

  /** Creates a logging backend that sends information to NetworkTables. */
  public NTBackend() {
    this.m_nt = NetworkTableInstance.getDefault();
  }

  /**
   * Creates a new NTBackend that is disabled whenever the disabledSupplier returns true.
   *
   * @param disabledSupplier the disable condition for NT logging
   * @return a new NTBackend
   */
  public NTBackend disableWhen(BooleanSupplier disabledSupplier) {
    var newBackend = new NTBackend(this.m_nt);
    if (m_disabledSupplier == null) {
      newBackend.m_disabledSupplier = disabledSupplier;
    } else {
      newBackend.m_disabledSupplier =
          () -> m_disabledSupplier.getAsBoolean() || disabledSupplier.getAsBoolean();
    }
    return newBackend;
  }

  @Override
  public EpilogueBackend getNested(String path) {
    return m_nestedBackends.computeIfAbsent(path, k -> new NestedBackend(k, this));
  }

  @Override
  public void log(String identifier, int value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((IntegerPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getIntegerTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, long value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((IntegerPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getIntegerTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, float value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((FloatPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getFloatTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, double value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((DoublePublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getDoubleTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, boolean value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((BooleanPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getBooleanTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, byte[] value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((RawPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getRawTopic(k).publish("raw")))
        .set(value);
  }

  @Override
  @SuppressWarnings("PMD.UnnecessaryCastRule")
  public void log(String identifier, int[] value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    // NT backend only supports int64[], so we have to manually widen to 64 bits before sending
    long[] widened = new long[value.length];

    for (int i = 0; i < value.length; i++) {
      widened[i] = (long) value[i];
    }

    ((IntegerArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getIntegerArrayTopic(k).publish()))
        .set(widened);
  }

  @Override
  public void log(String identifier, long[] value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((IntegerArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getIntegerArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, float[] value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((FloatArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getFloatArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, double[] value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((DoubleArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getDoubleArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, boolean[] value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((BooleanArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getBooleanArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, String value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((StringPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getStringTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, String[] value) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    ((StringArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getStringArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S value, Struct<S> struct) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    m_nt.addSchema(struct);
    ((StructPublisher<S>)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getStructTopic(k, struct).publish()))
        .set(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    if (m_disabledSupplier != null && m_disabledSupplier.getAsBoolean()) {
      return;
    }
    m_nt.addSchema(struct);
    ((StructArrayPublisher<S>)
            m_publishers.computeIfAbsent(
                identifier, k -> m_nt.getStructArrayTopic(k, struct).publish()))
        .set(value);
  }
}
