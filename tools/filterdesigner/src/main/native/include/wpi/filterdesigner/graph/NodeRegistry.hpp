// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"

namespace wpi::filterdesigner {

class Graph;

/**
 * Maps the string type tags used in .fdsgn v2 files to (a) a factory that
 * constructs the corresponding node and (b) UI metadata used by the creation
 * popup. Built once at startup; pure data afterwards.
 */
class NodeRegistry {
 public:
  using Factory =
      std::function<std::shared_ptr<FilterDesignerNode>(Graph&, const ImVec2&)>;

  struct Entry {
    /** Type tag as it appears in saved files (e.g. "WpiLogSource"). */
    std::string tag;
    /** Human-readable name shown in the creation popup (e.g. "WPILOG Source"). */
    std::string menuLabel;
    /** Submenu category (e.g. "Sources"). Empty = top level. */
    std::string menuCategory;
    /**
     * Pin data types the node exposes — used by the creation popup to filter
     * compatible nodes when the user drags a wire onto empty canvas. Compared
     * with the dragged pin's typeid().
     */
    std::vector<std::type_index> inputTypes;
    std::vector<std::type_index> outputTypes;
    /** Builds the node on the graph (and assigns an id). */
    Factory factory;
  };

  /** Registers an entry. Duplicate tags overwrite the previous entry. */
  void Register(Entry entry);

  /** Looks up an entry by its tag, or nullptr if absent. */
  const Entry* FindByTag(std::string_view tag) const;

  /** All registered entries in insertion order. */
  const std::vector<Entry>& All() const { return m_entries; }

 private:
  std::vector<Entry> m_entries;
};

}  // namespace wpi::filterdesigner
