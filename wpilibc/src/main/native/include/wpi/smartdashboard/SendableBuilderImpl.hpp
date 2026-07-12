// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NTSendableBuilder.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/util/FunctionExtras.hpp"
#include "wpi/util/SmallVector.hpp"

namespace wpi {

/**
 * Implementation detail for SendableBuilder.
 */
class SendableBuilderImpl : public wpi::nt::NTSendableBuilder {
 public:
  SendableBuilderImpl() = default;
  ~SendableBuilderImpl() override = default;

  SendableBuilderImpl(SendableBuilderImpl&&) = default;
  SendableBuilderImpl& operator=(SendableBuilderImpl&&) = default;

  /**
   * Set the network table.  Must be called prior to any Add* functions being
   * called.
   * @param table Network table
   */
  void SetTable(std::shared_ptr<wpi::nt::NetworkTable> table);

  /**
   * Get the network table.
   * @return The network table
   */
  std::shared_ptr<wpi::nt::NetworkTable> GetTable() override;

  /**
   * Return whether this sendable has an associated table.
   * @return True if it has a table, false if not.
   */
  bool IsPublished() const override;

  /**
   * Return whether this sendable should be treated as an actuator.
   * @return True if actuator, false if not.
   */
  bool IsActuator() const;

  /**
   * Synchronize with network table values by calling the getters for all
   * properties and setters when the network table value has changed.
   */
  void Update() override;

  /**
   * Hook setters for all properties.
   */
  void StartListeners();

  /**
   * Unhook setters for all properties.
   */
  void StopListeners();

  /**
   * Clear properties.
   */
  void ClearProperties() override;

  void SetSmartDashboardType(std::string_view type) override;
  void SetActuator(bool value) override;
  void SetUpdateTable(wpi::util::unique_function<void()> func) override;
  wpi::nt::Topic GetTopic(std::string_view key) override;

  void AddBooleanProperty(std::string_view key, std::function<bool()> getter,
                          std::function<void(bool)> setter) override;

  void PublishConstBoolean(std::string_view key, bool value) override;

  void AddIntegerProperty(std::string_view key, std::function<int64_t()> getter,
                          std::function<void(int64_t)> setter) override;

  void PublishConstInteger(std::string_view key, int64_t value) override;

  void AddFloatProperty(std::string_view key, std::function<float()> getter,
                        std::function<void(float)> setter) override;

  void PublishConstFloat(std::string_view key, float value) override;

  void AddDoubleProperty(std::string_view key, std::function<double()> getter,
                         std::function<void(double)> setter) override;

  void PublishConstDouble(std::string_view key, double value) override;

  void AddStringProperty(std::string_view key,
                         std::function<std::string()> getter,
                         std::function<void(std::string_view)> setter) override;

  void PublishConstString(std::string_view key,
                          std::string_view value) override;

  void AddBooleanArrayProperty(
      std::string_view key, std::function<std::vector<int>()> getter,
      std::function<void(std::span<const int>)> setter) override;

  void PublishConstBooleanArray(std::string_view key,
                                std::span<const int> value) override;

  void AddIntegerArrayProperty(
      std::string_view key, std::function<std::vector<int64_t>()> getter,
      std::function<void(std::span<const int64_t>)> setter) override;

  void PublishConstIntegerArray(std::string_view key,
                                std::span<const int64_t> value) override;

  void AddFloatArrayProperty(
      std::string_view key, std::function<std::vector<float>()> getter,
      std::function<void(std::span<const float>)> setter) override;

  void PublishConstFloatArray(std::string_view key,
                              std::span<const float> value) override;

  void AddDoubleArrayProperty(
      std::string_view key, std::function<std::vector<double>()> getter,
      std::function<void(std::span<const double>)> setter) override;

  void PublishConstDoubleArray(std::string_view key,
                               std::span<const double> value) override;

  void AddStringArrayProperty(
      std::string_view key, std::function<std::vector<std::string>()> getter,
      std::function<void(std::span<const std::string>)> setter) override;

  void PublishConstStringArray(std::string_view key,
                               std::span<const std::string> value) override;

  void AddRawProperty(
      std::string_view key, std::string_view typeString,
      std::function<std::vector<uint8_t>()> getter,
      std::function<void(std::span<const uint8_t>)> setter) override;

  void PublishConstRaw(std::string_view key, std::string_view typeString,
                       std::span<const uint8_t> value) override;

  void AddSmallStringProperty(
      std::string_view key,
      std::function<std::string_view(wpi::util::SmallVectorImpl<char>& buf)>
          getter,
      std::function<void(std::string_view)> setter) override;

  void AddSmallBooleanArrayProperty(
      std::string_view key,
      std::function<std::span<const int>(wpi::util::SmallVectorImpl<int>& buf)>
          getter,
      std::function<void(std::span<const int>)> setter) override;

  void AddSmallIntegerArrayProperty(
      std::string_view key,
      std::function<
          std::span<const int64_t>(wpi::util::SmallVectorImpl<int64_t>& buf)>
          getter,
      std::function<void(std::span<const int64_t>)> setter) override;

  void AddSmallFloatArrayProperty(
      std::string_view key,
      std::function<
          std::span<const float>(wpi::util::SmallVectorImpl<float>& buf)>
          getter,
      std::function<void(std::span<const float>)> setter) override;

  void AddSmallDoubleArrayProperty(
      std::string_view key,
      std::function<
          std::span<const double>(wpi::util::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(std::span<const double>)> setter) override;

  void AddSmallStringArrayProperty(
      std::string_view key,
      std::function<std::span<const std::string>(
          wpi::util::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(std::span<const std::string>)> setter) override;

  void AddSmallRawProperty(
      std::string_view key, std::string_view typeString,
      std::function<
          std::span<uint8_t>(wpi::util::SmallVectorImpl<uint8_t>& buf)>
          getter,
      std::function<void(std::span<const uint8_t>)> setter) override;

 private:
  struct Property {
    virtual ~Property() = default;
    virtual void Update(bool controllable, int64_t time) = 0;
  };

  template <typename Topic>
  struct PropertyImpl : public Property {
    void Update(bool controllable, int64_t time) override;

    using Publisher = typename Topic::PublisherType;
    using Subscriber = typename Topic::SubscriberType;
    Publisher pub;
    Subscriber sub;
    std::function<void(Publisher& pub, int64_t time)> updateNetwork;
    std::function<void(Subscriber& sub)> updateLocal;
  };

  template <typename Topic, typename Getter, typename Setter>
  void AddPropertyImpl(Topic topic, Getter getter, Setter setter);

  template <typename Topic, typename Value>
  void PublishConstImpl(Topic topic, Value value);

  template <typename T, size_t Size, typename Topic, typename Getter,
            typename Setter>
  void AddSmallPropertyImpl(Topic topic, Getter getter, Setter setter);

  std::vector<std::unique_ptr<Property>> m_properties;
  std::vector<wpi::util::unique_function<void()>> m_updateTables;
  std::shared_ptr<wpi::nt::NetworkTable> m_table;
  bool m_controllable = false;
  bool m_actuator = false;

  wpi::nt::BooleanPublisher m_controllablePublisher;
  wpi::nt::StringPublisher m_typePublisher;
  wpi::nt::BooleanPublisher m_actuatorPublisher;
};

}  // namespace wpi
