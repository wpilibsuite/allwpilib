#include "imgui_ProggyDotted.h"
#include "ProggyDotted.inc"
ImFont* ImGui::AddFontProggyDotted(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(ProggyDotted_compressed_data, ProggyDotted_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
