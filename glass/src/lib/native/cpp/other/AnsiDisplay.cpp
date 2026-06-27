// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/other/AnsiDisplay.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "mrc/AnsiDisplayState.h"

using namespace wpi::glass;

namespace {
constexpr ImU32 MakeColor(int r, int g, int b) {
  return IM_COL32(r, g, b, 255);
}

constexpr ImU32 DEFAULT_FOREGROUND = MakeColor(0xff, 0xff, 0xff);
constexpr ImU32 DEFAULT_BACKGROUND = MakeColor(0x24, 0x24, 0x24);
constexpr std::array<ImU32, 16> ANSI_COLORS = {
    MakeColor(0x00, 0x00, 0x00), MakeColor(0x80, 0x00, 0x00),
    MakeColor(0x00, 0x80, 0x00), MakeColor(0x80, 0x80, 0x00),
    MakeColor(0x00, 0x00, 0x80), MakeColor(0x80, 0x00, 0x80),
    MakeColor(0x00, 0x80, 0x80), MakeColor(0xc0, 0xc0, 0xc0),
    MakeColor(0x80, 0x80, 0x80), MakeColor(0xff, 0x00, 0x00),
    MakeColor(0x00, 0xff, 0x00), MakeColor(0xff, 0xff, 0x00),
    MakeColor(0x00, 0x00, 0xff), MakeColor(0xff, 0x00, 0xff),
    MakeColor(0x00, 0xff, 0xff), MakeColor(0xff, 0xff, 0xff),
};

int ClampColorChannel(int value) {
  return std::clamp(value, 0, 255);
}

ImU32 GetAnsiColor(int index, bool bright) {
  int paletteIndex = (bright ? 8 : 0) + index;
  if (paletteIndex < 0 ||
      paletteIndex >= static_cast<int>(ANSI_COLORS.size())) {
    return DEFAULT_FOREGROUND;
  }

  return ANSI_COLORS[paletteIndex];
}

int ColorCubeChannel(int value) {
  return value == 0 ? 0 : 55 + (value * 40);
}

ImU32 GetAnsi256Color(int index) {
  index = std::clamp(index, 0, 255);
  if (index < 16) {
    return GetAnsiColor(index % 8, index >= 8);
  }

  if (index >= 232) {
    int channel = 8 + ((index - 232) * 10);
    return MakeColor(channel, channel, channel);
  }

  int colorIndex = index - 16;
  int r = ColorCubeChannel(colorIndex / 36);
  int g = ColorCubeChannel((colorIndex / 6) % 6);
  int b = ColorCubeChannel(colorIndex % 6);
  return MakeColor(r, g, b);
}

bool GetColor(const mrc::AnsiDisplayColor& color, ImU32* output) {
  switch (color.ColorKind) {
    case mrc::AnsiDisplayColor::Kind::Default:
      return false;
    case mrc::AnsiDisplayColor::Kind::Basic:
      *output = GetAnsiColor(color.Index, color.Bright);
      return true;
    case mrc::AnsiDisplayColor::Kind::Indexed256:
      *output = GetAnsi256Color(color.Index);
      return true;
    case mrc::AnsiDisplayColor::Kind::Rgb:
      *output = MakeColor(ClampColorChannel(color.Red),
                          ClampColorChannel(color.Green),
                          ClampColorChannel(color.Blue));
      return true;
  }

  return false;
}

AnsiDisplayStyle ConvertStyle(const mrc::AnsiDisplayStyle& style) {
  AnsiDisplayStyle converted;
  converted.hasForeground = GetColor(style.Foreground, &converted.foreground);
  converted.hasBackground = GetColor(style.Background, &converted.background);
  converted.bold = style.Bold;
  converted.italic = style.Italic;
  converted.underline = style.Underline;
  converted.inverse = style.Inverse;
  return converted;
}

std::string BuildPlainText(const std::vector<AnsiDisplayLine>& lines) {
  std::string text;
  size_t textLength = lines.empty() ? 0 : lines.size() - 1;
  for (const auto& line : lines) {
    size_t column = 0;
    for (const auto& segment : line.segments) {
      if (segment.startColumn > column) {
        textLength += segment.startColumn - column;
      }
      textLength += segment.text.size();
      column = segment.startColumn + segment.columns;
    }
  }
  text.reserve(textLength);

  for (size_t row = 0; row < lines.size(); ++row) {
    size_t column = 0;
    for (const auto& segment : lines[row].segments) {
      if (segment.startColumn > column) {
        text.append(segment.startColumn - column, ' ');
      }
      text += segment.text;
      column = segment.startColumn + segment.columns;
    }
    if (row < lines.size() - 1) {
      text += '\n';
    }
  }
  return text;
}

struct ResolvedStyle {
  ImU32 foreground = DEFAULT_FOREGROUND;
  bool hasBackground = false;
  ImU32 background = DEFAULT_BACKGROUND;
  bool bold = false;
  bool underline = false;
};

ResolvedStyle ResolveStyle(const AnsiDisplayStyle& style) {
  ResolvedStyle resolved;
  resolved.bold = style.bold;
  resolved.underline = style.underline;
  if (style.inverse) {
    resolved.foreground =
        style.hasBackground ? style.background : DEFAULT_BACKGROUND;
    resolved.background =
        style.hasForeground ? style.foreground : DEFAULT_FOREGROUND;
    resolved.hasBackground = true;
  } else {
    resolved.foreground =
        style.hasForeground ? style.foreground : DEFAULT_FOREGROUND;
    resolved.background =
        style.hasBackground ? style.background : DEFAULT_BACKGROUND;
    resolved.hasBackground = style.hasBackground;
  }
  return resolved;
}
}  // namespace

