// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>

#include <wpi/SmallVector.h>
#include <wpi/Synchronization.h>
#include <wpi/json.h>

#include "Handle.h"
#include "VectorSet.h"
#include "local/LocalDataLogger.h"
#include "local/LocalDataLoggerEntry.h"
#include "ntcore_cpp.h"

namespace nt::local {

struct LocalDataLogger;
struct LocalEntry;
struct LocalMultiSubscriber;
struct LocalPublisher;
struct LocalSubscriber;

constexpr bool IsSpecial(std::string_view name) {
  return name.empty() ? false : name.front() == '$';
}

struct LocalTopic {
  static constexpr auto kType = Handle::kTopic;

  LocalTopic(NT_Topic handle, std::string_view name)
      : handle{handle}, name{name}, special{IsSpecial(name)} {}

  bool Exists() const { return onNetwork || !localPublishers.empty(); }

  bool IsCached() const { return (m_flags & NT_UNCACHED) == 0; }

  // starts if publish is true, stops if false
  void StartStopDataLog(LocalDataLogger* logger, int64_t timestamp,
                        bool publish);
  void UpdateDataLogProperties();

  unsigned int GetFlags() const { return m_flags; }

  // these return update json
  wpi::json SetFlags(unsigned int flags);
  wpi::json SetPersistent(bool value);
  wpi::json SetRetained(bool value);
  wpi::json SetCached(bool value);
  wpi::json SetProperty(std::string_view name, const wpi::json& value);
  wpi::json DeleteProperty(std::string_view name);

  // returns false if not object
  bool SetProperties(const wpi::json& update);

  void RefreshProperties(bool updateFlags);

  // returns update json
  wpi::json CompareProperties(const wpi::json props);

  TopicInfo GetTopicInfo() const {
    TopicInfo info;
    info.topic = handle;
    info.name = name;
    info.type = type;
    info.type_str = typeStr;
    info.properties = m_propertiesStr;
    return info;
  }

  void ResetIfDoesNotExist();

  // invariants
  wpi::SignalObject<NT_Topic> handle;
  std::string name;
  bool special;

  Value lastValue;  // also stores timestamp
  Value lastValueNetwork;
  NT_Type type{NT_UNASSIGNED};
  std::string typeStr;
  wpi::json properties = wpi::json::object();
  LocalEntry* entry{nullptr};  // cached entry for GetEntry()

  bool onNetwork{false};  // true if there are any remote publishers
  bool lastValueFromNetwork{false};

  wpi::SmallVector<LocalDataLoggerEntry, 1> datalogs;
  NT_Type datalogType{NT_UNASSIGNED};

  VectorSet<LocalPublisher*> localPublishers;
  VectorSet<LocalSubscriber*> localSubscribers;
  VectorSet<LocalMultiSubscriber*> multiSubscribers;
  VectorSet<LocalEntry*> entries;
  VectorSet<NT_Listener> listeners;

 private:
  // update flags from properties
  void RefreshFlags();

  unsigned int m_flags{0};            // for NT3 APIs
  std::string m_propertiesStr{"{}"};  // cached string for GetTopicInfo() et al
};

}  // namespace nt::local
