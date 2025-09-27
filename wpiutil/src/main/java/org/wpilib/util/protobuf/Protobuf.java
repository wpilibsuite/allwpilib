// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.protobuf;

import java.lang.reflect.Array;
import java.util.function.BiConsumer;
import java.util.function.Predicate;
import us.hebi.quickbuf.Descriptors.Descriptor;
import us.hebi.quickbuf.Descriptors.FileDescriptor;
import us.hebi.quickbuf.ProtoMessage;
import us.hebi.quickbuf.RepeatedDouble;
import us.hebi.quickbuf.RepeatedMessage;

/**
 * Interface for Protobuf serialization.
 *
 * <p>This is designed for serialization of more complex data structures including forward/backwards
 * compatibility and repeated/nested/variable length members, etc. Serialization and deserialization
 * code is auto-generated from .proto interface descriptions (the MessageType generic parameter).
 *
 * <p>Idiomatically, classes that support protobuf serialization should provide a static final
 * member named "proto" that provides an instance of an implementation of this interface, or a
 * static final method named "getProto" if the class is generic.
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
   * Returns whether or not objects are immutable. Immutable objects must also be comparable using
   * the equals() method. Default implementation returns false.
   *
   * @return True if object is immutable
   */
  default boolean isImmutable() {
    return false;
  }

  /**
   * Returns whether or not objects are cloneable using the clone() method. Cloneable objects must
   * also be comparable using the equals() method. Default implementation returns false.
   *
   * @return True if object is cloneable
   */
  default boolean isCloneable() {
    return false;
  }

  /**
   * Creates a (deep) clone of the object. May also return the object directly if the object is
   * immutable. Default implementation throws CloneNotSupportedException. Typically this should be
   * implemented by implementing clone() on the object itself, and calling it from here.
   *
   * @param obj object to clone
   * @return Clone of object (if immutable, may be same object)
   * @throws CloneNotSupportedException if clone not supported
   */
  default T clone(T obj) throws CloneNotSupportedException {
    throw new CloneNotSupportedException();
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

  /**
   * Unpack a serialized protobuf array message.
   *
   * @param <T> object type
   * @param <MessageType> element type of the protobuf array
   * @param msg protobuf array message
   * @param proto protobuf implementation
   * @return Deserialized array
   */
  static <T, MessageType extends ProtoMessage<MessageType>> T[] unpackArray(
      RepeatedMessage<MessageType> msg, Protobuf<T, MessageType> proto) {
    @SuppressWarnings("unchecked")
    T[] result = (T[]) Array.newInstance(proto.getTypeClass(), msg.length());
    for (int i = 0; i < result.length; i++) {
      result[i] = proto.unpack(msg.get(i));
    }
    return result;
  }

  /**
   * Unpack a serialized protobuf double array message.
   *
   * @param msg protobuf double array message
   * @return Deserialized array
   */
  static double[] unpackArray(RepeatedDouble msg) {
    double[] result = new double[msg.length()];
    for (int i = 0; i < result.length; i++) {
      result[i] = msg.get(i);
    }
    return result;
  }

  /**
   * Pack a serialized protobuf array message.
   *
   * @param <T> object type
   * @param <MessageType> element type of the protobuf array
   * @param msg protobuf array message
   * @param arr array of objects
   * @param proto protobuf implementation
   */
  static <T, MessageType extends ProtoMessage<MessageType>> void packArray(
      RepeatedMessage<MessageType> msg, T[] arr, Protobuf<T, MessageType> proto) {
    msg.clear();
    msg.reserve(arr.length);
    for (var obj : arr) {
      proto.pack(msg.next(), obj);
    }
  }

  /**
   * Pack a serialized protobuf double array message.
   *
   * @param msg protobuf double array message
   * @param arr array of objects
   */
  static void packArray(RepeatedDouble msg, double[] arr) {
    msg.clear();
    msg.reserve(arr.length);
    msg.addAll(arr);
  }
}
