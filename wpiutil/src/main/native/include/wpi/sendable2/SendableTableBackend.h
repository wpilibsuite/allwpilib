// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/json_fwd.h"
#include "wpi/sendable2/SendableOptions.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace wpi2 {

class SendableWrapper;

class SendableTableBackend {
 public:
  virtual ~SendableTableBackend() = default;

  virtual void SetBoolean(std::string_view name, bool value) = 0;

  virtual void SetInteger(std::string_view name, int64_t value) = 0;

  virtual void SetFloat(std::string_view name, float value) = 0;

  virtual void SetDouble(std::string_view name, double value) = 0;

  virtual void SetString(std::string_view name, std::string_view value) = 0;

  virtual void SetRaw(std::string_view name, std::string_view typeString,
                      std::span<const uint8_t> value) = 0;

  virtual void PublishBoolean(std::string_view name,
                              std::function<bool()> supplier) = 0;

  virtual void PublishInteger(std::string_view name,
                              std::function<int64_t()> supplier) = 0;

  virtual void PublishFloat(std::string_view name,
                            std::function<float()> supplier) = 0;

  virtual void PublishDouble(std::string_view name,
                             std::function<double()> supplier) = 0;

  virtual void PublishString(std::string_view name,
                             std::function<std::string()> supplier) = 0;

  virtual void PublishRaw(std::string_view name, std::string_view typeString,
                          std::function<std::vector<uint8_t>()> supplier) = 0;

  virtual void PublishRawSmall(
      std::string_view name, std::string_view typeString,
      std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
          supplier) = 0;

  [[nodiscard]]
  virtual std::function<void(bool)> AddBooleanPublisher(
      std::string_view name) = 0;

  [[nodiscard]]
  virtual std::function<void(int64_t)> AddIntegerPublisher(
      std::string_view name) = 0;

  [[nodiscard]]
  virtual std::function<void(float)> AddFloatPublisher(
      std::string_view name) = 0;

  [[nodiscard]]
  virtual std::function<void(double)> AddDoublePublisher(
      std::string_view name) = 0;

  [[nodiscard]]
  virtual std::function<void(std::string_view)> AddStringPublisher(
      std::string_view name) = 0;

  [[nodiscard]]
  virtual std::function<void(std::span<const uint8_t>)> AddRawPublisher(
      std::string_view name, std::string_view typeString) = 0;

  virtual void SubscribeBoolean(std::string_view name,
                                std::function<void(bool)> consumer) = 0;

  virtual void SubscribeInteger(std::string_view name,
                                std::function<void(int64_t)> consumer) = 0;

  virtual void SubscribeFloat(std::string_view name,
                              std::function<void(float)> consumer) = 0;

  virtual void SubscribeDouble(std::string_view name,
                               std::function<void(double)> consumer) = 0;

  virtual void SubscribeString(
      std::string_view name,
      std::function<void(std::string_view)> consumer) = 0;

  virtual void SubscribeRaw(
      std::string_view name, std::string_view typeString,
      std::function<void(std::span<const uint8_t>)> consumer) = 0;

  virtual std::shared_ptr<SendableTableBackend> CreateSendable(
      std::string_view name, std::unique_ptr<SendableWrapper> sendable) = 0;

  virtual std::shared_ptr<SendableTableBackend> GetChild(
      std::string_view name) = 0;

  virtual void SetPublishOptions(const SendableOptions& options) = 0;

  virtual void SetSubscribeOptions(const SendableOptions& options) = 0;

  /**
   * Gets the current value of a property (as a JSON object).
   *
   * @param name name
   * @param propName property name
   * @return JSON object; null object if the property does not exist.
   */
  virtual wpi::json GetProperty(std::string_view name,
                                std::string_view propName) const = 0;

  /**
   * Sets a property value.
   *
   * @param name name
   * @param propName property name
   * @param value property value
   */
  virtual void SetProperty(std::string_view name, std::string_view propName,
                           const wpi::json& value) = 0;

  /**
   * Deletes a property.  Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  virtual void DeleteProperty(std::string_view name,
                              std::string_view propName) = 0;

  /**
   * Gets all topic properties as a JSON object.  Each key in the object
   * is the property name, and the corresponding value is the property value.
   *
   * @param name name
   * @return JSON object
   */
  virtual wpi::json GetProperties(std::string_view name) const = 0;

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
  virtual bool SetProperties(std::string_view name,
                             const wpi::json& properties) = 0;

  virtual void Remove(std::string_view name) = 0;

  /**
   * Return whether this sendable has been published.
   *
   * @return True if it has been published, false if not.
   */
  virtual bool IsPublished() const = 0;

  /**
   * Update the published values by calling the getters for all properties.
   */
  virtual void Update() = 0;

  /**
   * Erases all publishers and subscribers.
   */
  virtual void Clear() = 0;

  /**
   * Returns whether there is a data schema already registered with the given
   * name.
   *
   * @param name Name (the string passed as the data type for topics using this
   *             schema)
   * @return True if schema already registered
   */
  virtual bool HasSchema(std::string_view name) const = 0;

  /**
   * Registers a data schema.  Data schemas provide information for how a
   * certain data type string can be decoded.  The type string of a data schema
   * indicates the type of the schema itself (e.g. "protobuf" for protobuf
   * schemas, "struct" for struct schemas, etc).
   *
   * @param name Name (the string passed as the data type for topics using this
   *             schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  virtual void AddSchema(std::string_view name, std::string_view type,
                         std::span<const uint8_t> schema) = 0;

  /**
   * Registers a data schema.  Data schemas provide information for how a
   * certain data type string can be decoded.  The type string of a data schema
   * indicates the type of the schema itself (e.g. "protobuf" for protobuf
   * schemas, "struct" for struct schemas, etc).
   *
   * @param name Name (the string passed as the data type for topics using this
   *             schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  virtual void AddSchema(std::string_view name, std::string_view type,
                         std::string_view schema) = 0;
};

}  // namespace wpi2
