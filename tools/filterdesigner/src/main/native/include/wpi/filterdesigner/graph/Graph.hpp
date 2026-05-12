// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"

namespace wpi::filterdesigner {

/**
 * Thin wrapper around ImFlow::ImNodeFlow that adds the bookkeeping the rest of
 * the filter designer needs:
 *
 *   - Stable graph-local int ids on top of ImNodeFlow's pointer-derived UIDs,
 *     so saved files don't depend on memory layout.
 *   - Typed iteration helpers (Nodes/Links) returning FilterDesignerNode*
 *     pointers and (srcId, srcPin, dstId, dstPin) link tuples — ImNodeFlow
 *     keeps links inside InPins and doesn't expose a queryable list.
 *   - Reset/lookup primitives used by the serializer.
 *
 * Construction is ImGui-free; only Update() / the underlying ImNodeFlow draw
 * paths touch ImGui state, so tests can build graphs without a UI context.
 */
class Graph {
 public:
  struct LiveLink {
    int srcId;
    std::string srcPin;
    int dstId;
    std::string dstPin;
  };

  Graph();
  ~Graph();

  Graph(const Graph&) = delete;
  Graph& operator=(const Graph&) = delete;

  /** Draws/updates the underlying ImNodeFlow editor. Call once per frame. */
  void Update();

  /** Direct access to the underlying editor, for popup-callback registration. */
  ImFlow::ImNodeFlow& Editor() { return *m_editor; }

  /**
   * Construct a node of type T, place it on the canvas, and assign the next
   * graph-local id.
   */
  template <typename T, typename... Args>
  std::shared_ptr<T> AddNode(const ImVec2& pos, Args&&... args) {
    static_assert(std::is_base_of_v<FilterDesignerNode, T>,
                  "Graph nodes must derive from FilterDesignerNode");
    auto node = m_editor->addNode<T>(pos, std::forward<Args>(args)...);
    node->SetGraphId(m_nextId++);
    return node;
  }

  /**
   * Same as AddNode, but uses a known id (load path). Bumps m_nextId past
   * @p id so subsequent AddNode calls don't collide.
   */
  template <typename T, typename... Args>
  std::shared_ptr<T> AddNodeWithId(const ImVec2& pos, int id, Args&&... args) {
    static_assert(std::is_base_of_v<FilterDesignerNode, T>,
                  "Graph nodes must derive from FilterDesignerNode");
    auto node = m_editor->addNode<T>(pos, std::forward<Args>(args)...);
    node->SetGraphId(id);
    m_nextId = std::max(m_nextId, id + 1);
    return node;
  }

  /** Live nodes, filtered to FilterDesignerNode subclasses. */
  std::vector<FilterDesignerNode*> Nodes() const;

  /** Live links resolved to graph-local ids + pin names. */
  std::vector<LiveLink> Links() const;

  /** Returns the live node with the given graph id, or nullptr. */
  FilterDesignerNode* FindNodeById(int id) const;

  /**
   * Bumps the next-id counter so that subsequent AddNode calls produce ids
   * strictly greater than @p id. Used by the deserializer after overriding a
   * just-created node's id with the value from disk.
   */
  void BumpNextIdAbove(int id) { m_nextId = std::max(m_nextId, id + 1); }

  /** Drops every node + link. Equivalent to constructing a fresh Graph. */
  void Reset();

 private:
  /** Applies our config (zoom off, etc.) to a freshly-constructed editor. */
  void ConfigureEditor();

  std::unique_ptr<ImFlow::ImNodeFlow> m_editor;
  int m_nextId = 1;
};

}  // namespace wpi::filterdesigner
