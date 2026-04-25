// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

namespace ImFlow {
class ImNodeFlow;
}  // namespace ImFlow

namespace wpi::filterdesigner {

// Milestone 1 spike host for the ImNodeFlow node-graph rewrite. Owns one
// ImNodeFlow editor pre-populated with two debug nodes wired together so we
// can confirm the library renders, links visually, and round-trips a
// type-checked Signal between nodes inside the WPILib build. Lives alongside
// the existing linear-chain views; deletion of those views happens once the
// node taxonomy in docs/nodes-plan.md catches up.
class GraphEditor {
 public:
  GraphEditor();
  ~GraphEditor();

  GraphEditor(const GraphEditor&) = delete;
  GraphEditor& operator=(const GraphEditor&) = delete;

  // Renders the canvas inside the current ImGui window.
  void Display();

 private:
  std::unique_ptr<ImFlow::ImNodeFlow> m_editor;
};

}  // namespace wpi::filterdesigner
