// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <functional>
#include <limits>
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
 * Construction is ImGui-free, so tests can build graphs without a UI context.
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
    node->SetGraphId(m_nextId);
    // Saturate rather than wrap. The deserializer caps the ids a file may
    // set well below this, and a graph would have to hold two billion live
    // nodes to climb here on its own, so the ceiling is unreachable in
    // practice — but incrementing past it would be undefined behaviour, and
    // the counter takes whatever a file puts in it.
    if (m_nextId < std::numeric_limits<int>::max()) {
      ++m_nextId;
    }
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
   * Drops every node and link by rebuilding the underlying ImNodeFlow, which
   * also drops every callback registered against it. Callers that depend on
   * those must re-register from @ref SetOnReset.
   */
  void Reset();

  /**
   * Installs a callback that fires after every @ref Reset (including the
   * one triggered from inside the deserializer). Replaces any previously
   * registered callback. Pass an empty function to clear.
   */
  void SetOnReset(std::function<void()> cb) { m_onReset = std::move(cb); }

  /**
   * Per-frame cycle indicator: empty when acyclic, otherwise a formatted cycle
   * path (see @ref FormatCycle). Sinks read it in `draw()` and short-circuit
   * when non-empty, so a cycle surfaces as a banner instead of recursing
   * through pin-pull lambdas.
   */
  const std::string& CycleError() const { return m_cycleError; }

  /**
   * Recomputes @ref CycleError. Called automatically by @ref Update; exposed
   * for tests that build a graph without an ImGui context and want to
   * exercise the cycle path without driving a frame through ImNodeFlow.
   */
  void RecomputeCycleError();

  /**
   * True if the cursor is over any node's rect, so @ref Update can route the
   * wheel to a plot inside that node rather than zooming the grid. Always
   * false in test builds, which have no ImGui frame.
   */
  bool IsAnyNodeHovered();

 private:
  /** Applies our config (zoom range, etc.) to a freshly-constructed editor. */
  void ConfigureEditor();

  /**
   * Carries the host ImGui context's per-context settings into ImNodeFlow's
   * inner one. ImNodeFlow draws every node inside a second context it creates
   * on its first update(); it copies the style struct across each frame and
   * shares the font atlas, but nothing else, so anything installed on the
   * host context alone is missing inside nodes:
   *
   *   - Clipboard handlers. The platform backend only installed its on the
   *     host, so a node's ImGui::SetClipboardText hit ImGui's fallback, on
   *     macOS and Linux a private in-memory buffer: Copy "worked" without
   *     reaching the system clipboard, and paste into a node's text field
   *     read that same buffer.
   *   - The default font. io.FontDefault is per context, and the inner one
   *     falls back to the atlas's first font, so a font picked in the View
   *     menu (or a changed wpigui default) changed the chrome but not the
   *     nodes.
   *
   * No-op in test builds.
   */
  void SyncInnerContext();

  /**
   * Pulls the active ImGui style's colors into ImNodeFlow's grid and per-node
   * style fields so the editor follows the host wpigui theme. Header colors
   * stay at their per-category values; only body, border and grid follow.
   * No-op in test builds.
   */
  void ApplyTheme();

  std::unique_ptr<ImFlow::ImNodeFlow> m_editor;
  std::function<void()> m_onReset;
  std::string m_cycleError;
  int m_nextId = 1;
};

}  // namespace wpi::filterdesigner
