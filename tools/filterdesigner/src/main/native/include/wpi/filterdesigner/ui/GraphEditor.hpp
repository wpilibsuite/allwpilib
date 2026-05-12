// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace pfd {
class open_file;
class save_file;
}  // namespace pfd

namespace wpi::filterdesigner {

class CreationPopup;
class Graph;
class NodeRegistry;

/**
 * Hosts the node-graph canvas: owns the @ref Graph, the @ref NodeRegistry
 * (populated at construction with every node type the tool knows about),
 * and the @ref CreationPopup that drives the three node-creation entry
 * points. Also wires up the .fdsgn v2 file-dialog save/load.
 *
 * Lives alongside the legacy linear-chain views in M2; deletion of those
 * views happens in M8 per the milestone plan.
 */
class GraphEditor {
 public:
  explicit GraphEditor(std::string_view saveDir);
  ~GraphEditor();

  GraphEditor(const GraphEditor&) = delete;
  GraphEditor& operator=(const GraphEditor&) = delete;

  /** Renders the canvas + popups. Must be called inside an ImGui window. */
  void Display();

  /** Routes a menu-bar "Add Node" click into the creation popup. */
  void RequestAddNodeAtMouse();

  /** Opens a system save-file dialog. Saves to that path on confirm. */
  void RequestSave();

  /** Opens a system open-file dialog. Loads from that path on confirm. */
  void RequestLoad();

 private:
  void PollDialogs();

  std::unique_ptr<NodeRegistry> m_registry;
  std::unique_ptr<Graph> m_graph;
  std::unique_ptr<CreationPopup> m_creationPopup;

  std::unique_ptr<pfd::save_file> m_saver;
  std::unique_ptr<pfd::open_file> m_opener;

  std::string m_defaultDir;
  std::string m_lastPath;
  std::string m_status;
};

}  // namespace wpi::filterdesigner
