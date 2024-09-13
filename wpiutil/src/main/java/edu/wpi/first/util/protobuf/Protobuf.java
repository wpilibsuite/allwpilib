// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.protobuf;

import java.util.function.BiConsumer;
import java.util.function.Predicate;
import us.hebi.quickbuf.Descriptors.Descriptor;
import us.hebi.quickbuf.Descriptors.FileDescriptor;
import us.hebi.quickbuf.ProtoMessage;

/**
 * Interface for Protobuf serialization.
 *
 * <p>This is designed for serialization of more complex data structures including forward/backwards
 * compatibility and repeated/nested/variable length members, etc. Serialization and deserialization
 * code is auto-generated from .proto interface descriptions (the MessageType generic parameter).
 *
 * <p>Idiomatically, classes that support protobuf serialization should provide a static final
 * member named "proto" that provides an instance of an implementation of this interface.
 *
 * @param <T> object type
 * @param <MessageType> protobuf message type
 */
public interface Protobuf<T, MessageType extends ProtoMessage<?>> {
  /**
   * Gets the Class object for the stored value.
   *
   * @return Class
   */
  Class<T> getTypeClass();

  /**
   * Gets the type string (e.g. for NetworkTables). This should be globally unique and start with
   * "proto:".
   *
   * @return type string
   */
  default String getTypeString() {
    return "proto:" + getDescriptor().getFullName();
  }

  /**
   * Gets the protobuf descriptor.
   *
   * @return descriptor
   */
  Descriptor getDescriptor();

  /**
   * Gets the list of protobuf types referenced by this protobuf.
   *
   * @return list of protobuf types
   */
  default Protobuf<?, ?>[] getNested() {
    return new Protobuf<?, ?>[] {};
  }

  /**
   * Creates protobuf message.
   *
   * @return protobuf message
   */
  MessageType createMessage();

  /**
   * Deserializes an object from a protobuf message.
   *
   * @param msg protobuf message
   * @return New object
   */
  T unpack(MessageType msg);

  /**
   * Copies the object contents into a protobuf message. Implementations should call either
   * msg.setMember(member) or member.copyToProto(msg.getMutableMember()) for each member.
   *
   * @param msg protobuf message
   * @param value object to serialize
   */
  void pack(MessageType msg, T value);

  /**
   * Updates the object contents from a protobuf message. Implementations should call
   * msg.getMember(member), MemberClass.makeFromProto(msg.getMember()), or
   * member.updateFromProto(msg.getMember()) for each member.
   *
   * <p>Immutable classes cannot and should not implement this function. The default implementation
   * throws UnsupportedOperationException.
   *
   * @param out object to update
   * @param msg protobuf message
   * @throws UnsupportedOperationException if the object is immutable
   */
  default void unpackInto(T out, MessageType msg) {
    throw new UnsupportedOperationException("object does not support unpackInto");
  }

  /**
   * Loops over all protobuf descriptors including nested/referenced descriptors.
   *
   * @param exists function that returns false if fn should be called for the given type string
   * @param fn function to call for each descriptor
   */
  default void forEachDescriptor(Predicate<String> exists, BiConsumer<String, byte[]> fn) {
    forEachDescriptorImpl(getDescriptor().getFile(), exists, fn);
  }

  private static void forEachDescriptorImpl(
      FileDescriptor desc, Predicate<String> exists, BiConsumer<String, byte[]> fn) {
    String name = "proto:" + desc.getFullName();
    if (exists.test(name)) {
      return;
    }
    for (FileDescriptor dep : desc.getDependencies()) {
      forEachDescriptorImpl(dep, exists, fn);
    }
    fn.accept(name, desc.toProtoBytes());
  }
}
