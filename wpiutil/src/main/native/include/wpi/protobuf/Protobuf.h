// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/function_ref.h"

namespace google::protobuf {
class Arena;
class Message;
}  // namespace google::protobuf

namespace wpi {

template <typename T>
class SmallVectorImpl;

/**
 * Protobuf serialization template. Unspecialized class has no members; only
 * specializations of this class are useful, and only if they meet the
 * ProtobufSerializable concept.
 *
 * @tparam T type to serialize/deserialize
 */
template <typename T>
struct Protobuf {};

/**
 * Specifies that a type is capable of protobuf serialization and
 * deserialization.
 *
 * This is designed for serializing complex flexible data structures using
 * code generated from a .proto file. Serialization consists of writing
 * values into a mutable protobuf Message and deserialization consists of
 * reading values from an immutable protobuf Message.
 *
 * Implementations must define a template specialization for wpi::Protobuf with
 * T being the type that is being serialized/deserialized, with the following
 * static members (as enforced by this concept):
 * - google::protobuf::Message* New(google::protobuf::Arena*): create a protobuf
 *   message
 * - T Unpack(const google::protobuf::Message&): function for deserialization
 * - void Pack(google::protobuf::Message*, T&& value): function for
 *   serialization
 *
 * To avoid pulling in the protobuf headers, these functions use
 * google::protobuf::Message instead of a more specific type; implementations
 * will need to static_cast to the correct type as created by New().
 *
 * Additionally: In a static block, call StructRegistry.registerClass() to
 * register the class
 */
template <typename T>
concept ProtobufSerializable = requires(
    google::protobuf::Arena* arena, const google::protobuf::Message& inmsg,
    google::protobuf::Message* outmsg, const T& value) {
  typename Protobuf<typename std::remove_cvref_t<T>>;
  {
    Protobuf<typename std::remove_cvref_t<T>>::New(arena)
  } -> std::same_as<google::protobuf::Message*>;
  {
    Protobuf<typename std::remove_cvref_t<T>>::Unpack(inmsg)
  } -> std::same_as<typename std::remove_cvref_t<T>>;
  Protobuf<typename std::remove_cvref_t<T>>::Pack(outmsg, value);
};

/**
 * Specifies that a type is capable of in-place protobuf deserialization.
 *
 * In addition to meeting ProtobufSerializable, implementations must define a
 * wpi::Protobuf<T> static member
 * `void UnpackInto(T*, const google::protobuf::Message&)` to update the
 * pointed-to T with the contents of the message.
 */
template <typename T>
concept MutableProtobufSerializable =
    ProtobufSerializable<T> &&
    requires(T* out, const google::protobuf::Message& msg) {
      Protobuf<typename std::remove_cvref_t<T>>::UnpackInto(out, msg);
    };

/**
 * Unpack a serialized protobuf message.
 *
 * @tparam T object type
 * @param msg protobuf message
 * @return Deserialized object
 */
template <ProtobufSerializable T>
inline T UnpackProtobuf(const google::protobuf::Message& msg) {
  return Protobuf<T>::Unpack(msg);
}

/**
 * Pack a serialized protobuf message.
 *
 * @param msg protobuf message (mutable, output)
 * @param value object
 */
template <ProtobufSerializable T>
inline void PackProtobuf(google::protobuf::Message* msg, const T& value) {
  Protobuf<typename std::remove_cvref_t<T>>::Pack(msg, value);
}

/**
 * Unpack a serialized struct into an existing object, overwriting its contents.
 *
 * @param out object (output)
 * @param msg protobuf message
 */
template <ProtobufSerializable T>
inline void UnpackProtobufInto(T* out, const google::protobuf::Message& msg) {
  if constexpr (MutableProtobufSerializable<T>) {
    Protobuf<T>::UnpackInto(out, msg);
  } else {
    *out = UnpackProtobuf<T>(msg);
  }
}

// these detail functions avoid the need to include protobuf headers
namespace detail {
void DeleteProtobuf(google::protobuf::Message* msg);
bool ParseProtobuf(google::protobuf::Message* msg,
                   std::span<const uint8_t> data);
bool SerializeProtobuf(wpi::SmallVectorImpl<uint8_t>& out,
                       const google::protobuf::Message& msg);
bool SerializeProtobuf(std::vector<uint8_t>& out,
                       const google::protobuf::Message& msg);
std::string GetTypeString(const google::protobuf::Message& msg);
void ForEachProtobufDescriptor(
    const google::protobuf::Message& msg,
    function_ref<bool(std::string_view filename)> wants,
    function_ref<void(std::string_view filename,
                      std::span<const uint8_t> descriptor)>
        fn);
}  // namespace detail

/**
 * Owning wrapper (ala std::unique_ptr) for google::protobuf::Message* that does
 * not require the protobuf headers be included. Note this object is not thread
 * safe; users of this object are required to provide any necessary thread
 * safety.
 *
 * @tparam T serialized object type
 */
template <ProtobufSerializable T>
class ProtobufMessage {
 public:
  explicit ProtobufMessage(google::protobuf::Arena* arena = nullptr)
      : m_msg{Protobuf<T>::New(arena)} {}
  ~ProtobufMessage() { detail::DeleteProtobuf(m_msg); }
  ProtobufMessage(const ProtobufMessage&) = delete;
  ProtobufMessage& operator=(const ProtobufMessage&) = delete;
  ProtobufMessage(ProtobufMessage&& rhs) : m_msg{rhs.m_msg} {
    rhs.m_msg = nullptr;
  }
  ProtobufMessage& operator=(ProtobufMessage&& rhs) {
    std::swap(m_msg, rhs.m_msg);
    return *this;
  }

