// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/ExportNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace pfd {
class select_folder;
}  // namespace pfd

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Sink node that writes a generated class/function file for the upstream
 * Filter wire into a picked WPILib robot project root. Multi-language
 * export is just multiple ExportNodes on the same Filter wire — the node
 * does not multiplex languages itself.
 *
 * Pin shape: in (const DesignedFilter*) → —
 */
class ExportNode final : public FilterDesignerNode {
 public:
  ExportNode();
  ~ExportNode() override;

  std::string_view TypeTag() const override { return "Export"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const ExportNodeLogic& Logic() const { return *m_logic; }
  ExportNodeLogic& Logic() { return *m_logic; }

 private:
  void PollFolderPicker();

  std::unique_ptr<ExportNodeLogic> m_logic;
  std::unique_ptr<pfd::select_folder> m_rootPicker;
};

}  // namespace wpi::filterdesigner
