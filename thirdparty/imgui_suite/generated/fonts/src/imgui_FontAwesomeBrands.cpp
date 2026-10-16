#include "imgui_FontAwesomeBrands.h"
#include "FontAwesomeBrands.inc"
ImFont* ImGui::AddFontFontAwesomeBrands(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeBrands_compressed_data, FontAwesomeBrands_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
