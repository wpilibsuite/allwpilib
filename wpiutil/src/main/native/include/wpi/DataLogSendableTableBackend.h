// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string_view>

#include "wpi/mutex.h"
#include "wpi/sendable2/SendableTableBackend.h"
#include "wpi/DataLog.h"
#include "wpi/StringMap.h"

namespace wpi::log {

/** DataLog-backed implementation of SendableTable. */
class DataLogSendableTableBackend : public wpi2::SendableTableBackend {
 public:
  /**
   * Constructs a DataLog-backed sendable table.
   *
   * @param log DataLog
   * @param path path to sendable table, excluding trailing '/'
   */
  DataLogSendableTableBackend(DataLog& log, std::string_view path);
  ~DataLogSendableTableBackend();

  /**
   * Gets the DataLog for the table.
   *
   * @return DataLog
   */
  DataLog& GetLog() const { return m_log; }

  void SetBoolean(std::string_view name, bool value) override;

  void SetInteger(std::string_view name, int64_t value) override;

  void SetFloat(std::string_view name, float value) override;

  void SetDouble(std::string_view name, double value) override;

  void SetString(std::string_view name, std::string_view value) override;

  void SetRaw(std::string_view name, std::string_view typeString,
              std::span<const uint8_t> value) override;

  void PublishBoolean(std::string_view name,
                      std::function<bool()> supplier) override;

  void PublishInteger(std::string_view name,
                      std::function<int64_t()> supplier) override;

  void PublishFloat(std::string_view name,
                    std::function<float()> supplier) override;

  void PublishDouble(std::string_view name,
                     std::function<double()> supplier) override;

  void PublishString(std::string_view name,
                     std::function<std::string()> supplier) override;

  void PublishRaw(std::string_view name, std::string_view typeString,
                  std::function<std::vector<uint8_t>()> supplier) override;

  void PublishRawSmall(
      std::string_view name, std::string_view typeString,
      std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
          supplier) override;

  [[nodiscard]]
  std::function<void(bool)> AddBooleanPublisher(std::string_view name) override;

  [[nodiscard]]
  std::function<void(int64_t)> AddIntegerPublisher(
      std::string_view name) override;

  [[nodiscard]]
  std::function<void(float)> AddFloatPublisher(std::string_view name) override;

  [[nodiscard]]
  std::function<void(double)> AddDoublePublisher(
      std::string_view name) override;

  [[nodiscard]]
  std::function<void(std::string_view)> AddStringPublisher(
      std::string_view name) override;

  [[nodiscard]]
  std::function<void(std::span<const uint8_t>)> AddRawPublisher(
      std::string_view name, std::string_view typeString) override;

  void SubscribeBoolean(std::string_view name,
                        std::function<void(bool)> consumer) override;

  void SubscribeInteger(std::string_view name,
                        std::function<void(int64_t)> consumer) override;

  void SubscribeFloat(std::string_view name,
                      std::function<void(float)> consumer) override;

  void SubscribeDouble(std::string_view name,
                       std::function<void(double)> consumer) override;

  void SubscribeString(std::string_view name,
                       std::function<void(std::string_view)> consumer) override;

  void SubscribeRaw(
      std::string_view name, std::string_view typeString,
      std::function<void(std::span<const uint8_t>)> consumer) override;

  std::shared_ptr<wpi2::SendableTableBackend> CreateSendable(
      std::string_view name,
      std::unique_ptr<wpi2::SendableWrapper> sendable) override;

  std::shared_ptr<wpi2::SendableTableBackend> GetChild(
      std::string_view name) override;

  void SetPublishOptions(std::string_view name,
                         const wpi2::SendableOptions& options) override;

  void SetSubscribeOptions(std::string_view name,
                           const wpi2::SendableOptions& options) override;

  wpi::json GetProperty(std::string_view name,
                        std::string_view propName) const override;

  void SetProperty(std::string_view name, std::string_view propName,
                   const wpi::json& value) override;

  void DeleteProperty(std::string_view name,
                      std::string_view propName) override;

  wpi::json GetProperties(std::string_view name) const override;

  bool SetProperties(std::string_view name,
                     const wpi::json& properties) override;

  void Remove(std::string_view name) override;

  bool IsPublished() const override;

  void Update() override;

  void Clear() override;

  bool HasSchema(std::string_view name) const override;

  void AddSchema(std::string_view name, std::string_view type,
                 std::span<const uint8_t> schema) override;

  void AddSchema(std::string_view name, std::string_view type,
                 std::string_view schema) override;

 private:
  struct EntryData;

  DataLog& m_log;
  std::string m_path;
  mutable wpi::recursive_mutex m_mutex;
  wpi::StringMap<std::shared_ptr<EntryData>> m_entries;
  wpi::StringMap<std::shared_ptr<DataLogSendableTableBackend>> m_tables;
  std::unique_ptr<wpi2::SendableWrapper> m_sendable;

  EntryData& GetOrNew(std::string_view name);
  std::shared_ptr<DataLogSendableTableBackend> GetChildInternal(
      std::string_view name);
};

}  // namespace wpi::log
