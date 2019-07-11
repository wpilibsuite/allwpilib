/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/ArrayRef.h>
#include <wpi/SmallVector.h>
#include <wpi/Twine.h>

#include "frc/smartdashboard/SendableBuilder.h"

namespace frc {

class SendableBuilderImpl : public SendableBuilder {
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
  std::shared_ptr<nt::NetworkTable> GetTable();

  /**
   * Return whether this sendable should be treated as an actuator.
   * @return True if actuator, false if not.
   */
  bool IsActuator() const;

  /**
   * Update the network table values by calling the getters for all properties.
   */
  void UpdateTable();

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

  void SetSmartDashboardType(const wpi::Twine& type) override;
  void SetActuator(bool value) override;
  void SetSafeState(std::function<void()> func) override;
  void SetUpdateTable(std::function<void()> func) override;
  nt::NetworkTableEntry GetEntry(const wpi::Twine& key) override;

  void AddBooleanProperty(const wpi::Twine& key, std::function<bool()> getter,
                          std::function<void(bool)> setter) override;

  void AddDoubleProperty(const wpi::Twine& key, std::function<double()> getter,
                         std::function<void(double)> setter) override;

  void AddStringProperty(const wpi::Twine& key,
                         std::function<std::string()> getter,
                         std::function<void(wpi::StringRef)> setter) override;

  void AddBooleanArrayProperty(
      const wpi::Twine& key, std::function<std::vector<int>()> getter,
      std::function<void(wpi::ArrayRef<int>)> setter) override;

  void AddDoubleArrayProperty(
      const wpi::Twine& key, std::function<std::vector<double>()> getter,
      std::function<void(wpi::ArrayRef<double>)> setter) override;

  void AddStringArrayProperty(
      const wpi::Twine& key, std::function<std::vector<std::string>()> getter,
      std::function<void(wpi::ArrayRef<std::string>)> setter) override;

  void AddRawProperty(const wpi::Twine& key,
                      std::function<std::string()> getter,
                      std::function<void(wpi::StringRef)> setter) override;

  void AddValueProperty(
      const wpi::Twine& key, std::function<std::shared_ptr<nt::Value>()> getter,
      std::function<void(std::shared_ptr<nt::Value>)> setter) override;

  void AddSmallStringProperty(
      const wpi::Twine& key,
      std::function<wpi::StringRef(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(wpi::StringRef)> setter) override;

  void AddSmallBooleanArrayProperty(
      const wpi::Twine& key,
      std::function<wpi::ArrayRef<int>(wpi::SmallVectorImpl<int>& buf)> getter,
      std::function<void(wpi::ArrayRef<int>)> setter) override;

  void AddSmallDoubleArrayProperty(
      const wpi::Twine& key,
      std::function<wpi::ArrayRef<double>(wpi::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(wpi::ArrayRef<double>)> setter) override;

  void AddSmallStringArrayProperty(
      const wpi::Twine& key,
      std::function<
          wpi::ArrayRef<std::string>(wpi::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(wpi::ArrayRef<std::string>)> setter) override;

  void AddSmallRawProperty(
      const wpi::Twine& key,
      std::function<wpi::StringRef(wpi::SmallVectorImpl<char>& buf)> getter,
      std::function<void(wpi::StringRef)> setter) override;

 private:
  struct Property {
    Property(nt::NetworkTable& table, const wpi::Twine& key)
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
      if (entry && listener == 0 && createListener)
        listener = createListener(entry);
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
  std::function<void()> m_updateTable;
  std::shared_ptr<nt::NetworkTable> m_table;
  nt::NetworkTableEntry m_controllableEntry;
  bool m_actuator = false;
};

}  // namespace frc
