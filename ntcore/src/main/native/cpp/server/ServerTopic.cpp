// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerTopic.h"

#include "Log.h"

using namespace nt::server;

bool ServerTopic::SetProperties(const wpi::json& update) {
  if (!update.is_object()) {
    return false;
  }
  bool updated = false;
  for (auto&& elem : update.items()) {
    if (elem.value().is_null()) {
      properties.erase(elem.key());
    } else {
      properties[elem.key()] = elem.value();
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

  auto persistentIt = properties.find("persistent");
  if (persistentIt != properties.end()) {
    if (auto val = persistentIt->get_ptr<bool*>()) {
      persistent = *val;
    }
  }

  auto retainedIt = properties.find("retained");
  if (retainedIt != properties.end()) {
    if (auto val = retainedIt->get_ptr<bool*>()) {
      retained = *val;
    }
  }

  auto cachedIt = properties.find("cached");
  if (cachedIt != properties.end()) {
    if (auto val = cachedIt->get_ptr<bool*>()) {
      cached = *val;
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
