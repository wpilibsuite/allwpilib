// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <wpi/SmallVector.h>
#include <wpi/Synchronization.h>
#include <wpi/json.h>

#include "Handle.h"
#include "VectorSet.h"
#include "local/LocalDataLoggerEntry.h"
#include "ntcore_cpp.h"

namespace nt::local {

struct EntryData;
struct MultiSubscriberData;
struct PublisherData;
struct SubscriberData;

constexpr bool IsSpecial(std::string_view name) {
  return name.empty() ? false : name.front() == '$';
}

struct TopicData {
  static constexpr auto kType = Handle::kTopic;

  TopicData(NT_Topic handle, std::string_view name)
      : handle{handle}, name{name}, special{IsSpecial(name)} {}

  bool Exists() const { return onNetwork || !localPublishers.empty(); }

  bool IsCached() const { return (flags & NT_UNCACHED) == 0; }

  TopicInfo GetTopicInfo() const {
    TopicInfo info;
    info.topic = handle;
    info.name = name;
    info.type = type;
    info.type_str = typeStr;
    info.properties = propertiesStr;
    return info;
  }

  // invariants
  wpi::SignalObject<NT_Topic> handle;
  std::string name;
  bool special;

  Value lastValue;  // also stores timestamp
  Value lastValueNetwork;
  NT_Type type{NT_UNASSIGNED};
  std::string typeStr;
  unsigned int flags{0};            // for NT3 APIs
  std::string propertiesStr{"{}"};  // cached string for GetTopicInfo() et al
  wpi::json properties = wpi::json::object();
  EntryData* entry{nullptr};  // cached entry for GetEntry()

  bool onNetwork{false};  // true if there are any remote publishers
  bool lastValueFromNetwork{false};

  wpi::SmallVector<DataLoggerEntry, 1> datalogs;
  NT_Type datalogType{NT_UNASSIGNED};

  VectorSet<PublisherData*> localPublishers;
  VectorSet<SubscriberData*> localSubscribers;
  VectorSet<MultiSubscriberData*> multiSubscribers;
  VectorSet<EntryData*> entries;
  VectorSet<NT_Listener> listeners;
};

}  // namespace nt::local
