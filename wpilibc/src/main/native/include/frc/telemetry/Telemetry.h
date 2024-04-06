// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/json_fwd.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/sendable2/Sendable.h>
#include <wpi/sendable2/SendableOptions.h>
#include <wpi/struct/Struct.h>

namespace wpi2 {
class SendableTable;
}  // namespace wpi2

namespace frc {

class Telemetry {
 public:
  Telemetry() = delete;

  static wpi2::SendableTable SetTable(wpi2::SendableTable table);

  static wpi2::SendableTable GetTable();

  static void SetBoolean(std::string_view name, bool value);

  static void SetInteger(std::string_view name, int64_t value);

  static void SetFloat(std::string_view name, float value);

  static void SetDouble(std::string_view name, double value);

  static void SetString(std::string_view name, std::string_view value);

  static void SetRaw(std::string_view name, std::string_view typeString,
                     std::span<const uint8_t> value);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  static void SetStruct(std::string_view name, const T& value, I... info);

  template <wpi::ProtobufSerializable T>
  static void SetProtobuf(std::string_view name, const T& value);

  static void PublishBoolean(std::string_view name,
                             std::function<bool()> supplier);

  static void PublishInteger(std::string_view name,
                             std::function<int64_t()> supplier);

  static void PublishFloat(std::string_view name,
                           std::function<float()> supplier);

  static void PublishDouble(std::string_view name,
                            std::function<double()> supplier);

  static void PublishString(std::string_view name,
                            std::function<std::string()> supplier);

  static void PublishRaw(std::string_view name, std::string_view typeString,
                         std::function<std::vector<uint8_t>()> supplier);

  static void PublishRawSmall(
      std::string_view name, std::string_view typeString,
      std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
          supplier);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  static void PublishStruct(std::string_view name, std::function<T()> supplier,
                            I... info);

  template <wpi::ProtobufSerializable T>
  static void PublishProtobuf(std::string_view name,
                              std::function<T()> supplier);

  [[nodiscard]]
  static std::function<void(bool)> AddBooleanPublisher(std::string_view name);

  [[nodiscard]]
  static std::function<void(int64_t)> AddIntegerPublisher(
      std::string_view name);

  [[nodiscard]]
  static std::function<void(float)> AddFloatPublisher(std::string_view name);

  [[nodiscard]]
  static std::function<void(double)> AddDoublePublisher(std::string_view name);

  [[nodiscard]]
  static std::function<void(std::string_view)> AddStringPublisher(
      std::string_view name);

  [[nodiscard]]
  static std::function<void(std::span<const uint8_t>)> AddRawPublisher(
      std::string_view name, std::string_view typeString);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  [[nodiscard]]
  static std::function<void(const T&)> AddStructPublisher(std::string_view name,
                                                          I... info);

  template <wpi::ProtobufSerializable T>
  [[nodiscard]]
  static std::function<void(const T&)> AddProtobufPublisher(
      std::string_view name);

  static void SubscribeBoolean(std::string_view name,
                               std::function<void(bool)> consumer);

  static void SubscribeInteger(std::string_view name,
                               std::function<void(int64_t)> consumer);

  static void SubscribeFloat(std::string_view name,
                             std::function<void(float)> consumer);

  static void SubscribeDouble(std::string_view name,
                              std::function<void(double)> consumer);

  static void SubscribeString(std::string_view name,
                              std::function<void(std::string_view)> consumer);

  static void SubscribeRaw(
      std::string_view name, std::string_view typeString,
      std::function<void(std::span<const uint8_t>)> consumer);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  static void SubscribeStruct(std::string_view name,
                              std::function<void(T)> consumer, I... info);

  template <wpi::ProtobufSerializable T>
  static void SubscribeProtobuf(std::string_view name,
                                std::function<void(T)> consumer);

  template <typename T, typename... I>
    requires wpi2::SendableSerializableMoveTracked<T, I...>
  static wpi2::SendableTable AddSendable(std::string_view name, T* obj,
                                         I... info);

  template <typename T, typename... I>
    requires wpi2::SendableSerializableSharedPointer<T, I...>
  static wpi2::SendableTable AddSendable(std::string_view name,
                                         std::shared_ptr<T> obj, I... info);

  static wpi2::SendableTable GetChild(std::string_view name);

  static void SetPublishOptions(const wpi2::SendableOptions& options);

  static void SetSubscribeOptions(const wpi2::SendableOptions& options);

  /**
   * Gets the current value of a property (as a JSON object).
   *
   * @param name name
   * @param propName property name
   * @return JSON object; null object if the property does not exist.
   */
  static wpi::json GetProperty(std::string_view name,
                               std::string_view propName);

  /**
   * Sets a property value.
   *
   * @param name name
   * @param propName property name
   * @param value property value
   */
  static void SetProperty(std::string_view name, std::string_view propName,
                          const wpi::json& value);

  /**
   * Deletes a property.  Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  static void DeleteProperty(std::string_view name, std::string_view propName);

  /**
   * Gets all topic properties as a JSON object.  Each key in the object
   * is the property name, and the corresponding value is the property value.
   *
   * @param name name
   * @return JSON object
   */
  static wpi::json GetProperties(std::string_view name);

  /**
   * Updates multiple topic properties.  Each key in the passed-in object is
   * the name of the property to add/update, and the corresponding value is the
   * property value to set for that property.  Null values result in deletion
   * of the corresponding property.
   *
   * @param name name
   * @param properties JSON object with keys to add/update/delete
   * @return False if properties is not an object
   */
  static bool SetProperties(std::string_view name, const wpi::json& properties);

  static void Remove(std::string_view name);

  /**
   * Erases all publishers and subscribers.
   */
  static void Clear();

 private:
  static wpi2::SendableTable& GetTableHolder();
};

}  // namespace frc

#include "frc/telemetry/Telemetry.inc"
