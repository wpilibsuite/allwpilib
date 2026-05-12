// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/WpiLogSourceNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace pfd {
class open_file;
}  // namespace pfd

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Source node that reads numeric entries out of a WPILOG file and exposes
 * the picked entry on its "out" pin as a `const Signal*`.
 *
 * Pin shape: — → out (const Signal*)
 *
 * Pure state lives in @ref WpiLogSourceNodeLogic; this class is just the
 * ImNodeFlow + ImGui shell.
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

  // Held by unique_ptr so the OutPin behaviour lambda can safely capture a
  // raw pointer — non-copyable, stable address across the node's lifetime.
  std::unique_ptr<WpiLogSourceNodeLogic> m_logic;
  std::unique_ptr<pfd::open_file> m_opener;
};

}  // namespace wpi::filterdesigner
