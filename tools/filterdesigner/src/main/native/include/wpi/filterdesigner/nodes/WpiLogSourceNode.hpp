// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/nodes/WpiLogSourceNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace pfd {
class open_file;
}  // namespace pfd

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Source node that reads numeric entries out of a WPILOG file and exposes
 * the picked entry, restricted to a user-chosen time window, on its "out" pin
 * as a `const Signal*`.
 *
 * Pin shape: — → out (const Signal*)
 *
 * Pure state lives in @ref WpiLogSourceNodeLogic; this class is the ImNodeFlow
 * and ImGui shell, plus the in-flight marker drag, which commits to the logic
 * on mouse release rather than per frame.
 */
class WpiLogSourceNode final : public FilterDesignerNode {
 public:
  WpiLogSourceNode();
  ~WpiLogSourceNode() override;

  std::string_view TypeTag() const override { return "WpiLogSource"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  /** Registers this node type with the @p registry. */
  static void Register(NodeRegistry& registry);

  const WpiLogSourceNodeLogic& Logic() const { return *m_logic; }
  WpiLogSourceNodeLogic& Logic() { return *m_logic; }

 private:
  void PollFileDialog();
  /** Everything the node draws, wrapped by @ref draw so its width can be
   * measured. */
  void DrawBody();
  /** Draws the segment-shaded timeline strip and its two markers. */
  void DrawTimeline();

  // Held by unique_ptr so the OutPin behaviour lambda can safely capture a
  // raw pointer — non-copyable, stable address across the node's lifetime.
  std::unique_ptr<WpiLogSourceNodeLogic> m_logic;
  std::unique_ptr<pfd::open_file> m_opener;

  // The fields below only matter to draw, which the test build compiles
  // out, so clang's -Wunused-private-field sees the trivially-typed ones as
  // dead there.

  /** Live query in the entry picker. Owned here rather than by the logic:
   * it filters what the popup shows and never touches what is published. */
  std::string m_entrySearch;
  /** Where the markers are right now — the logic's window except mid-drag. */
  TimeRange m_pending;
  /** True while a marker or a new-window drag is live, so the window commits
   * once on release. */
  [[maybe_unused]]
  bool m_dragging = false;
  /** True while a left-drag across open strip is drawing a new window. */
  [[maybe_unused]]
  bool m_selecting = false;
  /** Where that drag started, in seconds. The other edge is the cursor, so
   * one gesture names both and neither is ever the ambiguous one. */
  [[maybe_unused]]
  double m_selectAnchor = 0.0;
  /** Record span the timeline's x-axis was last fitted to. Refitting when it
   * changes is what keeps picking a differently-timed entry from leaving the
   * view somewhere the new data isn't. */
  TimeRange m_fittedSpan;
  /** Width of everything the node drew last frame. The timeline stretches to
   * it, a node's width not being known until after @ref draw returns. It
   * settles in two frames: the strip matches the widest row, never widens
   * past it. */
  [[maybe_unused]]
  float m_contentWidth = 0.0f;
};

}  // namespace wpi::filterdesigner
