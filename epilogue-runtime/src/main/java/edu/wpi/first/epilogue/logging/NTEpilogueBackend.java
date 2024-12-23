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

/**
 * A backend implementation that sends data over network tables. Be careful when using this, since
 * sending too much data may cause bandwidth or CPU starvation.
 */
public class NTEpilogueBackend implements EpilogueBackend {
  private final NetworkTableInstance m_nt;

  private final Map<String, Publisher> m_publishers = new HashMap<>();
  private final Map<String, NestedBackend> m_nestedBackends = new HashMap<>();

  /**
   * Creates a logging backend that sends information to NetworkTables.
   *
   * @param nt the NetworkTable instance to use to send data to
   */
  public NTEpilogueBackend(NetworkTableInstance nt) {
    this.m_nt = nt;
  }

  @Override
  public EpilogueBackend getNested(String path) {
    return m_nestedBackends.computeIfAbsent(path, k -> new NestedBackend(k, this));
  }

  @Override
  public void log(String identifier, int value) {
    ((IntegerPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getIntegerTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, long value) {
    ((IntegerPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getIntegerTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, float value) {
    ((FloatPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getFloatTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, double value) {
    ((DoublePublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getDoubleTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, boolean value) {
    ((BooleanPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getBooleanTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, byte[] value) {
    ((RawPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getRawTopic(k).publish("raw")))
        .set(value);
  }

  @Override
  @SuppressWarnings("PMD.UnnecessaryCastRule")
  public void log(String identifier, int[] value) {
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
    ((IntegerArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getIntegerArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, float[] value) {
    ((FloatArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getFloatArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, double[] value) {
    ((DoubleArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getDoubleArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, boolean[] value) {
    ((BooleanArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getBooleanArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, String value) {
    ((StringPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getStringTopic(k).publish()))
        .set(value);
  }

  @Override
  public void log(String identifier, String[] value) {
    ((StringArrayPublisher)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getStringArrayTopic(k).publish()))
        .set(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S value, Struct<S> struct) {
    m_nt.addSchema(struct);
    ((StructPublisher<S>)
            m_publishers.computeIfAbsent(identifier, k -> m_nt.getStructTopic(k, struct).publish()))
        .set(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    m_nt.addSchema(struct);
    ((StructArrayPublisher<S>)
            m_publishers.computeIfAbsent(
                identifier, k -> m_nt.getStructArrayTopic(k, struct).publish()))
        .set(value);
  }
}
