// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/CodeGenNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Sink node that emits a ready-to-paste C++/Java/Python snippet for the
 * upstream Filter wire and exposes a "Copy to clipboard" button. Multiple
 * CodeGenNodes attached to the same Filter wire give multi-language export
 * without UI gymnastics.
 *
 * Pin shape: in (const DesignedFilter*) → —
 */
class CodeGenNode final : public FilterDesignerNode {
 public:
  CodeGenNode();
  ~CodeGenNode() override;

  std::string_view TypeTag() const override { return "CodeGen"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const CodeGenNodeLogic& Logic() const { return *m_logic; }
  CodeGenNodeLogic& Logic() { return *m_logic; }

 private:
  std::unique_ptr<CodeGenNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
