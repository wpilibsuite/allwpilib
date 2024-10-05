#include "imgui_RobotoRegular.h"
#include "RobotoRegular.inc"
ImFont* ImGui::AddFontRobotoRegular(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(RobotoRegular_compressed_data, RobotoRegular_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
