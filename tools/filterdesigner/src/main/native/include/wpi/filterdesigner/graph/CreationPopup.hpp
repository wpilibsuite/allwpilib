// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <imgui.h>

namespace ImFlow {
class Pin;
}  // namespace ImFlow

namespace wpi::filterdesigner {

class Graph;
class NodeRegistry;

/**
 * Single creation popup shared by the three node-creation entry points:
 *
 *   - Right-click on empty canvas (hooked through ImNodeFlow's
 *     `rightClickPopUpContent`).
 *   - "Add Node" in the menu bar (call @ref RequestOpen).
 *   - Drag a wire onto empty canvas (hooked through ImNodeFlow's
 *     `droppedLinkPopUpContent`; the dragged pin filters the menu and the
 *     popup auto-wires the resulting link after the node is created).
 *
 * One implementation, so menu structure and type filtering stay in lockstep.
 */
class CreationPopup {
 public:
  CreationPopup(Graph& graph, const NodeRegistry& registry);

  /**
   * Installs the right-click + drag-to-canvas callbacks on the graph's
   * editor. Call once after construction.
   */
  void Attach();

  /**
   * Queues the "external" (menu-bar) popup to open during the next
   * @ref DrawExternal call. The request comes from the toolbar, above the
   * canvas, so a node it creates lands at the centre of the visible canvas.
   */
  void RequestOpen();

  /**
   * Renders the menu-bar-triggered popup. Must be called every frame inside an
   * ImGui window so its OpenPopup→BeginPopup state machine ticks; ImNodeFlow
   * drives the other two entry points itself.
   */
  void DrawExternal();

 private:
  /**
   * Renders the menu items. Used by all three entry points. When
   * @p draggedPin is non-null the menu is filtered to compatible node
   * types; if the user picks one, the newly-created node is auto-linked to
   * the dragged pin.
   *
   * @return true if a node was created this frame (caller should close the
   *         enclosing popup).
   */
  bool DrawMenuItems(const ImVec2& gridPos, ImFlow::Pin* draggedPin);

  // Only read by the drawing code, which the test build compiles out, so
  // clang's -Wunused-private-field would otherwise fail that build.
  [[maybe_unused]]
  Graph* m_graph;
  [[maybe_unused]]
  const NodeRegistry* m_registry;
  [[maybe_unused]]
  bool m_openExternalRequested = false;
  /**
   * Where a created node goes, in grid coordinates. Captured the frame a
   * popup opens: by the time a menu item is clicked the mouse is on that
   * item, not where the user asked for the node.
   */
  [[maybe_unused]]
  ImVec2 m_gridPos{};
};

}  // namespace wpi::filterdesigner
