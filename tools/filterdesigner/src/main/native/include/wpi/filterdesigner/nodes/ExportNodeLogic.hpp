// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/codegen/Export.hpp"

namespace wpi::filterdesigner {

struct DesignedFilter;

/**
 * Pure (UI-free) state of an @c ExportNode: target language, identifier for
 * the generated class/function, and the WPILib robot project root the
 * generated file is written under. Holds the last @ref ExportFilter result
 * (status + message) so the node can render it without re-running the
 * export.
 */
class ExportNodeLogic {
 public:
  ExportNodeLogic() = default;

  ExportNodeLogic(const ExportNodeLogic&) = delete;
  ExportNodeLogic& operator=(const ExportNodeLogic&) = delete;

  /** Target language. */
  Language lang = Language::Cpp;

  /** Class / function identifier. Must satisfy @ref IsValidIdentifier. */
  std::string className = "MyFilter";

  /** Absolute path to the WPILib robot project root. Empty = unset. */
  std::string projectRoot;

  /**
   * Runs @ref ExportFilter for @p filter using the current parameters and
   * caches the result message on @ref lastMessage / @ref lastOk. Returns
   * @c false (and sets a "no filter" message) when @p filter is null or has
   * no sections. The file comment header gets a best-effort description
   * derived from the filter (sample rate + section count).
   */
  bool Export(const DesignedFilter* filter);

  /** Status text of the last @ref Export call. Empty before the first call. */
  std::string lastMessage;

  /** Whether the last @ref Export call succeeded. */
  bool lastOk = false;
};

}  // namespace wpi::filterdesigner
