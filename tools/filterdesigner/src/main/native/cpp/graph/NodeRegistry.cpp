// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/NodeRegistry.hpp"

#include <algorithm>
#include <string_view>
#include <utility>

namespace wpi::filterdesigner {

void NodeRegistry::Register(Entry entry) {
  auto it = std::find_if(m_entries.begin(), m_entries.end(),
                         [&](const Entry& e) { return e.tag == entry.tag; });
  if (it != m_entries.end()) {
    *it = std::move(entry);
  } else {
    m_entries.push_back(std::move(entry));
  }
}

const NodeRegistry::Entry* NodeRegistry::FindByTag(std::string_view tag) const {
  for (const auto& e : m_entries) {
    if (e.tag == tag) {
      return &e;
    }
  }
  return nullptr;
}

}  // namespace wpi::filterdesigner
