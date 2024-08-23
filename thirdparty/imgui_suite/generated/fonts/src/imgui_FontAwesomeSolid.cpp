#include "imgui_FontAwesomeSolid.h"
#include "FontAwesomeSolid.inc"
ImFont* ImGui::AddFontFontAwesomeSolid(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid_compressed_data, FontAwesomeSolid_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
