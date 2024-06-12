// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/support/ExtraGuiWidgets.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <wpi/DenseMap.h>

#include "glass/DataSource.h"
#include "glass/support/ExpressionParser.h"

namespace glass {

void DrawLEDSources(const int* values, DataSource** sources, int numValues,
                    int cols, const ImU32* colors, float size, float spacing,
                    const LEDConfig& config) {
  if (numValues == 0 || cols < 1) {
    return;
  }
  if (size == 0) {
    size = ImGui::GetFontSize() / 2.0;
  }
  if (spacing == 0) {
    spacing = ImGui::GetFontSize() / 3.0;
  }

  int rows = (numValues + cols - 1) / cols;
  float inc = size + spacing;

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  const ImVec2 p = ImGui::GetCursorScreenPos();

  float sized2 = size / 2;
  float ystart, yinc;
  if (config.start & 1) {
    // lower
    ystart = p.y + sized2 + inc * (rows - 1);
    yinc = -inc;
  } else {
    // upper
    ystart = p.y + sized2;
    yinc = inc;
  }

  float xstart, xinc;
  if (config.start & 2) {
    // right
    xstart = p.x + sized2 + inc * (cols - 1);
    xinc = -inc;
  } else {
    // left
    xstart = p.x + sized2;
    xinc = inc;
  }

  float x = xstart, y = ystart;
  int rowcol = 1;  // row for row-major, column for column-major
  for (int i = 0; i < numValues; ++i) {
    if (config.order == LEDConfig::RowMajor) {
      if (i >= (rowcol * cols)) {
        ++rowcol;
        if (config.serpentine) {
          x -= xinc;
          xinc = -xinc;
        } else {
          x = xstart;
        }
        y += yinc;
      }
    } else {
      if (i >= (rowcol * rows)) {
        ++rowcol;
        if (config.serpentine) {
          y -= yinc;
          yinc = -yinc;
        } else {
          y = ystart;
        }
        x += xinc;
      }
    }
    if (values[i] > 0) {
      drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size),
                              colors[values[i] - 1]);
    } else if (values[i] < 0) {
      drawList->AddRect(ImVec2(x, y), ImVec2(x + size, y + size),
                        colors[-values[i] - 1], 0.0f, 0, 1.0);
    }
    if (sources) {
      ImGui::SetCursorScreenPos(ImVec2(x - sized2, y - sized2));
      if (sources[i]) {
        ImGui::PushID(i);
        ImGui::Selectable("", false, 0, ImVec2(inc, inc));
        sources[i]->EmitDrag();
        ImGui::PopID();
      } else {
        ImGui::Dummy(ImVec2(inc, inc));
      }
    }
    if (config.order == LEDConfig::RowMajor) {
      x += xinc;
    } else {
      y += yinc;
    }
  }

  if (!sources) {
    ImGui::Dummy(ImVec2(inc * cols, inc * rows));
  }
}

void DrawLEDs(const int* values, int numValues, int cols, const ImU32* colors,
              float size, float spacing, const LEDConfig& config) {
  DrawLEDSources(values, nullptr, numValues, cols, colors, size, spacing,
                 config);
}

bool DeleteButton(ImGuiID id, const ImVec2& pos) {
  ImGuiContext& g = *GImGui;
  ImGuiWindow* window = g.CurrentWindow;

  // We intentionally allow interaction when clipped so that a mechanical
  // Alt,Right,Validate sequence close a window. (this isn't the regular
  // behavior of buttons, but it doesn't affect the user much because navigation
  // tends to keep items visible).
  const ImRect bb(
      pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
  bool is_clipped = !ImGui::ItemAdd(bb, id);

  bool hovered, held;
  bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
  if (is_clipped) {
    return pressed;
  }

  // Render
  ImU32 col =
      ImGui::GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
  ImVec2 center = bb.GetCenter();
  if (hovered) {
    window->DrawList->AddCircleFilled(
        center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f), col, 12);
  }

  ImU32 cross_col = ImGui::GetColorU32(ImGuiCol_Text);
  window->DrawList->AddCircle(center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f),
                              cross_col, 12);
  float cross_extent = g.FontSize * 0.5f * 0.5f - 1.0f;
  center -= ImVec2(0.5f, 0.5f);
  window->DrawList->AddLine(center + ImVec2(+cross_extent, +cross_extent),
                            center + ImVec2(-cross_extent, -cross_extent),
                            cross_col, 1.0f);
  window->DrawList->AddLine(center + ImVec2(+cross_extent, -cross_extent),
                            center + ImVec2(-cross_extent, +cross_extent),
                            cross_col, 1.0f);

  return pressed;
}

