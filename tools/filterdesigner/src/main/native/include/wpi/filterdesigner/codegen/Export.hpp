// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * Spec metadata embedded as a comment header in exported files.
 *
 * @c description is a multi-line, language-agnostic block (no comment markers)
 * that's prefixed line-by-line with the language's comment syntax during
 * emission.
 */
struct ExportSpec {
  double sampleRate = 0.0;
  std::string description;
};

/** Result of an @ref ExportFilter call. */
struct ExportResult {
  bool ok = false;
  std::string message;
  std::filesystem::path writtenPath;
};

/**
 * @return @c true if @a name is a non-empty C-style identifier
 *         (`[A-Za-z_][A-Za-z0-9_]*`).
 */
bool IsValidIdentifier(std::string_view name);

/**
 * Converts CamelCase / PascalCase to snake_case. Used for Python file
 * and function names so they match PEP 8 even though the user types a
 * single PascalCase class name in the UI.
 */
std::string ToSnakeCase(std::string_view name);

/**
 * Resolves the input to an absolute, normalized path. Returns an empty path
 * if the input is empty.
 */
std::filesystem::path NormalizeProjectRoot(const std::filesystem::path& raw);

/**
 * Resolves the absolute target path for an exported filter file, relative
 * to the picked WPILib robot project root. The root is normalized via
 * @ref NormalizeProjectRoot first.
 *
 *   Java:   <root>/src/main/java/frc/robot/filters/<ClassName>.java
 *   C++:    <root>/src/main/include/filters/<ClassName>.h
 *   Python: <root>/filters/<snake_case>.py
 */
std::filesystem::path ResolveExportPath(
    const std::filesystem::path& projectRoot, Language lang,
    std::string_view className);

/**
 * Builds the full file contents — header comment + class wrapper + body.
 * Pure function, no I/O. Useful for unit testing the emitted code.
 */
std::string BuildExportFileContents(const Sections& sections, Language lang,
                                    std::string_view className,
                                    const ExportSpec& spec);

/**
 * Writes the export file to disk, creating intermediate directories as
 * needed. Silently overwrites an existing file at the target path —
 * the file's "DO NOT EDIT" comment header is the contract for this.
 *
 * @param sections    Combined cascade to emit.
 * @param lang        Target language.
 * @param className   Identifier to use for the class / function name.
 *                    Must satisfy @ref IsValidIdentifier.
 * @param projectRoot Root of the user's WPILib robot project.
 * @param spec        Comment-header metadata.
 *
 * @return Status with a human-readable message and (on success) the
 *         absolute path that was written.
 */
ExportResult ExportFilter(const Sections& sections, Language lang,
                          std::string_view className,
                          const std::filesystem::path& projectRoot,
                          const ExportSpec& spec);

}  // namespace wpi::filterdesigner
