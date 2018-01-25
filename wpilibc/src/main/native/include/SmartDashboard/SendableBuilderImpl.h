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

#include <llvm/ArrayRef.h>
#include <llvm/SmallVector.h>
#include <llvm/Twine.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>

#include "SendableBuilder.h"

namespace frc {

class SendableBuilderImpl : public SendableBuilder {
 public:
  SendableBuilderImpl() = default;
  SendableBuilderImpl(const SendableBuilderImpl&) = delete;
  SendableBuilderImpl(SendableBuilderImpl&& other) = default;
  SendableBuilderImpl& operator=(const SendableBuilderImpl&) = delete;
  SendableBuilderImpl& operator=(SendableBuilderImpl&& other) = default;
  ~SendableBuilderImpl() override = default;

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

  void SetSmartDashboardType(const llvm::Twine& type) override;
  void SetSafeState(std::function<void()> func) override;
  void SetUpdateTable(std::function<void()> func) override;
  nt::NetworkTableEntry GetEntry(const llvm::Twine& key) override;

  void AddBooleanProperty(const llvm::Twine& key, std::function<bool()> getter,
                          std::function<void(bool)> setter) override;

  void AddDoubleProperty(const llvm::Twine& key, std::function<double()> getter,
                         std::function<void(double)> setter) override;

  void AddStringProperty(const llvm::Twine& key,
                         std::function<std::string()> getter,
                         std::function<void(llvm::StringRef)> setter) override;

  void AddBooleanArrayProperty(
      const llvm::Twine& key, std::function<std::vector<int>()> getter,
      std::function<void(llvm::ArrayRef<int>)> setter) override;

  void AddDoubleArrayProperty(
      const llvm::Twine& key, std::function<std::vector<double>()> getter,
      std::function<void(llvm::ArrayRef<double>)> setter) override;

  void AddStringArrayProperty(
      const llvm::Twine& key, std::function<std::vector<std::string>()> getter,
      std::function<void(llvm::ArrayRef<std::string>)> setter) override;

  void AddRawProperty(const llvm::Twine& key,
                      std::function<std::string()> getter,
                      std::function<void(llvm::StringRef)> setter) override;

  void AddValueProperty(
      const llvm::Twine& key,
      std::function<std::shared_ptr<nt::Value>()> getter,
      std::function<void(std::shared_ptr<nt::Value>)> setter) override;

  void AddSmallStringProperty(
      const llvm::Twine& key,
      std::function<llvm::StringRef(llvm::SmallVectorImpl<char>& buf)> getter,
      std::function<void(llvm::StringRef)> setter) override;

  void AddSmallBooleanArrayProperty(
      const llvm::Twine& key,
      std::function<llvm::ArrayRef<int>(llvm::SmallVectorImpl<int>& buf)>
          getter,
      std::function<void(llvm::ArrayRef<int>)> setter) override;

  void AddSmallDoubleArrayProperty(
      const llvm::Twine& key,
      std::function<llvm::ArrayRef<double>(llvm::SmallVectorImpl<double>& buf)>
          getter,
      std::function<void(llvm::ArrayRef<double>)> setter) override;

  void AddSmallStringArrayProperty(
      const llvm::Twine& key,
      std::function<
          llvm::ArrayRef<std::string>(llvm::SmallVectorImpl<std::string>& buf)>
          getter,
      std::function<void(llvm::ArrayRef<std::string>)> setter) override;

  void AddSmallRawProperty(
      const llvm::Twine& key,
      std::function<llvm::StringRef(llvm::SmallVectorImpl<char>& buf)> getter,
      std::function<void(llvm::StringRef)> setter) override;

 private:
  struct Property {
    Property(nt::NetworkTable& table, const llvm::Twine& key)
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
};

}  // namespace frc
