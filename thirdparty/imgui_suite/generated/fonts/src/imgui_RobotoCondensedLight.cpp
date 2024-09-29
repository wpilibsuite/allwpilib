#include "imgui_RobotoCondensedLight.h"
#include "RobotoCondensedLight.inc"
ImFont* ImGui::AddFontRobotoCondensedLight(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(RobotoCondensedLight_compressed_data, RobotoCondensedLight_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
