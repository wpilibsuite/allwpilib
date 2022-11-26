// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "imgui_ProggyDotted.h"

#include "ProggyDotted.inc"

ImFont* ImGui::AddFontProggyDotted(ImGuiIO& io, float size_pixels,
                                   const ImFontConfig* font_cfg,
                                   const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(
      ProggyDotted_compressed_data, ProggyDotted_compressed_size, size_pixels,
      font_cfg, glyph_ranges);
}