struct AnsiDisplayModel::Impl {
  void Invalidate() const {
    cacheValid = false;
    plainTextValid = false;
  }

  void RebuildCache() const {
    if (cacheValid) {
      return;
    }

    class SnapshotBuilder : public mrc::AnsiDisplayRunVisitor {
     public:
      explicit SnapshotBuilder(AnsiDisplaySnapshot& snapshot)
          : m_snapshot{snapshot} {
        m_snapshot.maxColumns = 0;
        for (auto& line : m_snapshot.lines) {
          line.columns = 0;
          line.segments.clear();
        }
      }

      void StartRun(size_t row, size_t column, size_t columns,
                    const mrc::AnsiDisplayStyle& style) override {
        if (m_snapshot.lines.size() <= row) {
          m_snapshot.lines.resize(row + 1);
        }
        m_usedLines = std::max(m_usedLines, row + 1);

        auto& line = m_snapshot.lines[row];
        line.columns = std::max(line.columns, column + columns);
        m_snapshot.maxColumns = std::max(m_snapshot.maxColumns, line.columns);

        m_segment = &line.segments.emplace_back();
        m_segment->startColumn = column;
        m_segment->columns = columns;
        m_segment->text.reserve(columns);
        m_segment->style = ConvertStyle(style);
      }

      void AppendRunText(std::string_view text) override {
        m_segment->text += text;
      }

      void Finish() { m_snapshot.lines.resize(m_usedLines); }

     private:
      AnsiDisplaySnapshot& m_snapshot;
      AnsiDisplaySegment* m_segment = nullptr;
      size_t m_usedLines = 0;
    };

    // RebuildCache() is called while the model mutex is held. Once published
    // below, readers can keep the immutable snapshot alive without the lock.
    auto nextSnapshot = GetWritableSnapshot();
    SnapshotBuilder builder{*nextSnapshot};
    state.VisitRuns(builder);
    builder.Finish();

    snapshot = std::move(nextSnapshot);
    cacheValid = true;
  }

  std::shared_ptr<AnsiDisplaySnapshot> GetWritableSnapshot() const {
    // Reuse the published snapshot only when the model is its sole owner. If a
    // caller still holds a shared snapshot, allocate a separate object instead.
    if (!snapshot || snapshot.use_count() != 1) {
      return std::make_shared<AnsiDisplaySnapshot>();
    }

    // Move the unique snapshot out before rewriting it so Impl::snapshot is
    // never pointing at the object currently being rebuilt.
    return std::move(snapshot);
  }

  mutable std::mutex mutex;
  mrc::AnsiDisplayState state;
  mutable bool cacheValid = false;
  mutable std::shared_ptr<AnsiDisplaySnapshot> snapshot;
  mutable bool plainTextValid = false;
  mutable std::string plainText;
};

AnsiDisplayModel::AnsiDisplayModel() : m_impl{std::make_unique<Impl>()} {}

AnsiDisplayModel::~AnsiDisplayModel() = default;

void AnsiDisplayModel::Clear() {
  std::scoped_lock lock{m_impl->mutex};
  m_impl->state = mrc::AnsiDisplayState{};
  m_impl->Invalidate();
}

void AnsiDisplayModel::Append(std::string_view text) {
  std::scoped_lock lock{m_impl->mutex};
  m_impl->state.Apply(text);
  m_impl->Invalidate();
}