  /**
   * Gets the stored message object.
   *
   * @return google::protobuf::Message*
   */
  google::protobuf::Message* GetMessage() { return m_msg; }
  const google::protobuf::Message* GetMessage() const { return m_msg; }

  /**
   * Unpacks from a byte array.
   *
   * @param data byte array
   * @return Optional; empty if parsing failed
   */
  std::optional<T> Unpack(std::span<const uint8_t> data) {
    if (!detail::ParseProtobuf(m_msg, data)) {
      return std::nullopt;
    }
    return Protobuf<T>::Unpack(*m_msg);
  }

  /**
   * Unpacks from a byte array into an existing object.
   *
   * @param[out] out output object
   * @param[in] data byte array
   * @return true if successful
   */
  bool UnpackInto(T* out, std::span<const uint8_t> data) {
    if (!detail::ParseProtobuf(m_msg, data)) {
      return false;
    }
    UnpackProtobufInto(out, *m_msg);
    return true;
  }

  /**
   * Packs object into a SmallVector.
   *
   * @param[out] out output bytes
   * @param[in] value value
   * @return true if successful
   */
  bool Pack(wpi::SmallVectorImpl<uint8_t>& out, const T& value) {
    Protobuf<T>::Pack(m_msg, value);
    return detail::SerializeProtobuf(out, *m_msg);
  }

  /**
   * Packs object into a std::vector.
   *
   * @param[out] out output bytes
   * @param[in] value value
   * @return true if successful
   */
  bool Pack(std::vector<uint8_t>& out, const T& value) {
    Protobuf<T>::Pack(m_msg, value);
    return detail::SerializeProtobuf(out, *m_msg);
  }

  /**
   * Gets the type string for the message.
   *
   * @return type string
   */
  std::string GetTypeString() const { return detail::GetTypeString(*m_msg); }

  /**
   * Loops over all protobuf descriptors including nested/referenced
   * descriptors.
   *
   * @param exists function that returns false if fn should be called for the
   *               given type string
   * @param fn function to call for each descriptor
   */
  void ForEachProtobufDescriptor(
      function_ref<bool(std::string_view filename)> exists,
      function_ref<void(std::string_view filename,
                        std::span<const uint8_t> descriptor)>
          fn) {
    detail::ForEachProtobufDescriptor(*m_msg, exists, fn);
  }

 private:
  google::protobuf::Message* m_msg = nullptr;
};

}  // namespace wpi
