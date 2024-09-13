#include "imgui_DroidSans.h"
#include "DroidSans.inc"
ImFont* ImGui::AddFontDroidSans(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
