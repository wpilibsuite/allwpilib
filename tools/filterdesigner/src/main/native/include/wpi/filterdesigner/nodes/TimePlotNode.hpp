// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/TimePlotNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Sink node that draws up to four Signal inputs as a time-domain line plot.
 *
 * Pin shape: in0..in3 (const Signal*) → —
 *
 * Each non-null input contributes one ImPlot series. Inputs share the same
 * x-axis (time, in seconds).
 */
class TimePlotNode final : public FilterDesignerNode {
 public:
  static constexpr int kInputCount = 4;

  TimePlotNode();
  ~TimePlotNode() override;

  std::string_view TypeTag() const override { return "TimePlot"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const TimePlotNodeLogic& Logic() const { return *m_logic; }
  TimePlotNodeLogic& Logic() { return *m_logic; }

 private:
  std::unique_ptr<TimePlotNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
