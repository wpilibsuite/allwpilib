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
import edu.wpi.first.networktables.ProtobufPublisher;
import edu.wpi.first.networktables.Publisher;
import edu.wpi.first.networktables.RawPublisher;
import edu.wpi.first.networktables.StringArrayPublisher;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.networktables.StructArrayPublisher;
import edu.wpi.first.networktables.StructPublisher;
import edu.wpi.first.networktables.Topic;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.function.Function;
import us.hebi.quickbuf.ProtoMessage;

/**
 * A backend implementation that sends data over network tables. Be careful when using this, since
 * sending too much data may cause bandwidth or CPU starvation.
 */
public class NTEpilogueBackend implements EpilogueBackend {
  private final NetworkTableInstance m_nt;

  private final Map<String, Publisher> m_publishers = new HashMap<>();
  private final Map<String, NestedBackend> m_nestedBackends = new HashMap<>();
  private final Set<Struct<?>> m_seenSchemas = new HashSet<>();
  private final Set<Protobuf<?, ?>> m_seenProtos = new HashSet<>();
  private final Function<String, IntegerPublisher> m_createIntPublisher;
  private final Function<String, FloatPublisher> m_createFloatPublisher;
  private final Function<String, DoublePublisher> m_createDoublePublisher;
  private final Function<String, BooleanPublisher> m_createBooleanPublisher;
  private final Function<String, RawPublisher> m_createRawPublisher;
  private final Function<String, IntegerArrayPublisher> m_createIntegerArrayPublisher;
  private final Function<String, FloatArrayPublisher> m_createFloatArrayPublisher;
  private final Function<String, DoubleArrayPublisher> m_createDoubleArrayPublisher;
  private final Function<String, BooleanArrayPublisher> m_createBooleanArrayPublisher;
  private final Function<String, StringPublisher> m_createStringPublisher;
  private final Function<String, StringArrayPublisher> m_createStringArrayPublisher;

  /**
   * Creates a logging backend that sends information to NetworkTables.
   *
   * @param nt the NetworkTable instance to use to send data to
   */
  @SuppressWarnings("unchecked")
  public NTEpilogueBackend(NetworkTableInstance nt) {
    this.m_nt = nt;
    m_createIntPublisher = identifier -> m_nt.getIntegerTopic(identifier).publish();
    m_createFloatPublisher = identifier -> m_nt.getFloatTopic(identifier).publish();
    m_createDoublePublisher = identifier -> m_nt.getDoubleTopic(identifier).publish();
    m_createBooleanPublisher = identifier -> m_nt.getBooleanTopic(identifier).publish();
    m_createRawPublisher = identifier -> m_nt.getRawTopic(identifier).publish("raw");
    m_createIntegerArrayPublisher = identifier -> m_nt.getIntegerArrayTopic(identifier).publish();
    m_createFloatArrayPublisher = identifier -> m_nt.getFloatArrayTopic(identifier).publish();
    m_createDoubleArrayPublisher = identifier -> m_nt.getDoubleArrayTopic(identifier).publish();
    m_createBooleanArrayPublisher = identifier -> m_nt.getBooleanArrayTopic(identifier).publish();
    m_createStringPublisher = identifier -> m_nt.getStringTopic(identifier).publish();
    m_createStringArrayPublisher = identifier -> m_nt.getStringArrayTopic(identifier).publish();
  }

  @Override
  public EpilogueBackend getNested(String path) {
    if (!m_nestedBackends.containsKey(path)) {
      var nested = new NestedBackend(path, this);
      m_nestedBackends.put(path, nested);
      return nested;
    }

    return m_nestedBackends.get(path);
  }

  @Override
  public void log(String identifier, int value) {
    ((IntegerPublisher) m_publishers.computeIfAbsent(identifier, m_createIntPublisher)).set(value);
  }

  @Override
  public void log(String identifier, long value) {
    ((IntegerPublisher) m_publishers.computeIfAbsent(identifier, m_createIntPublisher)).set(value);
  }

  @Override
  public void log(String identifier, float value) {
    ((FloatPublisher) m_publishers.computeIfAbsent(identifier, m_createFloatPublisher)).set(value);
  }

  @Override
  public void log(String identifier, double value) {
    ((DoublePublisher) m_publishers.computeIfAbsent(identifier, m_createDoublePublisher))
        .set(value);
  }

  @Override
  public void log(String identifier, boolean value) {
    ((BooleanPublisher) m_publishers.computeIfAbsent(identifier, m_createBooleanPublisher))
        .set(value);
  }

