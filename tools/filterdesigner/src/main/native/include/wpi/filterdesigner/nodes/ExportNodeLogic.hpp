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
 * Pure (UI-free) state of an @c ExportNode: target language, identifier for the
 * generated class or function, and the robot project root it is written under.
 * Keeps the last @ref ExportFilter result so the node can render it without
 * re-running the export.
 */
class ExportNodeLogic {
 public:
  ExportNodeLogic() = default;

  ExportNodeLogic(const ExportNodeLogic&) = delete;
  ExportNodeLogic& operator=(const ExportNodeLogic&) = delete;

  /** Target language. */
  Language lang = Language::Cpp;

  /** Class / function identifier. Normalized on export. */
  std::string className = "MyFilter";

  /** Absolute path to the WPILib robot project root. Empty = unset. */
  std::string projectRoot;

  /**
   * Runs @ref ExportFilter for @p filter with the current parameters, caching
   * the outcome on @ref lastMessage and @ref lastOk. Returns false, with a
   * message, when @p filter is null or has no sections.
   */
  bool Export(const DesignedFilter* filter);

  /** Status text of the last @ref Export call. Empty before the first call. */
  std::string lastMessage;

  /** Whether the last @ref Export call succeeded. */
  bool lastOk = false;
};

}  // namespace wpi::filterdesigner
