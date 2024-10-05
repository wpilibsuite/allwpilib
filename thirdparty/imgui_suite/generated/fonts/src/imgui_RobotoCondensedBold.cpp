#include "imgui_RobotoCondensedBold.h"
#include "RobotoCondensedBold.inc"
ImFont* ImGui::AddFontRobotoCondensedBold(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(RobotoCondensedBold_compressed_data, RobotoCondensedBold_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
