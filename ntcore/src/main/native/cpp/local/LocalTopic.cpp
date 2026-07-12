// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalTopic.hpp"

#include <algorithm>

#include "local/LocalDataLogger.hpp"

using namespace wpi::nt::local;

void LocalTopic::StartStopDataLog(LocalDataLogger* logger, int64_t timestamp,
                                  bool publish) {
  auto it = std::find_if(
      datalogs.begin(), datalogs.end(),
      [&](const auto& elem) { return elem.logger == logger->handle; });
  if (publish && it == datalogs.end()) {
    datalogs.emplace_back(
        logger->log,
        logger->Start(name, typeStr,
                      LocalDataLoggerEntry::MakeMetadata(m_propertiesStr),
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
    auto metadata = LocalDataLoggerEntry::MakeMetadata(m_propertiesStr);
    for (auto&& datalog : datalogs) {
      datalog.SetMetadata(metadata, now);
    }
  }
}

wpi::util::json LocalTopic::SetFlags(unsigned int flags) {
  wpi::util::json update = wpi::util::json::object();
  if ((flags & NT_PERSISTENT) != 0) {
    properties["persistent"] = true;
    update["persistent"] = true;
  } else {
    properties.erase("persistent");
    update["persistent"] = wpi::util::json();
  }
  if ((flags & NT_RETAINED) != 0) {
    properties["retained"] = true;
    update["retained"] = true;
  } else {
    properties.erase("retained");
    update["retained"] = wpi::util::json();
  }
  if ((flags & NT_UNCACHED) != 0) {
    properties["cached"] = false;
    update["cached"] = false;
  } else {
    properties.erase("cached");
    update["cached"] = wpi::util::json();
  }
  if ((flags & NT_UNCACHED) != 0) {
    lastValue = {};
    lastValueNetwork = {};
    lastValueFromNetwork = false;
  }
  this->m_flags = flags;
  return update;
}

wpi::util::json LocalTopic::SetPersistent(bool value) {
  wpi::util::json update = wpi::util::json::object();
  if (value) {
    m_flags |= NT_PERSISTENT;
    properties["persistent"] = true;
    update["persistent"] = true;
  } else {
    m_flags &= ~NT_PERSISTENT;
    properties.erase("persistent");
    update["persistent"] = wpi::util::json();
  }
  return update;
}

wpi::util::json LocalTopic::SetRetained(bool value) {
  wpi::util::json update = wpi::util::json::object();
  if (value) {
    m_flags |= NT_RETAINED;
    properties["retained"] = true;
    update["retained"] = true;
  } else {
    m_flags &= ~NT_RETAINED;
    properties.erase("retained");
    update["retained"] = wpi::util::json();
  }
  return update;
}

wpi::util::json LocalTopic::SetCached(bool value) {
  wpi::util::json update = wpi::util::json::object();
  if (value) {
    m_flags &= ~NT_UNCACHED;
    properties.erase("cached");
    update["cached"] = wpi::util::json();
  } else {
    m_flags |= NT_UNCACHED;
    properties["cached"] = false;
    update["cached"] = false;
  }
  return update;
}

wpi::util::json LocalTopic::SetProperty(std::string_view name,
                                        const wpi::util::json& value) {
  if (value.is_null()) {
    properties.erase(name);
  } else {
    properties[name] = value;
  }
  wpi::util::json update = wpi::util::json::object();
  update[name] = value;
  return update;
}

wpi::util::json LocalTopic::DeleteProperty(std::string_view name) {
  properties.erase(name);
  wpi::util::json update = wpi::util::json::object();
  update[name] = wpi::util::json();
  return update;
}

bool LocalTopic::SetProperties(const wpi::util::json& update) {
  if (!update.is_object()) {
    return false;
  }
  for (auto&& [key, value] : update.get_object()) {
    if (value.is_null()) {
      properties.erase(key);
    } else {
      properties[key] = value;
    }
  }
  return true;
}

void LocalTopic::RefreshProperties(bool updateFlags) {
  if (updateFlags) {
    RefreshFlags();
  }
  m_propertiesStr = properties.to_string();
}

wpi::util::json LocalTopic::CompareProperties(const wpi::util::json& props) {
  wpi::util::json update = wpi::util::json::object();
  // added/changed
  if (!props.is_object()) {
    return update;
  }
  for (auto&& [key, value] : props.get_object()) {
    auto it = properties.lookup(key);
    if (!it || *it != value) {
      update[key] = value;
    }
  }
  // removed
  for (auto&& [key, value] : properties.get_object()) {
    if (!props.contains(key)) {
      update[key] = wpi::util::json();
    }
  }
  return update;
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
  m_flags = 0;
  properties = wpi::util::json::object();
  m_propertiesStr = "{}";
}

void LocalTopic::RefreshFlags() {
  if (auto persistent = properties.lookup("persistent")) {
    if (persistent->is_bool()) {
      if (persistent->get_bool()) {
        m_flags |= NT_PERSISTENT;
      } else {
        m_flags &= ~NT_PERSISTENT;
      }
    }
  }
  if (auto retained = properties.lookup("retained")) {
    if (retained->is_bool()) {
      if (retained->get_bool()) {
        m_flags |= NT_RETAINED;
      } else {
        m_flags &= ~NT_RETAINED;
      }
    }
  }
  if (auto cached = properties.lookup("cached")) {
    if (cached->is_bool()) {
      if (cached->get_bool()) {
        m_flags &= ~NT_UNCACHED;
      } else {
        m_flags |= NT_UNCACHED;
      }
    }
  }

  if ((m_flags & NT_UNCACHED) != 0) {
    lastValue = {};
    lastValueNetwork = {};
    lastValueFromNetwork = false;
  }
}
