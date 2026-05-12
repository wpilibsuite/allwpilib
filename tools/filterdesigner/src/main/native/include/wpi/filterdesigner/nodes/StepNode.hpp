// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/StepNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Source node that emits a unit step Signal. Pairs with BiquadStageNode +
 * TimePlotNode to reproduce the linear-chain step-response view.
 *
 * Pin shape: — → out (const Signal*)
 */
class StepNode final : public FilterDesignerNode {
 public:
  StepNode();
  ~StepNode() override;

  std::string_view TypeTag() const override { return "Step"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const StepNodeLogic& Logic() const { return *m_logic; }
  StepNodeLogic& Logic() { return *m_logic; }

 private:
  std::unique_ptr<StepNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
