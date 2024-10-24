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

wpi::json LocalTopic::SetFlags(unsigned int flags) {
  wpi::json update = wpi::json::object();
  if ((flags & NT_PERSISTENT) != 0) {
    properties["persistent"] = true;
    update["persistent"] = true;
  } else {
    properties.erase("persistent");
    update["persistent"] = wpi::json();
  }
  if ((flags & NT_RETAINED) != 0) {
    properties["retained"] = true;
    update["retained"] = true;
  } else {
    properties.erase("retained");
    update["retained"] = wpi::json();
  }
  if ((flags & NT_UNCACHED) != 0) {
    properties["cached"] = false;
    update["cached"] = false;
  } else {
    properties.erase("cached");
    update["cached"] = wpi::json();
  }
  if ((flags & NT_UNCACHED) != 0) {
    lastValue = {};
    lastValueNetwork = {};
    lastValueFromNetwork = false;
  }
  this->m_flags = flags;
  return update;
}

wpi::json LocalTopic::SetPersistent(bool value) {
  wpi::json update = wpi::json::object();
  if (value) {
    m_flags |= NT_PERSISTENT;
    properties["persistent"] = true;
    update["persistent"] = true;
  } else {
    m_flags &= ~NT_PERSISTENT;
    properties.erase("persistent");
    update["persistent"] = wpi::json();
  }
  return update;
}

wpi::json LocalTopic::SetRetained(bool value) {
  wpi::json update = wpi::json::object();
  if (value) {
    m_flags |= NT_RETAINED;
    properties["retained"] = true;
    update["retained"] = true;
  } else {
    m_flags &= ~NT_RETAINED;
    properties.erase("retained");
    update["retained"] = wpi::json();
  }
  return update;
}

wpi::json LocalTopic::SetCached(bool value) {
  wpi::json update = wpi::json::object();
  if (value) {
    m_flags &= ~NT_UNCACHED;
    properties.erase("cached");
    update["cached"] = wpi::json();
  } else {
    m_flags |= NT_UNCACHED;
    properties["cached"] = false;
    update["cached"] = false;
  }
  return update;
}

wpi::json LocalTopic::SetProperty(std::string_view name,
                                  const wpi::json& value) {
  if (value.is_null()) {
    properties.erase(name);
  } else {
    properties[name] = value;
  }
  wpi::json update = wpi::json::object();
  update[name] = value;
  return update;
}

wpi::json LocalTopic::DeleteProperty(std::string_view name) {
  properties.erase(name);
  wpi::json update = wpi::json::object();
  update[name] = wpi::json();
  return update;
}

bool LocalTopic::SetProperties(const wpi::json& update) {
  if (!update.is_object()) {
    return false;
  }
  for (auto&& change : update.items()) {
    if (change.value().is_null()) {
      properties.erase(change.key());
    } else {
      properties[change.key()] = change.value();
    }
  }
  return true;
}

void LocalTopic::RefreshProperties(bool updateFlags) {
  if (updateFlags) {
    RefreshFlags();
  }
  m_propertiesStr = properties.dump();
}

wpi::json LocalTopic::CompareProperties(const wpi::json props) {
  wpi::json update = wpi::json::object();
  // added/changed
  for (auto&& prop : props.items()) {
    auto it = properties.find(prop.key());
    if (it == properties.end() || *it != prop.value()) {
      update[prop.key()] = prop.value();
    }
  }
  // removed
  for (auto&& prop : properties.items()) {
    if (props.find(prop.key()) == props.end()) {
      update[prop.key()] = wpi::json();
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
  properties = wpi::json::object();
  m_propertiesStr = "{}";
}

void LocalTopic::RefreshFlags() {
  auto it = properties.find("persistent");
  if (it != properties.end()) {
    if (auto val = it->get_ptr<bool*>()) {
      if (*val) {
        m_flags |= NT_PERSISTENT;
      } else {
        m_flags &= ~NT_PERSISTENT;
      }
    }
  }
  it = properties.find("retained");
  if (it != properties.end()) {
    if (auto val = it->get_ptr<bool*>()) {
      if (*val) {
        m_flags |= NT_RETAINED;
      } else {
        m_flags &= ~NT_RETAINED;
      }
    }
  }
  it = properties.find("cached");
  if (it != properties.end()) {
    if (auto val = it->get_ptr<bool*>()) {
      if (*val) {
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