  @Override
  public void log(String identifier, byte[] value) {
    ((RawPublisher) m_publishers.computeIfAbsent(identifier, m_createRawPublisher)).set(value);
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
            m_publishers.computeIfAbsent(identifier, m_createIntegerArrayPublisher))
        .set(widened);
  }

  @Override
  public void log(String identifier, long[] value) {
    ((IntegerArrayPublisher)
            m_publishers.computeIfAbsent(identifier, m_createIntegerArrayPublisher))
        .set(value);
  }

  @Override
  public void log(String identifier, float[] value) {
    ((FloatArrayPublisher) m_publishers.computeIfAbsent(identifier, m_createFloatArrayPublisher))
        .set(value);
  }

  @Override
  public void log(String identifier, double[] value) {
    ((DoubleArrayPublisher) m_publishers.computeIfAbsent(identifier, m_createDoubleArrayPublisher))
        .set(value);
  }

  @Override
  public void log(String identifier, boolean[] value) {
    ((BooleanArrayPublisher)
            m_publishers.computeIfAbsent(identifier, m_createBooleanArrayPublisher))
        .set(value);
  }

  @Override
  public void log(String identifier, String value) {
    ((StringPublisher) m_publishers.computeIfAbsent(identifier, m_createStringPublisher))
        .set(value);
  }

  @Override
  public void log(String identifier, String[] value) {
    ((StringArrayPublisher) m_publishers.computeIfAbsent(identifier, m_createStringArrayPublisher))
        .set(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S value, Struct<S> struct) {
    // NetworkTableInstance.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_nt.addSchema(struct);
    }

    if (m_publishers.containsKey(identifier)) {
      ((StructPublisher<S>) m_publishers.get(identifier)).set(value);
    } else {
      StructPublisher<S> publisher = m_nt.getStructTopic(identifier, struct).publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    // NetworkTableInstance.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_nt.addSchema(struct);
    }

    if (m_publishers.containsKey(identifier)) {
      ((StructArrayPublisher<S>) m_publishers.get(identifier)).set(value);
    } else {
      StructArrayPublisher<S> publisher = m_nt.getStructArrayTopic(identifier, struct).publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  @SuppressWarnings("unchecked")
  public <P, M extends ProtoMessage<M>> void log(String identifier, P value, Protobuf<P, M> proto) {
    // NetworkTableInstance.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenProtos.add(proto)) {
      m_nt.addSchema(proto);
    }

    if (m_publishers.containsKey(identifier)) {
      ((ProtobufPublisher<P>) m_publishers.get(identifier)).set(value);
    } else {
      ProtobufPublisher<P> publisher = m_nt.getProtobufTopic(identifier, proto).publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  @SuppressWarnings("unchecked")
  public <P, M extends ProtoMessage<M>> void log(
      String identifier, P value, Protobuf<P, M> proto, LogMetadata metadata) {
    // NetworkTableInstance.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenProtos.add(proto)) {
      m_nt.addSchema(proto);
    }

    if (m_publishers.containsKey(identifier)) {
      ((ProtobufPublisher<P>) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getProtobufTopic(identifier, proto);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  // Helper method for applying metadata to topics
  private void applyMetadata(Topic topic, LogMetadata metadata) {
    if (!metadata.isEmpty()) {
      topic.setProperties(metadata.toJson());
    }
  }

  // Metadata-aware log methods
  @Override
  public void log(String identifier, int value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((IntegerPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getIntegerTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, long value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((IntegerPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getIntegerTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, float value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((FloatPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getFloatTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, double value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((DoublePublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getDoubleTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, boolean value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((BooleanPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getBooleanTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, byte[] value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((RawPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getRawTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish("raw");
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  @SuppressWarnings("PMD.UnnecessaryCastRule")
  public void log(String identifier, int[] value, LogMetadata metadata) {
    // NT backend only supports int64[], so we have to manually widen to 64 bits before sending
    long[] widened = new long[value.length];

    for (int i = 0; i < value.length; i++) {
      widened[i] = (long) value[i];
    }

    if (m_publishers.containsKey(identifier)) {
      ((IntegerArrayPublisher) m_publishers.get(identifier)).set(widened);
    } else {
      var topic = m_nt.getIntegerArrayTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(widened);
    }
  }

  @Override
  public void log(String identifier, long[] value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((IntegerArrayPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getIntegerArrayTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, float[] value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((FloatArrayPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getFloatArrayTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, double[] value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((DoubleArrayPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getDoubleArrayTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, boolean[] value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((BooleanArrayPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getBooleanArrayTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, String value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((StringPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getStringTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  public void log(String identifier, String[] value, LogMetadata metadata) {
    if (m_publishers.containsKey(identifier)) {
      ((StringArrayPublisher) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getStringArrayTopic(identifier);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S value, Struct<S> struct, LogMetadata metadata) {
    // NetworkTableInstance.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_nt.addSchema(struct);
    }

    if (m_publishers.containsKey(identifier)) {
      ((StructPublisher<S>) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getStructTopic(identifier, struct);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S[] value, Struct<S> struct, LogMetadata metadata) {
    // NetworkTableInstance.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_nt.addSchema(struct);
    }

    if (m_publishers.containsKey(identifier)) {
      ((StructArrayPublisher<S>) m_publishers.get(identifier)).set(value);
    } else {
      var topic = m_nt.getStructArrayTopic(identifier, struct);
      applyMetadata(topic, metadata);
      var publisher = topic.publish();
      m_publishers.put(identifier, publisher);
      publisher.set(value);
    }
  }
}
