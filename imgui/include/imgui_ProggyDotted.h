// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include "imgui.h"

namespace ImGui {
ImFont* AddFontProggyDotted(ImGuiIO& io, float size_pixels,
                            const ImFontConfig* font_cfg = nullptr,
                            const ImWchar* glyph_ranges = nullptr);
}  // namespace ImGui
