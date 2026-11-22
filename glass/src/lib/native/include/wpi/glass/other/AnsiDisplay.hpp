// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <imgui.h>

#include "wpi/glass/Model.hpp"
#include "wpi/glass/View.hpp"

namespace wpi::glass {

/**
 * ANSI display text style.
 */
struct AnsiDisplayStyle {
  bool hasForeground = false;
  ImU32 foreground = 0;
  bool hasBackground = false;
  ImU32 background = 0;
  bool bold = false;
  bool italic = false;
  bool underline = false;
  bool inverse = false;

  bool operator==(const AnsiDisplayStyle& other) const = default;
};

/**
 * Rendered run of same-styled ANSI display cells.
 */
struct AnsiDisplaySegment {
  size_t startColumn = 0;
  size_t columns = 0;
  std::string text;
  AnsiDisplayStyle style;
};

/**
 * Rendered ANSI display line.
 */
struct AnsiDisplayLine {
  size_t columns = 0;
  std::vector<AnsiDisplaySegment> segments;
};

/**
 * Rendered ANSI display snapshot.
 */
struct AnsiDisplaySnapshot {
  std::vector<AnsiDisplayLine> lines;
  size_t maxColumns = 0;
};

/**
 * Model for an ANSI console-style display.
 */
class AnsiDisplayModel : public Model {
 public:
  /**
   * Constructor.
   */
  AnsiDisplayModel();

  /**
   * Destructor.
   */
  ~AnsiDisplayModel() override;

  /**
   * Updates the model.
   */
  void Update() override {}

  /**
   * Returns whether the display exists.
   *
   * @return True
   */
  bool Exists() override { return true; }

  /**
   * Clears the display and resets parser state.
   */
  void Clear();

  /**
   * Appends ANSI text to the display.
   *
   * @param text ANSI text
   */
  void Append(std::string_view text);

  /**
   * Gets an immutable render snapshot of the display.
   *
   * The returned snapshot remains valid after later model updates as long as
   * the shared pointer is retained.
   *
   * @return Shared rendered display snapshot
   */
  std::shared_ptr<const AnsiDisplaySnapshot> GetSnapshot() const;

  /**
   * Gets the display as plain text.
   *
   * @return Plain text display contents
   */
  std::string GetPlainText() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

/**
 * Displays an ANSI console-style display.
 *
 * @param model Display model
 * @param autoScroll True to scroll to the bottom as new lines arrive instead of
 * preserving the current scroll offsets.
 */
void DisplayAnsiDisplay(AnsiDisplayModel* model, bool autoScroll = false);

/**
 * ANSI display view with display settings.
 */
class AnsiDisplayView : public View {
 public:
  /**
   * Constructor.
   *
   * @param model Display model
   */
  explicit AnsiDisplayView(AnsiDisplayModel* model) : m_model{model} {}

  /**
   * Displays the view contents.
   */
  void Display() override;

  /**
   * Displays view settings.
   */
  void Settings() override;

  /**
   * Returns whether this view has settings.
   *
   * @return True
   */
  bool HasSettings() override;

 private:
  AnsiDisplayModel* m_model;
};

}  // namespace wpi::glass
