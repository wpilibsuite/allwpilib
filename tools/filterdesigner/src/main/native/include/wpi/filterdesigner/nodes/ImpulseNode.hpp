// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/ImpulseNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Source node that emits a unit impulse Signal — @c [1, 0, 0, ...] of a
 * user-specified length at a user-specified sample rate. Pairs with
 * BiquadStageNode + TimePlotNode to reproduce the linear-chain
 * impulse-response view.
 *
 * Pin shape: — → out (const Signal*)
 */
class ImpulseNode final : public FilterDesignerNode {
 public:
  ImpulseNode();
  ~ImpulseNode() override;

  std::string_view TypeTag() const override { return "Impulse"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const ImpulseNodeLogic& Logic() const { return *m_logic; }
  ImpulseNodeLogic& Logic() { return *m_logic; }

 private:
  std::unique_ptr<ImpulseNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
