// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <imgui.h>

namespace wpi::filterdesigner {

/** Font size node metrics are written against: wpigui's FontSizeBase. */
inline constexpr float kBaselineFontSize = 13.0f;

/**
 * Scales a node metric written at @ref kBaselineFontSize to the current font,
 * so node layout tracks the app's zoom rather than pinning to pixels.
 */
inline float ScaleToFont(float size) {
  return size * ImGui::GetFontSize() / kBaselineFontSize;
}

/** Inverse of @ref ScaleToFont, for storing a measured pixel value. */
inline float ScaleFromFont(float pixels) {
  return pixels * kBaselineFontSize / ImGui::GetFontSize();
}

}  // namespace wpi::filterdesigner
