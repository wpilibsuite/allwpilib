// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <string>
#include <string_view>

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

  std::weak_ptr<SendableTableBackend> GetWeak() const { return m_backend; }

 private:
  std::shared_ptr<SendableTableBackend> m_backend;
};

}  // namespace wpi2
