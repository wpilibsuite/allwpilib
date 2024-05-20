// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.protobuf.ProtobufBuffer;
import java.io.IOException;
import java.nio.ByteBuffer;
import us.hebi.quickbuf.ProtoMessage;

/**
 * Log protobuf-encoded values.
 *
 * @param <T> value class
 */
public final class ProtobufLogEntry<T> extends DataLogEntry {
  private ProtobufLogEntry(
      DataLog log, String name, Protobuf<T, ?> proto, String metadata, long timestamp) {
    super(log, name, proto.getTypeString(), metadata, timestamp);
    m_buf = ProtobufBuffer.create(proto);
    log.addSchema(proto, timestamp);
  }

  /**
   * Creates a protobuf-encoded log entry.
   *
   * @param <T> value class (inferred from proto)
   * @param <MessageType> protobuf message type (inferred from proto)
   * @param log datalog
   * @param name name of the entry
   * @param proto protobuf serialization implementation
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   * @return ProtobufLogEntry
   */
  public static <T, MessageType extends ProtoMessage<?>> ProtobufLogEntry<T> create(
      DataLog log, String name, Protobuf<T, MessageType> proto, String metadata, long timestamp) {
    return new ProtobufLogEntry<>(log, name, proto, metadata, timestamp);
  }

  /**
   * Creates a protobuf-encoded log entry.
   *
   * @param <T> value class (inferred from proto)
   * @param <MessageType> protobuf message type (inferred from proto)
   * @param log datalog
   * @param name name of the entry
   * @param proto protobuf serialization implementation
   * @param metadata metadata
   * @return ProtobufLogEntry
   */
  public static <T, MessageType extends ProtoMessage<?>> ProtobufLogEntry<T> create(
      DataLog log, String name, Protobuf<T, MessageType> proto, String metadata) {
    return create(log, name, proto, metadata, 0);
  }

  /**
   * Creates a protobuf-encoded log entry.
   *
   * @param <T> value class (inferred from proto)
   * @param <MessageType> protobuf message type (inferred from proto)
   * @param log datalog
   * @param name name of the entry
   * @param proto protobuf serialization implementation
   * @param timestamp entry creation timestamp (0=now)
   * @return ProtobufLogEntry
   */
  public static <T, MessageType extends ProtoMessage<?>> ProtobufLogEntry<T> create(
      DataLog log, String name, Protobuf<T, MessageType> proto, long timestamp) {
    return create(log, name, proto, "", timestamp);
  }

  /**
   * Creates a protobuf-encoded log entry.
   *
   * @param <T> value class (inferred from proto)
   * @param <MessageType> protobuf message type (inferred from proto)
   * @param log datalog
   * @param name name of the entry
   * @param proto protobuf serialization implementation
   * @return ProtobufLogEntry
   */
  public static <T, MessageType extends ProtoMessage<?>> ProtobufLogEntry<T> create(
      DataLog log, String name, Protobuf<T, MessageType> proto) {
    return create(log, name, proto, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(T value, long timestamp) {
    try {
      synchronized (m_buf) {
        ByteBuffer bb = m_buf.write(value);
        m_log.appendRaw(m_entry, bb, 0, bb.position(), timestamp);
      }
    } catch (IOException e) {
      // ignore
    }
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(T value) {
    append(value, 0);
  }

  private final ProtobufBuffer<T, ?> m_buf;
}
