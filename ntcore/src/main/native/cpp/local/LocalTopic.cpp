// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalTopic.h"

#include <algorithm>

#include "local/LocalDataLogger.h"

using namespace nt::local;

void LocalTopic::StartStopDataLog(LocalDataLogger* logger, int64_t timestamp,
                                  bool publish) {
  auto it = std::find_if(
      datalogs.begin(), datalogs.end(),
      [&](const auto& elem) { return elem.logger == logger->handle; });
  if (publish && it == datalogs.end()) {
    datalogs.emplace_back(
        logger->log,
        logger->Start(name, typeStr,
                      LocalDataLoggerEntry::MakeMetadata(propertiesStr),
                      timestamp),
        logger->handle);
    datalogType = type;
  } else if (!publish && it != datalogs.end()) {
    it->Finish(timestamp);
    datalogType = NT_UNASSIGNED;
    datalogs.erase(it);
  }
}

void LocalTopic::UpdateDataLogProperties() {
  if (!datalogs.empty()) {
    auto now = Now();
    auto metadata = LocalDataLoggerEntry::MakeMetadata(propertiesStr);
    for (auto&& datalog : datalogs) {
      datalog.SetMetadata(metadata, now);
    }
  }
}

void LocalTopic::RefreshProperties(bool updateFlags) {
  if (updateFlags) {
    RefreshFlags();
  }
  propertiesStr = properties.dump();
}

void LocalTopic::ResetIfDoesNotExist() {
  if (Exists()) {
    return;
  }
  lastValue = {};
  lastValueNetwork = {};
  lastValueFromNetwork = false;
  type = NT_UNASSIGNED;
  typeStr.clear();
  flags = 0;
  properties = wpi::json::object();
  propertiesStr = "{}";
}

void LocalTopic::RefreshFlags() {
  auto it = properties.find("persistent");
  if (it != properties.end()) {
    if (auto val = it->get_ptr<bool*>()) {
      if (*val) {
        flags |= NT_PERSISTENT;
      } else {
        flags &= ~NT_PERSISTENT;
      }
    }
  }
  it = properties.find("retained");
  if (it != properties.end()) {
    if (auto val = it->get_ptr<bool*>()) {
      if (*val) {
        flags |= NT_RETAINED;
      } else {
        flags &= ~NT_RETAINED;
      }
    }
  }
  it = properties.find("cached");
  if (it != properties.end()) {
    if (auto val = it->get_ptr<bool*>()) {
      if (*val) {
        flags &= ~NT_UNCACHED;
      } else {
        flags |= NT_UNCACHED;
      }
    }
  }

  if ((flags & NT_UNCACHED) != 0) {
    lastValue = {};
    lastValueNetwork = {};
    lastValueFromNetwork = false;
  }
}
