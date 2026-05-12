// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "wpi/filterdesigner/codegen/CodeGen.hpp"
#include "wpi/filterdesigner/codegen/Export.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"

namespace pfd {
class select_folder;
}  // namespace pfd

namespace wpi::filterdesigner {

/**
 * ImGui panel that renders the current filter cascade as ready-to-paste
 * C++/Java/Python source (with a Copy button), and supports exporting a
 * full generated class file into the user's WPILib robot project under
 * a conventional @c filters/ subdirectory.
 */
class CodeGenView {
 public:
  // pfd::select_folder is forward-declared, so the implicitly generated dtor
  // would need its complete type at every translation unit that destroys the
  // view. Declare them here and define them in the .cpp.
  CodeGenView();
  ~CodeGenView();

  CodeGenView(const CodeGenView&) = delete;
  CodeGenView& operator=(const CodeGenView&) = delete;

  /**
   * @param sections       Current cascade (nullopt hides the code block).
   * @param filterVersion  Bumped on re-design — used as a cache key so the
   *                       code string is only regenerated when the filter
   *                       changes.
   * @param description    Multi-line, language-agnostic description of the
   *                       cascade (sample rate + per-stage params). Embedded
   *                       as a comment header in exported files.
   */
  void Display(const std::optional<Sections>& sections, uint64_t filterVersion,
               std::string_view description);

 private:
  void Regenerate(const Sections& sections);
  void PollRootPicker(std::string& projectRoot);

  Language m_lang = Language::Cpp;
  std::string m_varName = "filter";
  std::string m_code;

  uint64_t m_lastFilterVersion = 0;
  Language m_lastLang = Language::Cpp;
  std::string m_lastVarName;
  bool m_havePrevious = false;

  // Class name and project root live in glass storage so they persist across
  // launches; see the Display() implementation. Export status is session-only.
  std::string m_exportMessage;
  bool m_exportOk = false;
  std::unique_ptr<pfd::select_folder> m_rootPicker;
};

}  // namespace wpi::filterdesigner
