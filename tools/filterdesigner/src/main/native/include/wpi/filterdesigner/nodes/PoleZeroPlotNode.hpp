// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/PoleZeroPlotNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Sink node that draws poles and zeros on the unit circle for up to four
 * Filter inputs. Each non-null DesignedFilter input contributes one
 * "<label> poles" + "<label> zeros" series.
 *
 * Pin shape: in0..in3 (const DesignedFilter*) → —
 */
class PoleZeroPlotNode final : public FilterDesignerNode {
 public:
  static constexpr int kInputCount = 4;

  PoleZeroPlotNode();
  ~PoleZeroPlotNode() override;

  std::string_view TypeTag() const override { return "PoleZeroPlot"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const PoleZeroPlotNodeLogic& Logic() const { return *m_logic; }
  PoleZeroPlotNodeLogic& Logic() { return *m_logic; }

 private:
  std::unique_ptr<PoleZeroPlotNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
