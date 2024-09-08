#include "imgui_FontAwesomeRegular.h"
#include "FontAwesomeRegular.inc"
ImFont* ImGui::AddFontFontAwesomeRegular(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeRegular_compressed_data, FontAwesomeRegular_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
