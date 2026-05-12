// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Single-stage filter design node. Designs one biquad cascade from the
 * familiar (kind, family, order, cutoffs) parameters and exposes it on two
 * output pins:
 *
 *   - "filter" (const DesignedFilter*) — the cascade + the fs it was
 *     designed at, for downstream sinks (Bode, code-gen, etc.).
 *   - "signal" (const Signal*) — the optional input Signal filtered through
 *     the cascade. Lets chains like `Source → Stage → Stage → TimePlot`
 *     work without ever wiring the filter pins.
 *
 * Pin shape: in (const Signal*) → out filter, out signal
 *
 * Pure state lives in @ref BiquadStageNodeLogic; this class is just the
 * ImNodeFlow + ImGui shell.
 */
class BiquadStageNode final : public FilterDesignerNode {
 public:
  BiquadStageNode();
  ~BiquadStageNode() override;

  std::string_view TypeTag() const override { return "BiquadStage"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const BiquadStageNodeLogic& Logic() const { return *m_logic; }
  BiquadStageNodeLogic& Logic() { return *m_logic; }

 private:
  // Held by unique_ptr so the OutPin behaviour lambdas can safely capture a
  // raw pointer — non-copyable, stable address across the node's lifetime.
  std::unique_ptr<BiquadStageNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
