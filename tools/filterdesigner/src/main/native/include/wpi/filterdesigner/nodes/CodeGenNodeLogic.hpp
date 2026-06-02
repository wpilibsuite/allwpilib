// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"

namespace wpi::filterdesigner {

struct DesignedFilter;

/**
 * Pure (UI-free) configuration for a @c CodeGenNode: target language and
 * variable identifier. The emitted code is recomputed on demand from the
 * connected DesignedFilter via @ref Generate; the node calls it once per
 * frame so the displayed text always matches the upstream cascade.
 */
class CodeGenNodeLogic {
 public:
  CodeGenNodeLogic() = default;

  CodeGenNodeLogic(const CodeGenNodeLogic&) = delete;
  CodeGenNodeLogic& operator=(const CodeGenNodeLogic&) = delete;

  /** Target language for the emitted snippet. */
  Language lang = Language::Cpp;

  /** Identifier used for the emitted variable / declaration. */
  std::string varName = "filter";

  /**
   * Re-emits the snippet for @p filter using the current @ref lang and
   * @ref varName. Returns an empty string if @p filter is null or has no
   * sections. Pure — no I/O.
   */
  std::string Generate(const DesignedFilter* filter) const;
};

}  // namespace wpi::filterdesigner
