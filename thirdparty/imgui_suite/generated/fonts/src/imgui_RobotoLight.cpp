#include "imgui_RobotoLight.h"
#include "RobotoLight.inc"
ImFont* ImGui::AddFontRobotoLight(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(RobotoLight_compressed_data, RobotoLight_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