std::shared_ptr<const AnsiDisplaySnapshot> AnsiDisplayModel::GetSnapshot()
    const {
  std::scoped_lock lock{m_impl->mutex};
  m_impl->RebuildCache();
  return m_impl->snapshot;
}

std::string AnsiDisplayModel::GetPlainText() const {
  std::scoped_lock lock{m_impl->mutex};
  m_impl->RebuildCache();
  if (!m_impl->plainTextValid) {
    m_impl->plainText = BuildPlainText(m_impl->snapshot->lines);
    m_impl->plainTextValid = true;
  }
  return m_impl->plainText;
}

void wpi::glass::DisplayAnsiDisplay(AnsiDisplayModel* model, bool autoScroll) {
  if (!model) {
    return;
  }

  auto snapshot = model->GetSnapshot();
  const auto& lines = snapshot->lines;

  float lineHeight = ImGui::GetTextLineHeight();
  float cellWidth = ImGui::CalcTextSize("M").x;
  ImVec2 contentSize{
      static_cast<float>(snapshot->maxColumns) * cellWidth,
      std::max(static_cast<float>(lines.size()) * lineHeight, lineHeight)};
  ImGui::SetNextWindowContentSize(contentSize);

  ImGui::PushStyleColor(ImGuiCol_ChildBg,
                        ImGui::ColorConvertU32ToFloat4(DEFAULT_BACKGROUND));
  if (ImGui::BeginChild("##ansi_display", ImVec2{0, 0}, false,
                        ImGuiWindowFlags_HorizontalScrollbar)) {
    float windowHeight = ImGui::GetWindowHeight();
    float scrollX = ImGui::GetScrollX();
    float scrollY = ImGui::GetScrollY();
    float scrollMaxY = ImGui::GetScrollMaxY();
    bool shouldScroll =
        autoScroll && (scrollMaxY <= 0.0f || scrollY >= scrollMaxY);
    size_t firstLine =
        std::min(lines.size(),
                 static_cast<size_t>(std::max(0.0f, scrollY / lineHeight)));
    size_t lastLine = std::min(
        lines.size(),
        static_cast<size_t>(
            std::max(0.0f, (scrollY + windowHeight) / lineHeight) + 2));

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    for (size_t row = firstLine; row < lastLine; ++row) {
      float y = origin.y + static_cast<float>(row) * lineHeight;
      for (const auto& segment : lines[row].segments) {
        auto style = ResolveStyle(segment.style);
        float x =
            origin.x + static_cast<float>(segment.startColumn) * cellWidth;
        float width = static_cast<float>(segment.columns) * cellWidth;
        ImVec2 pos{x, y};
        if (style.hasBackground) {
          drawList->AddRectFilled(pos, ImVec2{x + width, y + lineHeight},
                                  style.background);
        }
        const char* textBegin = segment.text.data();
        const char* textEnd = textBegin + segment.text.size();
        drawList->AddText(pos, style.foreground, textBegin, textEnd);
        if (style.bold) {
          drawList->AddText(ImVec2{x + 1.0f, y}, style.foreground, textBegin,
                            textEnd);
        }
        if (style.underline) {
          float underlineY = y + lineHeight - 1.0f;
          drawList->AddLine(ImVec2{x, underlineY},
                            ImVec2{x + width, underlineY}, style.foreground);
        }
      }
    }

    ImGui::Dummy(
        ImVec2{std::max(contentSize.x, ImGui::GetContentRegionAvail().x),
               contentSize.y});
    ImGui::SetScrollX(scrollX);
    if (shouldScroll) {
      ImGui::SetScrollHereY(1.0f);
    } else {
      ImGui::SetScrollY(scrollY);
    }
  }
  ImGui::EndChild();
  ImGui::PopStyleColor();
}

void AnsiDisplayView::Display() {
  DisplayAnsiDisplay(m_model);
}

void AnsiDisplayView::Settings() {
  if (ImGui::Selectable("Clear")) {
    m_model->Clear();
  }
  auto snapshot = m_model->GetSnapshot();
  bool hasText = !snapshot->lines.empty();
  if (ImGui::Selectable("Copy to Clipboard", false,
                        hasText ? 0 : ImGuiSelectableFlags_Disabled)) {
    std::string text = m_model->GetPlainText();
    ImGui::SetClipboardText(text.c_str());
  }
}

bool AnsiDisplayView::HasSettings() {
  return true;
}
