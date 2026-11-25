// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/BodePlotNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Sink node that draws magnitude + phase response for up to four Filter
 * inputs. Each non-null DesignedFilter input becomes one curve on each
 * subplot. Each filter uses its own carried sample rate, so a single Bode
 * plot can compare filters designed at different rates.
 *
 * Pin shape: in0..in3 (const DesignedFilter*) → —
 */
class BodePlotNode final : public FilterDesignerNode {
 public:
  static constexpr int kInputCount = 4;

  BodePlotNode();
  ~BodePlotNode() override;

  std::string_view TypeTag() const override { return "BodePlot"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const BodePlotNodeLogic& Logic() const { return *m_logic; }
  BodePlotNodeLogic& Logic() { return *m_logic; }

 private:
  std::unique_ptr<BodePlotNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
