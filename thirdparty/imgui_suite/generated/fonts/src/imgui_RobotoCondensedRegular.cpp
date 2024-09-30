#include "imgui_RobotoCondensedRegular.h"
#include "RobotoCondensedRegular.inc"
ImFont* ImGui::AddFontRobotoCondensedRegular(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(RobotoCondensedRegular_compressed_data, RobotoCondensedRegular_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
