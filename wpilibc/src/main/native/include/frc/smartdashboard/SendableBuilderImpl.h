// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <networktables/NTSendableBuilder.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/SmallVector.h>
#include <wpi/span.h>

namespace frc {

class SendableBuilderImpl : public nt::NTSendableBuilder {
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
  void SetTable(std::shared_ptr<nt::NetworkTable> table);

  /**
   * Get the network table.
   * @return The network table
   */
  std::shared_ptr<nt::NetworkTable> GetTable() override;

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
   * Update the network table values by calling the getters for all properties.
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
   * Start LiveWindow mode by hooking the setters for all properties.  Also
   * calls the SafeState function if one was provided.
   */
  void StartLiveWindowMode();

  /**
   * Stop LiveWindow mode by unhooking the setters for all properties.  Also
   * calls the SafeState function if one was provided.
   */
  void StopLiveWindowMode();

  /**
   * Clear properties.
   */
  void ClearProperties() override;

  void SetSmartDashboardType(std::string_view type) override;
  void SetActuator(bool value) override;
  void SetSafeState(std::function<void()> func) override;
  void SetUpdateTable(std::function<void()> func) override;
  nt::NetworkTableEntry GetEntry(std::string_view key) override;

  void AddBooleanProperty(std::string_view key, std::function<bool()> getter,
                          std::function<void(bool)> setter) override;

  void AddDoubleProperty(std::string_view key, std::function<double()> getter,
                         std::function<void(double)> setter) override;

  void AddStringProperty(std::string_view key,
                         std::function<std::string()> getter,
                         std::function<void(std::string_view)> setter) override;

  void AddBooleanArrayProperty(
      std::string_view key, std::function<std::vector<int>()> getter,
      std::function<void(wpi::span<const int>)> setter) override;

  void AddDoubleArrayProperty(
      std::string_view key, std::function<std::vector<double>()> getter,
      std::function<void(wpi::span<const double>)> setter) override;

  void AddStringArrayProperty(
      std::string_view key, std::function<std::vector<std::string>()> getter,
      std::function<void(wpi::span<const std::string>)> setter) override;

  void AddRawProperty(std::string_view key, std::function<std::string()> getter,
                      std::function<void(std::string_view)> setter) override;

  void AddValueProperty(
      std::string_view key, std::function<std::shared_ptr<nt::Value>()> getter,
      std::function<void(std::shared_ptr<nt::Value>)> setter) override;

  void AddSmallStringProperty(
      std::string_view key,
      std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(std::string_view)> setter) override;

  void AddSmallBooleanArrayProperty(
      std::string_view key,
      std::function<wpi::span<const int>(wpi::SmallVectorImpl<int>& buf)>
          getter,
      std::function<void(wpi::span<const int>)> setter) override;

  void AddSmallDoubleArrayProperty(
      std::string_view key,
      std::function<wpi::span<const double>(wpi::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(wpi::span<const double>)> setter) override;

  void AddSmallStringArrayProperty(
      std::string_view key,
      std::function<
          wpi::span<const std::string>(wpi::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(wpi::span<const std::string>)> setter) override;

  void AddSmallRawProperty(
      std::string_view key,
      std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(std::string_view)> setter) override;

 private:
  struct Property {
    Property(nt::NetworkTable& table, std::string_view key)
        : entry(table.GetEntry(key)) {}

    Property(const Property&) = delete;
    Property& operator=(const Property&) = delete;

    Property(Property&& other) noexcept
        : entry(other.entry),
          listener(other.listener),
          update(std::move(other.update)),
          createListener(std::move(other.createListener)) {
      other.entry = nt::NetworkTableEntry();
      other.listener = 0;
    }

    Property& operator=(Property&& other) noexcept {
      entry = other.entry;
      listener = other.listener;
      other.entry = nt::NetworkTableEntry();
      other.listener = 0;
      update = std::move(other.update);
      createListener = std::move(other.createListener);
      return *this;
    }

    ~Property() { StopListener(); }

    void StartListener() {
      if (entry && listener == 0 && createListener) {
        listener = createListener(entry);
      }
    }

    void StopListener() {
      if (entry && listener != 0) {
        entry.RemoveListener(listener);
        listener = 0;
      }
    }

    nt::NetworkTableEntry entry;
    NT_EntryListener listener = 0;
    std::function<void(nt::NetworkTableEntry entry, uint64_t time)> update;
    std::function<NT_EntryListener(nt::NetworkTableEntry entry)> createListener;
  };

  std::vector<Property> m_properties;
  std::function<void()> m_safeState;
  std::vector<std::function<void()>> m_updateTables;
  std::shared_ptr<nt::NetworkTable> m_table;
  nt::NetworkTableEntry m_controllableEntry;
  bool m_actuator = false;
};

}  // namespace frc
