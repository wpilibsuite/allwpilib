#include "imgui_FiraCodeRetina.h"
#include "FiraCodeRetina.inc"
ImFont* ImGui::AddFontFiraCodeRetina(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(FiraCodeRetina_compressed_data, FiraCodeRetina_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
