// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

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
  explicit GraphEditor(std::string_view saveDir);
  ~GraphEditor();

  GraphEditor(const GraphEditor&) = delete;
  GraphEditor& operator=(const GraphEditor&) = delete;

  // Renders the canvas inside the current ImGui window.
  void Display();

 private:
  void PopulateDefaultGraph();
  void SaveToDisk();
  void LoadFromDisk();

  std::unique_ptr<ImFlow::ImNodeFlow> m_editor;
  std::string m_savePath;
  std::string m_status;
};

}  // namespace wpi::filterdesigner
