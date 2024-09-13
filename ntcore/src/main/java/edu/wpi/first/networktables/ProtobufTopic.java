// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.protobuf.ProtobufBuffer;

/**
 * NetworkTables protobuf-encoded value topic.
 *
 * @param <T> value class
 */
public final class ProtobufTopic<T> extends Topic {
  private ProtobufTopic(Topic topic, Protobuf<T, ?> proto) {
    super(topic.m_inst, topic.m_handle);
    m_proto = proto;
  }

  private ProtobufTopic(NetworkTableInstance inst, int handle, Protobuf<T, ?> proto) {
    super(inst, handle);
    m_proto = proto;
  }

  /**
   * Create a ProtobufTopic from a generic topic.
   *
   * @param <T> value class (inferred from proto)
   * @param topic generic topic
   * @param proto protobuf serialization implementation
   * @return ProtobufTopic for value class
   */
  public static <T> ProtobufTopic<T> wrap(Topic topic, Protobuf<T, ?> proto) {
    return new ProtobufTopic<>(topic, proto);
  }

  /**
   * Create a ProtobufTopic from a native handle; generally NetworkTableInstance.getProtobufTopic()
   * should be used instead.
   *
   * @param <T> value class (inferred from proto)
   * @param inst Instance
   * @param handle Native handle
   * @param proto protobuf serialization implementation
   * @return ProtobufTopic for value class
   */
  public static <T> ProtobufTopic<T> wrap(
      NetworkTableInstance inst, int handle, Protobuf<T, ?> proto) {
    return new ProtobufTopic<>(inst, handle, proto);
  }

  /**
   * Create a new subscriber to the topic.
   *
   * <p>The subscriber is only active as long as the returned object is not closed.
   *
   * <p>Subscribers that do not match the published data type do not return any values. To determine
   * if the data type matches, use the appropriate Topic functions.
   *
   * @param defaultValue default value used when a default is not provided to a getter function
   * @param options subscribe options
   * @return subscriber
   */
  public ProtobufSubscriber<T> subscribe(T defaultValue, PubSubOption... options) {
    return new ProtobufEntryImpl<>(
        this,
        ProtobufBuffer.create(m_proto),
        NetworkTablesJNI.subscribe(
            m_handle, NetworkTableType.kRaw.getValue(), m_proto.getTypeString(), options),
        defaultValue,
        false);
  }

  /**
   * Create a new publisher to the topic.
   *
   * <p>The publisher is only active as long as the returned object is not closed.
   *
   * <p>It is not possible to publish two different data types to the same topic. Conflicts between
   * publishers are typically resolved by the server on a first-come, first-served basis. Any
   * published values that do not match the topic's data type are dropped (ignored). To determine if
   * the data type matches, use the appropriate Topic functions.
   *
   * @param options publish options
   * @return publisher
   */
  public ProtobufPublisher<T> publish(PubSubOption... options) {
    m_inst.addSchema(m_proto);
    return new ProtobufEntryImpl<>(
        this,
        ProtobufBuffer.create(m_proto),
        NetworkTablesJNI.publish(
            m_handle, NetworkTableType.kRaw.getValue(), m_proto.getTypeString(), options),
        null,
        true);
  }

  /**
   * Create a new publisher to the topic, with type string and initial properties.
   *
   * <p>The publisher is only active as long as the returned object is not closed.
   *
   * <p>It is not possible to publish two different data types to the same topic. Conflicts between
   * publishers are typically resolved by the server on a first-come, first-served basis. Any
   * published values that do not match the topic's data type are dropped (ignored). To determine if
   * the data type matches, use the appropriate Topic functions.
   *
   * @param properties JSON properties
   * @param options publish options
   * @return publisher
   * @throws IllegalArgumentException if properties is not a JSON object
   */
  public ProtobufPublisher<T> publishEx(String properties, PubSubOption... options) {
    m_inst.addSchema(m_proto);
    return new ProtobufEntryImpl<>(
        this,
        ProtobufBuffer.create(m_proto),
        NetworkTablesJNI.publishEx(
            m_handle,
            NetworkTableType.kRaw.getValue(),
            m_proto.getTypeString(),
            properties,
            options),
        null,
        true);
  }

  /**
   * Create a new entry for the topic.
   *
   * <p>Entries act as a combination of a subscriber and a weak publisher. The subscriber is active
   * as long as the entry is not closed. The publisher is created when the entry is first written
   * to, and remains active until either unpublish() is called or the entry is closed.
   *
   * <p>It is not possible to use two different data types with the same topic. Conflicts between
   * publishers are typically resolved by the server on a first-come, first-served basis. Any
   * published values that do not match the topic's data type are dropped (ignored), and the entry
   * will show no new values if the data type does not match. To determine if the data type matches,
   * use the appropriate Topic functions.
   *
   * @param defaultValue default value used when a default is not provided to a getter function
   * @param options publish and/or subscribe options
   * @return entry
   */
  public ProtobufEntry<T> getEntry(T defaultValue, PubSubOption... options) {
    return new ProtobufEntryImpl<>(
        this,
        ProtobufBuffer.create(m_proto),
        NetworkTablesJNI.getEntry(
            m_handle, NetworkTableType.kRaw.getValue(), m_proto.getTypeString(), options),
        defaultValue,
        false);
  }

  /**
   * Returns the protobuf.
   *
   * @return The protobuf.
   */
  public Protobuf<T, ?> getProto() {
    return m_proto;
  }

  @Override
  public boolean equals(Object other) {
    return other == this
        || other instanceof ProtobufTopic<?> topic
            && super.equals(topic)
            && m_proto == topic.m_proto;
  }

  @Override
  public int hashCode() {
    return super.hashCode() ^ m_proto.hashCode();
  }

  private final Protobuf<T, ?> m_proto;
}
