// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/FrequencyPlotNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Sink node that draws single-sided Hann-windowed magnitude spectra for up
 * to four Signal inputs. Each non-null connected Signal becomes one curve.
 * Per-input sample rate is taken from the Signal — different inputs may
 * carry different rates and the x-axis spans the union.
 *
 * Pin shape: in0..in3 (const Signal*) → —
 */
class FrequencyPlotNode final : public FilterDesignerNode {
 public:
  static constexpr int kInputCount = 4;

  FrequencyPlotNode();
  ~FrequencyPlotNode() override;

  std::string_view TypeTag() const override { return "FrequencyPlot"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const FrequencyPlotNodeLogic& Logic() const { return *m_logic; }
  FrequencyPlotNodeLogic& Logic() { return *m_logic; }

 private:
  std::unique_ptr<FrequencyPlotNodeLogic> m_logic;
};

}  // namespace wpi::filterdesigner
