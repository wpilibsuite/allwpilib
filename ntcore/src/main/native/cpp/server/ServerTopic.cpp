// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerTopic.hpp"

#include "Log.hpp"

using namespace wpi::nt::server;

bool ServerTopic::SetProperties(const wpi::util::json& update) {
  if (!update.is_object()) {
    return false;
  }
  bool updated = false;
  for (auto&& [key, value] : update.get_object()) {
    if (value.is_null()) {
      properties.erase(key);
    } else {
      properties[key] = value;
    }
    updated = true;
  }
  if (updated) {
    RefreshProperties();
  }
  return updated;
}

void ServerTopic::RefreshProperties() {
  persistent = false;
  retained = false;
  cached = true;

  if (auto prop = properties.lookup("persistent")) {
    if (prop->is_bool()) {
      persistent = prop->get_bool();
    }
  }

  if (auto prop = properties.lookup("retained")) {
    if (prop->is_bool()) {
      retained = prop->get_bool();
    }
  }

  if (auto prop = properties.lookup("cached")) {
    if (prop->is_bool()) {
      cached = prop->get_bool();
    }
  }

  if (!cached) {
    lastValue = {};
    lastValueClient = nullptr;
  }

  if (!cached && persistent) {
    WARN("topic {}: disabling cached property disables persistent storage",
         name);
  }
}

bool ServerTopic::SetFlags(unsigned int flags_) {
  bool updated;
  if ((flags_ & NT_PERSISTENT) != 0) {
    updated = !persistent;
    persistent = true;
    properties["persistent"] = true;
  } else {
    updated = persistent;
    persistent = false;
    properties.erase("persistent");
  }
  if ((flags_ & NT_RETAINED) != 0) {
    updated |= !retained;
    retained = true;
    properties["retained"] = true;
  } else {
    updated |= retained;
    retained = false;
    properties.erase("retained");
  }
  if ((flags_ & NT_UNCACHED) != 0) {
    updated |= cached;
    cached = false;
    properties["cached"] = false;
    lastValue = {};
    lastValueClient = nullptr;
  } else {
    updated |= !cached;
    cached = true;
    properties.erase("cached");
  }
  if (!cached && persistent) {
    WARN("topic {}: disabling cached property disables persistent storage",
         name);
  }
  return updated;
}
