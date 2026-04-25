// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/CodeGenView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <imgui.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/gui/portable-file-dialogs.h"

namespace wpi::filterdesigner {

namespace {
constexpr const char* kLangLabels[] = {"C++", "Java", "Python"};
constexpr const char* kClassNameKey = "codegen.className";
constexpr const char* kProjectRootKey = "codegen.projectRoot";
constexpr const char* kDefaultClassName = "MyFilter";
}  // namespace

CodeGenView::CodeGenView() = default;
CodeGenView::~CodeGenView() = default;

void CodeGenView::Regenerate(const Sections& sections) {
  m_code = EmitCode(sections, m_lang, m_varName);
}

void CodeGenView::PollRootPicker(std::string& projectRoot) {
  if (!m_rootPicker || !m_rootPicker->ready(0)) {
    return;
  }
  std::string picked = m_rootPicker->result();
  m_rootPicker.reset();
  if (!picked.empty()) {
    projectRoot = std::move(picked);
  }
}

void CodeGenView::Display(const std::optional<Sections>& sections,
                          uint64_t filterVersion,
                          std::string_view description) {
  // Bind class name and project root to glass storage so they persist across
  // launches. The references stay valid for the program's lifetime —
  // glass::Storage holds values in a map of unique_ptrs that don't relocate.
  auto& storage = wpi::glass::GetStorage();
  std::string& className = storage.GetString(kClassNameKey, kDefaultClassName);
  std::string& projectRoot = storage.GetString(kProjectRootKey);

  PollRootPicker(projectRoot);

  int langIdx = static_cast<int>(m_lang);
  bool langChanged = ImGui::Combo("Language", &langIdx, kLangLabels,
                                  IM_ARRAYSIZE(kLangLabels));
  if (langChanged) {
    m_lang = static_cast<Language>(langIdx);
  }

  char varBuf[128];
  std::snprintf(varBuf, sizeof(varBuf), "%s", m_varName.c_str());
  bool varChanged = ImGui::InputText("Variable", varBuf, sizeof(varBuf));
  if (varChanged) {
    m_varName = varBuf;
    if (m_varName.empty()) {
      m_varName = "filter";
    }
  }

  bool haveSections = sections.has_value() && !sections->empty();

  ImGui::BeginDisabled(!haveSections);
  if (ImGui::Button("Copy to clipboard")) {
    ImGui::SetClipboardText(m_code.c_str());
  }
  ImGui::EndDisabled();

  ImGui::Separator();
  ImGui::TextUnformatted("Export to project");

  char nameBuf[128];
  std::snprintf(nameBuf, sizeof(nameBuf), "%s", className.c_str());
  if (ImGui::InputText("Class name", nameBuf, sizeof(nameBuf))) {
    className = nameBuf;
  }

  // Project root is set exclusively via the native folder picker — the OS
  // dialog always returns an absolute path, so there's no normalization to do
  // beyond what std::filesystem::absolute already gives us.
  if (ImGui::Button("Browse...")) {
    if (!m_rootPicker) {
      m_rootPicker = std::make_unique<pfd::select_folder>(
          "Select WPILib project root",
          projectRoot.empty() ? std::string{} : projectRoot);
    }
  }
  ImGui::SameLine();
  if (projectRoot.empty()) {
    ImGui::TextDisabled("(no project root selected)");
  } else {
    ImGui::TextDisabled("%s", projectRoot.c_str());
  }

  bool classNameValid = IsValidIdentifier(className);
  bool rootGiven = !projectRoot.empty();
  bool canExport = haveSections && classNameValid && rootGiven;

  if (haveSections && (classNameValid && rootGiven)) {
    auto target = ResolveExportPath(std::filesystem::path{projectRoot}, m_lang,
                                    className);
    ImGui::TextDisabled("Will write: %s", target.string().c_str());
  } else if (!classNameValid) {
    ImGui::TextDisabled("Class name must be a valid identifier.");
  } else if (!rootGiven) {
    ImGui::TextDisabled("Set a project root to enable export.");
  }

  ImGui::BeginDisabled(!canExport);
  if (ImGui::Button("Export")) {
    ExportSpec spec;
    spec.description = std::string{description};
    auto result = ExportFilter(*sections, m_lang, className,
                               std::filesystem::path{projectRoot}, spec);
    m_exportOk = result.ok;
    m_exportMessage = std::move(result.message);
  }
  ImGui::EndDisabled();

  if (!m_exportMessage.empty()) {
    ImVec4 col = m_exportOk ? ImVec4{0.4f, 1.0f, 0.4f, 1.0f}
                            : ImVec4{1.0f, 0.4f, 0.4f, 1.0f};
    ImGui::TextColored(col, "%s", m_exportMessage.c_str());
  }

  ImGui::Separator();

  if (!haveSections) {
    ImGui::TextDisabled("No filter to emit.");
    m_havePrevious = false;
    m_code.clear();
    return;
  }

  bool dirty = !m_havePrevious || filterVersion != m_lastFilterVersion ||
               langChanged || varChanged || m_lang != m_lastLang ||
               m_varName != m_lastVarName;
  if (dirty) {
    Regenerate(*sections);
    m_lastFilterVersion = filterVersion;
    m_lastLang = m_lang;
    m_lastVarName = m_varName;
    m_havePrevious = true;
  }

  // Read-only multi-line widget — InputTextMultiline needs a mutable buffer
  // even for ReadOnly mode, so hand it m_code's data directly (size is fixed
  // by the ReadOnly flag).
  ImGui::InputTextMultiline("##code", m_code.data(), m_code.size() + 1,
                            ImVec2{-1, -1}, ImGuiInputTextFlags_ReadOnly);
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