bool HeaderDeleteButton(const char* label) {
  ImGuiWindow* window = ImGui::GetCurrentWindow();
  ImGuiContext& g = *GImGui;
  ImGuiLastItemData last_item_backup = g.LastItemData;
  ImGuiID id = window->GetID(label);
  float button_size = g.FontSize;
  float button_x = ImMax(
      g.LastItemData.Rect.Min.x,
      g.LastItemData.Rect.Max.x - g.Style.FramePadding.x * 2.0f - button_size);
  float button_y = g.LastItemData.Rect.Min.y;
  bool rv = DeleteButton(
      window->GetID(reinterpret_cast<void*>(static_cast<intptr_t>(id) + 1)),
      ImVec2(button_x, button_y));
  g.LastItemData = last_item_backup;
  return rv;
}

bool HamburgerButton(const ImGuiID id, const ImVec2 position) {
  const ImGuiStyle& style = ImGui::GetStyle();

  ImGuiWindow* window = ImGui::GetCurrentWindow();

  // Frame padding on both sides, then one character in the middle
  const ImRect bb{
      position, position + ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()) +
                    style.FramePadding * 2.0f};

  ImGui::ItemAdd(bb, id);

  bool hovered, held;
  bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

  const ImU32 bgCol =
      ImGui::GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
  const ImVec2 center = bb.GetCenter();
  if (hovered) {
    window->DrawList->AddCircleFilled(
        center, ImMax(2.0f, ImGui::GetFontSize() * 0.5f + 1.0f), bgCol, 12);
  }

  const ImU32 fgCol = ImGui::GetColorU32(ImGuiCol_Text);

  const float halfLineWidth = ImGui::GetFontSize() * 0.5 * 0.7071;
  const float halfTotalHeight = halfLineWidth * 0.875;
  ImVec2 lineStart = {center.x - halfLineWidth, center.y - halfTotalHeight};
  ImVec2 lineEnd = {center.x + halfLineWidth, center.y - halfTotalHeight};

  ImVec2 increment = {0.0, halfTotalHeight};

  for (int i = 0; i < 3; i++) {
    window->DrawList->AddLine(lineStart, lineEnd, fgCol);

    lineStart += increment;
    lineEnd += increment;
  }

  return pressed;
}

static const int kBufferSize = 256;

struct InputExprState {
  char inputBuffer[kBufferSize];
};

static wpi::DenseMap<int, InputExprState> exprStates;
// Shared string buffer for inactive inputs
static char previewBuffer[kBufferSize];

template <typename V>
bool InputExpr(const char* label, V* v, const char* format,
               ImGuiInputTextFlags flags) {
  int id = ImGui::GetID(label);

  char* inputBuffer;
  bool hasState = exprStates.contains(id);
  if (hasState) {
    InputExprState& state = exprStates[id];
    inputBuffer = state.inputBuffer;
  } else {
    inputBuffer = previewBuffer;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    // Preview stored value
    std::snprintf(inputBuffer, kBufferSize, format, *v);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
  }

  ImGui::InputText(label, inputBuffer, kBufferSize, flags);
  bool active = ImGui::IsItemActive();
  bool changed = ImGui::IsItemDeactivatedAfterEdit();

  if (active || changed) {
    InputExprState& state = exprStates[id];
    if (!hasState) {
      // State was just created, copy in contents of preview buffer
      std::strncpy(state.inputBuffer, previewBuffer, kBufferSize);
    }

    // Attempt to parse current value
    auto result = glass::expression::TryParseExpr<V>(state.inputBuffer);
    if (result) {
      *v = result.value();
    } else if (active) {
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s",
                         result.error().c_str());
    }
  }

  // Don't need the state anymore if not editing
  if (!active) {
    exprStates.erase(id);
  }

  return changed;
}

template bool InputExpr(const char*, int64_t*, const char*,
                        ImGuiInputTextFlags);
template bool InputExpr(const char*, float*, const char*, ImGuiInputTextFlags);
template bool InputExpr(const char*, double*, const char*, ImGuiInputTextFlags);

}  // namespace glass
