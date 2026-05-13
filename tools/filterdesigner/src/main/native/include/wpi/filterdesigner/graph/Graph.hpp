// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <functional>
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

  /** Direct access to the underlying editor, for popup-callback registration.
   */
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
    node->SetGraph(this);
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
    node->SetGraph(this);
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

  /**
   * Drops every node + link. Equivalent to constructing a fresh Graph.
   *
   * The underlying ImNodeFlow instance is rebuilt, so any callbacks
   * registered against it (popup attach, drop-link handlers) are dropped on
   * the floor. Callers that depend on those must re-register inside an
   * @ref OnReset callback so the rebind happens atomically with the reset.
   */
  void Reset();

  /**
   * Installs a callback that fires after every @ref Reset (including the
   * one triggered from inside the deserializer). Replaces any previously
   * registered callback. Pass an empty function to clear.
   */
  void SetOnReset(std::function<void()> cb) { m_onReset = std::move(cb); }

  /**
   * Per-frame cycle indicator. Empty when the graph is acyclic; otherwise a
   * formatted cycle path (see @ref FormatCycle), e.g.
   * `"Biquad Stage[3] → Biquad Stage[5] → Biquad Stage[3]"`.
   *
   * Recomputed by @ref Update before each ImNodeFlow draw pass. Sinks read
   * this in their `draw()` body and short-circuit when non-empty so that a
   * user-introduced cycle surfaces as a banner instead of recursing through
   * pin-pull lambdas.
   */
  const std::string& CycleError() const { return m_cycleError; }

  /**
   * Recomputes @ref CycleError. Called automatically by @ref Update; exposed
   * for tests that build a graph without an ImGui context and want to
   * exercise the cycle path without driving a frame through ImNodeFlow.
   */
  void RecomputeCycleError();

  /**
   * True iff the cursor is over any node's rect. Used by @ref Update to
   * route the wheel to a plot inside a hovered node rather than letting
   * the editor zoom the grid. Always false in test builds since the check
   * needs a live ImGui frame.
   */
  bool IsAnyNodeHovered();

 private:
  /** Applies our config (zoom range, etc.) to a freshly-constructed editor. */
  void ConfigureEditor();

  std::unique_ptr<ImFlow::ImNodeFlow> m_editor;
  std::function<void()> m_onReset;
  std::string m_cycleError;
  int m_nextId = 1;
};

}  // namespace wpi::filterdesigner
