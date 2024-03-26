// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "wpi/json_fwd.h"
#include "wpi/sendable2/Sendable.h"
#include "wpi/sendable2/SendableTableBackend.h"
#include "wpi/struct/Struct.h"

namespace wpi2 {

class SendableTable final {
 public:
  explicit SendableTable(std::shared_ptr<SendableTableBackend> backend)
      : m_backend{std::move(backend)} {}

  void SetBoolean(std::string_view name, bool value);

  void PublishBoolean(std::string_view name, std::function<bool()> supplier);

  [[nodiscard]]
  std::function<void(bool)> PublishBoolean(std::string_view name);

  void SubscribeBoolean(std::string_view name,
                        std::function<void(bool)> consumer);

  void SetInt(std::string_view name, int64_t value);

  void PublishInt(std::string_view name, std::function<int64_t()> supplier);

  [[nodiscard]]
  std::function<void(int64_t)> PublishInt(std::string_view name);

  void SubscribeInt(std::string_view name,
                    std::function<void(int64_t)> consumer);

  void SetFloat(std::string_view name, float value);

  void PublishFloat(std::string_view name, std::function<float()> supplier);

  [[nodiscard]]
  std::function<void(float)> PublishFloat(std::string_view name);

  void SubscribeFloat(std::string_view name,
                      std::function<void(float)> consumer);

  void SetDouble(std::string_view name, double value);

  void PublishDouble(std::string_view name, std::function<double()> supplier);

  [[nodiscard]]
  std::function<void(double)> PublishDouble(std::string_view name);

  void SubscribeDouble(std::string_view name,
                       std::function<void(double)> consumer);

  void SetString(std::string_view name, std::string_view value);

  void PublishString(std::string_view name,
                     std::function<std::string()> supplier);

  [[nodiscard]]
  std::function<void(std::string_view)> PublishString(std::string_view name);

  void SubscribeString(std::string_view name,
                       std::function<void(std::string_view)> consumer);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  void SetStruct(std::string_view name, const T& obj, I... info);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  void PublishStruct(std::string_view name, std::function<T()> supplier,
                     I... info);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  [[nodiscard]]
  std::function<void(const T&)> PublishStruct(std::string_view name, I... info);

  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  void SubscribeStruct(std::string_view name, std::function<void(T)> consumer,
                       I... info);

  template <typename T, typename... I>
    requires SendableSerializable<T, I...>
  T* AddSendable(std::string_view name, T* obj, I... info);

  template <typename T, typename... I>
    requires SendableSerializable<T, I...>
  T* GetSendable(std::string_view name);

  /**
   * Gets the current value of a property (as a JSON object).
   *
   * @param name name
   * @param propName property name
   * @return JSON object; null object if the property does not exist.
   */
  wpi::json GetProperty(std::string_view name, std::string_view propName) const;

  /**
   * Sets a property value.
   *
   * @param name name
   * @param propName property name
   * @param value property value
   */
  void SetProperty(std::string_view name, std::string_view propName,
                   const wpi::json& value);

  /**
   * Deletes a property.  Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  void DeleteProperty(std::string_view name, std::string_view propName);

  /**
   * Gets all topic properties as a JSON object.  Each key in the object
   * is the property name, and the corresponding value is the property value.
   *
   * @param name name
   * @return JSON object
   */
  wpi::json GetProperties(std::string_view name) const;

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
  bool SetProperties(std::string_view name, const wpi::json& properties);

  void Erase(std::string_view name);

  /**
   * Return whether this sendable has been published.
   *
   * @return True if it has been published, false if not.
   */
  bool IsPublished() const;

  /**
   * Update the published values by calling the getters for all properties.
   */
  void Update();

  /**
   * Clear properties.
   */
  void ClearProperties();

  std::shared_ptr<SendableTableBackend> GetBackend() const { return m_backend; }
  std::weak_ptr<SendableTableBackend> GetWeak() const { return m_backend; }

 private:
  std::shared_ptr<SendableTableBackend> m_backend;
};

}  // namespace wpi2
