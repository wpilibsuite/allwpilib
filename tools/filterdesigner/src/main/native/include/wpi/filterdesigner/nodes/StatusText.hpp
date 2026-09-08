// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <imgui.h>

namespace wpi::filterdesigner {

/**
 * Tint for a status line the user has to act on: a design error, a failed
 * load, a bad export.
 */
inline constexpr ImVec4 kStatusErrorColor{1.0f, 0.4f, 0.4f, 1.0f};

/** Tint for a status line worth a look but not blocking. */
inline constexpr ImVec4 kStatusWarnColor{1.0f, 0.8f, 0.3f, 1.0f};

/** Tint for a status line reporting success. */
inline constexpr ImVec4 kStatusOkColor{0.4f, 1.0f, 0.4f, 1.0f};

/**
 * Wrap width for a node's status lines when the node has no better measure of
 * its own content. ImNodeFlow sizes a node to its content, so an unwrapped
 * message — a design error naming both cutoffs, a sampling readout with a gap
 * figure, an export path — stretched the node to the message's length.
 * Wrapping keeps the node the width of its widgets.
 *
 * Nodes that know their content width (a plot's width, the timeline's) pass
 * it instead. Never pass the node's own size: the text would fill it, the
 * node would grow by the pin columns to fit, and the two would chase each
 * other wider every frame.
 */
inline constexpr float kStatusWrapWidth = 300.0f;

/** Draws @p text in @p color, word-wrapped at @p wrapWidth from the cursor. */
inline void DrawStatusText(const ImVec4& color, std::string_view text,
                           float wrapWidth = kStatusWrapWidth) {
  ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + wrapWidth);
  ImGui::PushStyleColor(ImGuiCol_Text, color);
  ImGui::TextUnformatted(text.data(), text.data() + text.size());
  ImGui::PopStyleColor();
  ImGui::PopTextWrapPos();
}

/** @ref DrawStatusText in the theme's disabled-text color. */
inline void DrawStatusTextDisabled(std::string_view text,
                                   float wrapWidth = kStatusWrapWidth) {
  DrawStatusText(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), text,
                 wrapWidth);
}

}  // namespace wpi::filterdesigner
