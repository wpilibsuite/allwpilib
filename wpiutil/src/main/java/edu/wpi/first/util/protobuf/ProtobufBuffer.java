// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.protobuf;

import java.io.IOException;
import java.nio.ByteBuffer;
import us.hebi.quickbuf.ProtoMessage;
import us.hebi.quickbuf.ProtoSink;
import us.hebi.quickbuf.ProtoSource;

/**
 * Reusable buffer for serialization/deserialization to/from a protobuf.
 *
 * @param <T> object type
 * @param <MessageType> protobuf message type
 */
public final class ProtobufBuffer<T, MessageType extends ProtoMessage<?>> {
  private ProtobufBuffer(Protobuf<T, MessageType> proto) {
    m_buf = ByteBuffer.allocateDirect(1024);
    m_sink = ProtoSink.newDirectSink();
    m_sink.setOutput(m_buf);
    m_source = ProtoSource.newDirectSource();
    m_msg = proto.createMessage();
    m_proto = proto;
  }

  /**
   * Creates a ProtobufBuffer for the given Protobuf object.
   *
   * @param <T> The type to serialize.
   * @param <MessageType> The Protobuf message type.
   * @param proto The Protobuf object.
   * @return A ProtobufBuffer for the given Protobuf object.
   */
  public static <T, MessageType extends ProtoMessage<?>> ProtobufBuffer<T, MessageType> create(
      Protobuf<T, MessageType> proto) {
    return new ProtobufBuffer<>(proto);
  }

  /**
   * Gets the protobuf object of the stored type.
   *
   * @return protobuf object
   */
  public Protobuf<T, MessageType> getProto() {
    return m_proto;
  }

  /**
   * Gets the type string.
   *
   * @return type string
   */
  public String getTypeString() {
    return m_proto.getTypeString();
  }

  /**
   * Serializes a value to a ByteBuffer. The returned ByteBuffer is a direct byte buffer with the
   * position set to the end of the serialized data.
   *
   * @param value value
   * @return byte buffer
   * @throws IOException if serialization failed
   */
  public ByteBuffer write(T value) throws IOException {
    m_msg.clearQuick();
    m_proto.pack(m_msg, value);
    int size = m_msg.getSerializedSize();
    if (size > m_buf.capacity()) {
      m_buf = ByteBuffer.allocateDirect(size * 2);
      m_sink.setOutput(m_buf);
    }
    m_sink.reset();
    m_msg.writeTo(m_sink);
    m_buf.position(m_sink.getTotalBytesWritten());
    return m_buf;
  }

  /**
   * Deserializes a value from a byte array, creating a new object.
   *
   * @param buf byte array
   * @param start starting location within byte array
   * @param len length of serialized data
   * @return new object
   * @throws IOException if deserialization failed
   */
  public T read(byte[] buf, int start, int len) throws IOException {
    m_msg.clearQuick();
    m_source.setInput(buf, start, len);
    m_msg.mergeFrom(m_source);
    return m_proto.unpack(m_msg);
  }

  /**
   * Deserializes a value from a byte array, creating a new object.
   *
   * @param buf byte array
   * @return new object
   * @throws IOException if deserialization failed
   */
  public T read(byte[] buf) throws IOException {
    return read(buf, 0, buf.length);
  }

  /**
   * Deserializes a value from a ByteBuffer, creating a new object.
   *
   * @param buf byte buffer
   * @return new object
   * @throws IOException if deserialization failed
   */
  public T read(ByteBuffer buf) throws IOException {
    m_msg.clearQuick();
    m_source.setInput(buf);
    m_msg.mergeFrom(m_source);
    return m_proto.unpack(m_msg);
  }

  /**
   * Deserializes a value from a byte array into a mutable object.
   *
   * @param out object (will be updated with deserialized contents)
   * @param buf byte array
   * @param start starting location within byte array
   * @param len length of serialized data
   * @throws IOException if deserialization failed
   * @throws UnsupportedOperationException if the object is immutable
   */
  public void readInto(T out, byte[] buf, int start, int len) throws IOException {
    m_msg.clearQuick();
    m_source.setInput(buf, start, len);
    m_msg.mergeFrom(m_source);
    m_proto.unpackInto(out, m_msg);
  }

  /**
   * Deserializes a value from a byte array into a mutable object.
   *
   * @param out object (will be updated with deserialized contents)
   * @param buf byte array
   * @throws IOException if deserialization failed
   * @throws UnsupportedOperationException if the object is immutable
   */
  public void readInto(T out, byte[] buf) throws IOException {
    readInto(out, buf, 0, buf.length);
  }

  /**
   * Deserializes a value from a ByteBuffer into a mutable object.
   *
   * @param out object (will be updated with deserialized contents)
   * @param buf byte buffer
   * @throws IOException if deserialization failed
   * @throws UnsupportedOperationException if the object is immutable
   */
  public void readInto(T out, ByteBuffer buf) throws IOException {
    m_msg.clearQuick();
    m_source.setInput(buf);
    m_msg.mergeFrom(m_source);
    m_proto.unpackInto(out, m_msg);
  }

  private ByteBuffer m_buf;
  private final ProtoSink m_sink;
  private final ProtoSource m_source;
  private final MessageType m_msg;
  private final Protobuf<T, MessageType> m_proto;
}
