/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PlotGui.h"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <hal/simulation/MockHooks.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <wpi/Signal.h>
#include <wpi/SmallVector.h>
#include <wpi/timestamp.h>
#include <wpi/raw_ostream.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaverInfo.h"
#include "IniSaverVector.h"

using namespace halsimgui;

namespace {
struct PlotSeriesRef {
  size_t plotIndex;
  size_t seriesIndex;
};

class PlotSeries : public NameInfo, public OpenInfo {
 public:
  explicit PlotSeries(wpi::StringRef id);
  explicit PlotSeries(GuiDataSource* source, int yAxis = 0);

  const std::string& GetId() const { return m_id; }

  void CheckSource();

  void SetSource(GuiDataSource* source);
  GuiDataSource* GetSource() const { return m_source; }

  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out);

  bool EmitPlot(double now, size_t i, size_t plotIndex);
  bool EmitSettings(size_t i, size_t plotIndex);
  bool EmitSettingsDetail(size_t i);
  void EmitDragDropPayload(size_t i, size_t plotIndex);

  void GetLabel(char* buf, size_t size) const;

  int GetYAxis() const { return m_yAxis; }
  void SetYAxis(int yAxis) { m_yAxis = yAxis; }

 private:
  bool IsDigital() const {
    return m_digital == kDigital ||
           (m_digital == kAuto && m_source && m_source->IsDigital());
  }
  void AppendValue(double value);

  // source linkage
  GuiDataSource* m_source = nullptr;
  wpi::sig::ScopedConnection m_sourceCreatedConn;
  wpi::sig::ScopedConnection m_newValueConn;
  std::string m_id;

  // user settings
  int m_yAxis = 0;
  ImVec4 m_color = IMPLOT_AUTO_COL;
  int m_marker = 0;

  enum Digital { kAuto, kDigital, kAnalog };
  int m_digital = 0;
  int m_digitalBitHeight = 8;
  int m_digitalBitGap = 4;

  // value storage
  static constexpr int kMaxSize = 2000;
  static constexpr double kTimeGap = 0.05;
  std::atomic<int> m_size = 0;
  std::atomic<int> m_offset = 0;
  ImPlotPoint m_data[kMaxSize];
};

class Plot : public NameInfo, public OpenInfo {
 public:
  Plot();
  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out);

  void GetLabel(char* buf, size_t size, int index) const;
  void GetName(char* buf, size_t size, int index) const;

  void DragDropTarget(size_t i, bool inPlot);
  void EmitPlot(double now, size_t i);
  void EmitSettings(size_t i);

  std::vector<std::unique_ptr<PlotSeries>> m_series;

 private:
  void EmitSettingsLimits(int axis);

  bool m_visible = true;
  unsigned int m_plotFlags = ImPlotFlags_Default;
  bool m_lockPrevX = false;
  bool m_paused = false;
  float m_viewTime = 10;
  int m_height = 300;
  struct PlotRange {
    double min = 0;
    double max = 1;
    bool lockMin = false;
    bool lockMax = false;
    bool apply = false;
  };
  PlotRange m_axisRange[3];
  ImPlotRange m_xaxisRange;  // read from plot, used for lockPrevX
};
}  // namespace

static IniSaverVector<Plot> gPlots{"Plot"};

PlotSeries::PlotSeries(wpi::StringRef id) : m_id(id) {
  if (GuiDataSource* source = GuiDataSource::Find(id)) {
    SetSource(source);
    return;
  }
  CheckSource();
}

PlotSeries::PlotSeries(GuiDataSource* source, int yAxis) : m_yAxis(yAxis) {
  SetSource(source);
}

void PlotSeries::CheckSource() {
  if (!m_newValueConn.connected() && !m_sourceCreatedConn.connected()) {
    m_source = nullptr;
    m_sourceCreatedConn = GuiDataSource::sourceCreated.connect_connection(
        [this](const char* id, GuiDataSource* source) {
          if (m_id == id) {
            SetSource(source);
            m_sourceCreatedConn.disconnect();
          }
        });
  }
}

void PlotSeries::SetSource(GuiDataSource* source) {
  m_source = source;
  m_id = source->GetId();

  // add initial value
  m_data[m_size++] = ImPlotPoint{wpi::Now() * 1.0e-6, source->GetValue()};

  m_newValueConn = source->valueChanged.connect_connection(
      [this](double value) { AppendValue(value); });
}

void PlotSeries::AppendValue(double value) {
  double time = wpi::Now() * 1.0e-6;
  if (IsDigital()) {
    if (m_size < kMaxSize) {
      m_data[m_size] = ImPlotPoint{time, value};
      ++m_size;
    } else {
      m_data[m_offset] = ImPlotPoint{time, value};
      m_offset = (m_offset + 1) % kMaxSize;
    }
  } else {
    // as an analog graph draws linear lines in between each value,
    // insert duplicate value if "long" time between updates so it
    // looks appropriately flat
    if (m_size < kMaxSize) {
      if (m_size > 0) {
        if ((time - m_data[m_size - 1].x) > kTimeGap) {
          m_data[m_size] = ImPlotPoint{time, m_data[m_size - 1].y};
          ++m_size;
        }
      }
      m_data[m_size] = ImPlotPoint{time, value};
      ++m_size;
    } else {
      if (m_offset == 0) {
        if ((time - m_data[kMaxSize - 1].x) > kTimeGap) {
          m_data[m_offset] = ImPlotPoint{time, m_data[kMaxSize - 1].y};
          ++m_offset;
        }
      } else {
        if ((time - m_data[m_offset - 1].x) > kTimeGap) {
          m_data[m_offset] = ImPlotPoint{time, m_data[m_offset - 1].y};
          m_offset = (m_offset + 1) % kMaxSize;
        }
      }
      m_data[m_offset] = ImPlotPoint{time, value};
      m_offset = (m_offset + 1) % kMaxSize;
    }
  }
}

bool PlotSeries::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (NameInfo::ReadIni(name, value)) return true;
  if (OpenInfo::ReadIni(name, value)) return true;
  if (name == "yAxis") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_yAxis = num;
    return true;
  } else if (name == "color") {
    unsigned int num;
    if (value.getAsInteger(10, num)) return true;
    m_color = ImColor(num);
    return true;
  } else if (name == "marker") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_marker = num;
    return true;
  } else if (name == "digital") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_digital = num;
    return true;
  } else if (name == "digitalBitHeight") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_digitalBitHeight = num;
    return true;
  } else if (name == "digitalBitGap") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_digitalBitGap = num;
    return true;
  }
  return false;
}

void PlotSeries::WriteIni(ImGuiTextBuffer* out) {
  NameInfo::WriteIni(out);
  OpenInfo::WriteIni(out);
  out->appendf(
      "yAxis=%d\ncolor=%u\nmarker=%d\ndigital=%d\n"
      "digitalBitHeight=%d\ndigitalBitGap=%d\n",
      m_yAxis, static_cast<ImU32>(ImColor(m_color)), m_marker, m_digital,
      m_digitalBitHeight, m_digitalBitGap);
}

void PlotSeries::GetLabel(char* buf, size_t size) const {
  const char* name = GetName();
  if (name[0] == '\0' && m_newValueConn.connected()) name = m_source->GetName();
  if (name[0] == '\0') name = m_id.c_str();
  std::snprintf(buf, size, "%s###%s", name, m_id.c_str());
}

bool PlotSeries::EmitPlot(double now, size_t i, size_t plotIndex) {
  CheckSource();

  char label[128];
  GetLabel(label, sizeof(label));

  int size = m_size;
  int offset = m_offset;

  // need to have last value at current time, so need to create fake last value
  // we handle the offset logic ourselves to avoid wrap issues with size + 1
  struct GetterData {
    double now;
    ImPlotPoint* data;
    int size;
    int offset;
  };
  GetterData getterData = {now, m_data, size, offset};
  auto getter = [](void* data, int idx) {
    auto d = static_cast<GetterData*>(data);
    if (idx == d->size)
      return ImPlotPoint{
          d->now, d->data[d->offset == 0 ? d->size - 1 : d->offset - 1].y};
    if (d->offset + idx < d->size)
      return d->data[d->offset + idx];
    else
      return d->data[d->offset + idx - d->size];
  };

  if (m_color.w == IMPLOT_AUTO_COL.w) m_color = ImPlot::GetColormapColor(i);
  ImPlot::PushStyleColor(ImPlotCol_Line, m_color);
  if (IsDigital()) {
    ImPlot::PushStyleVar(ImPlotStyleVar_DigitalBitHeight, m_digitalBitHeight);
    ImPlot::PushStyleVar(ImPlotStyleVar_DigitalBitGap, m_digitalBitGap);
    ImPlot::PlotDigital(label, getter, &getterData, size + 1);
    ImPlot::PopStyleVar();
    ImPlot::PopStyleVar();
  } else {
    ImPlot::SetPlotYAxis(m_yAxis);
    ImPlot::SetNextMarkerStyle(m_marker - 1);
    ImPlot::PlotLine(label, getter, &getterData, size + 1);
  }
  ImPlot::PopStyleColor();

  // DND source for PlotSeries
  if (ImPlot::BeginLegendDragDropSource(label)) {
    EmitDragDropPayload(i, plotIndex);
    ImPlot::EndLegendDragDropSource();
  }

  // Plot-specific variant of IniSaverInfo::PopupEditName() that also
  // allows editing of other settings
  bool rv = false;
  if (ImPlot::BeginLegendPopup(label)) {
    if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
    ImGui::Text("Edit name:");
    if (InputTextName("##edit", ImGuiInputTextFlags_EnterReturnsTrue)) {
      ImGui::CloseCurrentPopup();
    }
    rv = EmitSettingsDetail(i);
    ImPlot::EndLegendPopup();
  }

  return rv;
}

void PlotSeries::EmitDragDropPayload(size_t i, size_t plotIndex) {
  PlotSeriesRef ref = {plotIndex, i};
  ImGui::SetDragDropPayload("PlotSeries", &ref, sizeof(ref));
  const char* name = GetName();
  if (name[0] == '\0' && m_newValueConn.connected()) name = m_source->GetName();
  if (name[0] == '\0') name = m_id.c_str();
  ImGui::TextUnformatted(name);
}

static void MovePlotSeries(size_t fromPlotIndex, size_t fromSeriesIndex,
                           size_t toPlotIndex, size_t toSeriesIndex,
                           int yAxis = -1) {
  if (fromPlotIndex == toPlotIndex) {
    // need to handle this specially as the index of the old location changes
    if (fromSeriesIndex != toSeriesIndex) {
      auto& plotSeries = gPlots[fromPlotIndex].m_series;
      auto val = std::move(plotSeries[fromSeriesIndex]);
      // only set Y-axis if actually set
      if (yAxis != -1) val->SetYAxis(yAxis);
      plotSeries.insert(plotSeries.begin() + toSeriesIndex, std::move(val));
      plotSeries.erase(plotSeries.begin() + fromSeriesIndex +
                       (fromSeriesIndex > toSeriesIndex ? 1 : 0));
    }
  } else {
    auto& fromPlot = gPlots[fromPlotIndex];
    auto& toPlot = gPlots[toPlotIndex];
    // always set Y-axis if moving plots
    fromPlot.m_series[fromSeriesIndex]->SetYAxis(yAxis == -1 ? 0 : yAxis);
    toPlot.m_series.insert(toPlot.m_series.begin() + toSeriesIndex,
                           std::move(fromPlot.m_series[fromSeriesIndex]));
    fromPlot.m_series.erase(fromPlot.m_series.begin() + fromSeriesIndex);
  }
}

bool PlotSeries::EmitSettings(size_t i, size_t plotIndex) {
  char label[128];
  GetLabel(label, sizeof(label));

  bool open = ImGui::CollapsingHeader(
      label, IsOpen() ? ImGuiTreeNodeFlags_DefaultOpen : 0);

  // DND source for PlotSeries
  if (ImGui::BeginDragDropSource()) {
    EmitDragDropPayload(i, plotIndex);
    ImGui::EndDragDropSource();
  }

  // If another PlotSeries is dropped, move it before this series
  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload =
            ImGui::AcceptDragDropPayload("PlotSeries")) {
      auto ref = static_cast<const PlotSeriesRef*>(payload->Data);
      MovePlotSeries(ref->plotIndex, ref->seriesIndex, plotIndex, i);
    }
  }

  SetOpen(open);
  PopupEditName(i);
  if (!open) return false;

  return EmitSettingsDetail(i);
}

bool PlotSeries::EmitSettingsDetail(size_t i) {
  if (ImGui::Button("Delete")) {
    return true;
  }

  // Line color
  {
    ImGui::ColorEdit3("Color", &m_color.x, ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    if (ImGui::Button("Default")) m_color = ImPlot::GetColormapColor(i);
  }

  // Digital
  {
    static const char* const options[] = {"Auto", "Digital", "Analog"};
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    ImGui::Combo("Digital", &m_digital, options,
                 sizeof(options) / sizeof(options[0]));
  }

  if (IsDigital()) {
    // Bit Height
    {
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
      ImGui::InputInt("Bit Height", &m_digitalBitHeight);
    }

    // Bit Gap
    {
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
      ImGui::InputInt("Bit Gap", &m_digitalBitGap);
    }
  } else {
    // Y-axis
    {
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
      static const char* const options[] = {"1", "2", "3"};
      ImGui::Combo("Y-Axis", &m_yAxis, options, 3);
    }

    // Marker
    {
      static const char* const options[] = {
          "None", "Circle", "Square", "Diamond", "Up",      "Down",
          "Left", "Right",  "Cross",  "Plus",    "Asterisk"};
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
      ImGui::Combo("Marker", &m_marker, options,
                   sizeof(options) / sizeof(options[0]));
    }
  }

  return false;
}

Plot::Plot() {
  for (int i = 0; i < 3; ++i) {
    m_axisRange[i] = PlotRange{};
  }
}

bool Plot::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (NameInfo::ReadIni(name, value)) return true;
  if (OpenInfo::ReadIni(name, value)) return true;
  if (name == "visible") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_visible = num != 0;
    return true;
  } else if (name == "lockPrevX") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_lockPrevX = num != 0;
    return true;
  } else if (name == "legend") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    if (num == 0)
      m_plotFlags &= ~ImPlotFlags_Legend;
    else
      m_plotFlags |= ImPlotFlags_Legend;
    return true;
  } else if (name == "yaxis2") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    if (num == 0)
      m_plotFlags &= ~ImPlotFlags_YAxis2;
    else
      m_plotFlags |= ImPlotFlags_YAxis2;
    return true;
  } else if (name == "yaxis3") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    if (num == 0)
      m_plotFlags &= ~ImPlotFlags_YAxis3;
    else
      m_plotFlags |= ImPlotFlags_YAxis3;
    return true;
  } else if (name == "viewTime") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_viewTime = num / 1000.0;
    return true;
  } else if (name == "height") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_height = num;
    return true;
  } else if (name.startswith("y")) {
    auto [yAxisStr, yName] = name.split('_');
    int yAxis;
    if (yAxisStr.substr(1).getAsInteger(10, yAxis)) return false;
    if (yAxis < 0 || yAxis > 3) return false;
    if (yName == "min") {
      int num;
      if (value.getAsInteger(10, num)) return true;
      m_axisRange[yAxis].min = num / 1000.0;
      return true;
    } else if (yName == "max") {
      int num;
      if (value.getAsInteger(10, num)) return true;
      m_axisRange[yAxis].max = num / 1000.0;
      return true;
    } else if (yName == "lockMin") {
      int num;
      if (value.getAsInteger(10, num)) return true;
      m_axisRange[yAxis].lockMin = num != 0;
      return true;
    } else if (yName == "lockMax") {
      int num;
      if (value.getAsInteger(10, num)) return true;
      m_axisRange[yAxis].lockMax = num != 0;
      return true;
    }
  }
  return false;
}

void Plot::WriteIni(ImGuiTextBuffer* out) {
  NameInfo::WriteIni(out);
  OpenInfo::WriteIni(out);
  out->appendf(
      "visible=%d\nlockPrevX=%d\nlegend=%d\nyaxis2=%d\nyaxis3=%d\n"
      "viewTime=%d\nheight=%d\n",
      m_visible ? 1 : 0, m_lockPrevX ? 1 : 0,
      (m_plotFlags & ImPlotFlags_Legend) ? 1 : 0,
      (m_plotFlags & ImPlotFlags_YAxis2) ? 1 : 0,
      (m_plotFlags & ImPlotFlags_YAxis3) ? 1 : 0,
      static_cast<int>(m_viewTime * 1000), m_height);
  for (int i = 0; i < 3; ++i) {
    out->appendf("y%d_min=%d\ny%d_max=%d\ny%d_lockMin=%d\ny%d_lockMax=%d\n", i,
                 static_cast<int>(m_axisRange[i].min * 1000), i,
                 static_cast<int>(m_axisRange[i].max * 1000), i,
                 m_axisRange[i].lockMin ? 1 : 0, i,
                 m_axisRange[i].lockMax ? 1 : 0);
  }
}

void Plot::GetLabel(char* buf, size_t size, int index) const {
  const char* name = NameInfo::GetName();
  if (name[0] != '\0') {
    std::snprintf(buf, size, "%s##Plot%d", name, index);
  } else {
    std::snprintf(buf, size, "Plot %d##Plot%d", index, index);
  }
}

void Plot::GetName(char* buf, size_t size, int index) const {
  const char* name = NameInfo::GetName();
  if (name[0] != '\0') {
    std::snprintf(buf, size, "%s", name);
  } else {
    std::snprintf(buf, size, "Plot %d", index);
  }
}

void Plot::DragDropTarget(size_t i, bool inPlot) {
  if (!ImGui::BeginDragDropTarget()) return;
  // handle dragging onto a specific Y axis
  int yAxis = -1;
  if (inPlot) {
    for (int y = 0; y < 3; ++y) {
      if (ImPlot::IsPlotYAxisHovered(y)) {
        yAxis = y;
        break;
      }
    }
  }
  if (const ImGuiPayload* payload =
          ImGui::AcceptDragDropPayload("DataSource")) {
    auto source = *static_cast<GuiDataSource**>(payload->Data);
    // don't add duplicates unless it's onto a different Y axis
    auto it =
        std::find_if(m_series.begin(), m_series.end(), [=](const auto& elem) {
          return elem->GetId() == source->GetId() &&
                 (yAxis == -1 || elem->GetYAxis() == yAxis);
        });
    if (it == m_series.end()) {
      m_series.emplace_back(
          std::make_unique<PlotSeries>(source, yAxis == -1 ? 0 : yAxis));
    }
  } else if (const ImGuiPayload* payload =
                 ImGui::AcceptDragDropPayload("PlotSeries")) {
    auto ref = static_cast<const PlotSeriesRef*>(payload->Data);
    MovePlotSeries(ref->plotIndex, ref->seriesIndex, i, m_series.size(), yAxis);
  } else if (const ImGuiPayload* payload =
                 ImGui::AcceptDragDropPayload("Plot")) {
    auto fromPlotIndex = *static_cast<const size_t*>(payload->Data);
    if (i != fromPlotIndex) {
      auto val = std::move(gPlots[fromPlotIndex]);
      gPlots.insert(gPlots.begin() + i, std::move(val));
      gPlots.erase(gPlots.begin() + fromPlotIndex +
                   (fromPlotIndex > i ? 1 : 0));
    }
  }
}

void Plot::EmitPlot(double now, size_t i) {
  if (!m_visible) return;

  bool lockX = (i != 0 && m_lockPrevX);

  if (!lockX && ImGui::Button(m_paused ? "Resume" : "Pause"))
    m_paused = !m_paused;

  char label[128];
  GetLabel(label, sizeof(label), i);

  if (lockX) {
    ImPlot::SetNextPlotLimitsX(gPlots[i - 1].m_xaxisRange.Min,
                               gPlots[i - 1].m_xaxisRange.Max,
                               ImGuiCond_Always);
  } else {
    // also force-pause plots if overall timing is paused
    ImPlot::SetNextPlotLimitsX(now - m_viewTime, now,
                               (m_paused || HALSIM_IsTimingPaused())
                                   ? ImGuiCond_Once
                                   : ImGuiCond_Always);
  }

  ImPlotAxisFlags yFlags[3] = {ImPlotAxisFlags_Default,
                               ImPlotAxisFlags_Auxiliary,
                               ImPlotAxisFlags_Auxiliary};
  for (int i = 0; i < 3; ++i) {
    ImPlot::SetNextPlotLimitsY(
        m_axisRange[i].min, m_axisRange[i].max,
        m_axisRange[i].apply ? ImGuiCond_Always : ImGuiCond_Once, i);
    m_axisRange[i].apply = false;
    if (m_axisRange[i].lockMin) yFlags[i] |= ImPlotAxisFlags_LockMin;
    if (m_axisRange[i].lockMax) yFlags[i] |= ImPlotAxisFlags_LockMax;
  }

  if (ImPlot::BeginPlot(label, nullptr, nullptr, ImVec2(-1, m_height),
                        m_plotFlags, ImPlotAxisFlags_Default, yFlags[0],
                        yFlags[1], yFlags[2])) {
    for (size_t j = 0; j < m_series.size(); ++j) {
      if (m_series[j]->EmitPlot(now, j, i)) {
        m_series.erase(m_series.begin() + j);
      }
    }
    DragDropTarget(i, true);
    m_xaxisRange = ImPlot::GetPlotLimits().X;
    ImPlot::EndPlot();
  }
}

void Plot::EmitSettingsLimits(int axis) {
  ImGui::Indent();
  ImGui::PushID(axis);

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 3.5);
  ImGui::InputDouble("Min", &m_axisRange[axis].min, 0, 0, "%.3f");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 3.5);
  ImGui::InputDouble("Max", &m_axisRange[axis].max, 0, 0, "%.3f");
  ImGui::SameLine();
  if (ImGui::Button("Apply")) m_axisRange[axis].apply = true;

  ImGui::TextUnformatted("Lock Axis");
  ImGui::SameLine();
  ImGui::Checkbox("Min##minlock", &m_axisRange[axis].lockMin);
  ImGui::SameLine();
  ImGui::Checkbox("Max##maxlock", &m_axisRange[axis].lockMax);

  ImGui::PopID();
  ImGui::Unindent();
}

// Delete button (X in circle), based on ImGui::CloseButton()
static bool DeleteButton(ImGuiID id, const ImVec2& pos) {
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
  if (is_clipped) return pressed;

  // Render
  ImU32 col =
      ImGui::GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
  ImVec2 center = bb.GetCenter();
  if (hovered)
    window->DrawList->AddCircleFilled(
        center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f), col, 12);

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

void Plot::EmitSettings(size_t i) {
  char label[128];
  GetLabel(label, sizeof(label), i);

  bool open = ImGui::CollapsingHeader(
      label, ImGuiTreeNodeFlags_AllowItemOverlap |
                 ImGuiTreeNodeFlags_ClipLabelForTrailingButton |
                 (IsOpen() ? ImGuiTreeNodeFlags_DefaultOpen : 0));

  {
    // Create a small overlapping delete button
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiContext& g = *GImGui;
    ImGuiItemHoveredDataBackup last_item_backup;
    ImGuiID id = window->GetID(label);
    float button_size = g.FontSize;
    float button_x = ImMax(window->DC.LastItemRect.Min.x,
                           window->DC.LastItemRect.Max.x -
                               g.Style.FramePadding.x * 2.0f - button_size);
    float button_y = window->DC.LastItemRect.Min.y;
    if (DeleteButton(window->GetID(reinterpret_cast<void*>(
                         static_cast<intptr_t>(id) + 1)),
                     ImVec2(button_x, button_y))) {
      gPlots.erase(gPlots.begin() + i);
      return;
    }
    last_item_backup.Restore();
  }

  // DND source for Plot
  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload("Plot", &i, sizeof(i));
    char name[64];
    GetName(name, sizeof(name), i);
    ImGui::TextUnformatted(name);
    ImGui::EndDragDropSource();
  }
  DragDropTarget(i, false);
  SetOpen(open);
  PopupEditName(i);
  if (!open) return;
  ImGui::PushID(i);
#if 0
  if (ImGui::Button("Move Up") && i > 0) {
    std::swap(gPlots[i - 1], gPlots[i]);
    ImGui::PopID();
    return;
  }
  ImGui::SameLine();
  if (ImGui::Button("Move Down") && i < (gPlots.size() - 1)) {
    std::swap(gPlots[i], gPlots[i + 1]);
    ImGui::PopID();
    return;
  }
  ImGui::SameLine();
  if (ImGui::Button("Delete")) {
    gPlots.erase(gPlots.begin() + i);
    ImGui::PopID();
    return;
  }
#endif
  ImGui::Checkbox("Visible", &m_visible);
  ImGui::CheckboxFlags("Show Legend", &m_plotFlags, ImPlotFlags_Legend);
  if (i != 0) ImGui::Checkbox("Lock X-axis to previous plot", &m_lockPrevX);
  ImGui::TextUnformatted("Primary Y-Axis");
  EmitSettingsLimits(0);
  ImGui::CheckboxFlags("2nd Y-Axis", &m_plotFlags, ImPlotFlags_YAxis2);
  if ((m_plotFlags & ImPlotFlags_YAxis2) != 0) EmitSettingsLimits(1);
  ImGui::CheckboxFlags("3rd Y-Axis", &m_plotFlags, ImPlotFlags_YAxis3);
  if ((m_plotFlags & ImPlotFlags_YAxis3) != 0) EmitSettingsLimits(2);
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
  ImGui::InputFloat("View Time (s)", &m_viewTime, 0.1f, 1.0f, "%.1f");
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
  if (ImGui::InputInt("Height", &m_height, 10)) {
    if (m_height < 0) m_height = 0;
  }

  ImGui::Indent();
  for (size_t j = 0; j < m_series.size(); ++j) {
    ImGui::PushID(j);
    if (m_series[j]->EmitSettings(j, i)) {
      m_series.erase(m_series.begin() + j);
    }
    ImGui::PopID();
  }
  ImGui::Unindent();

  ImGui::PopID();
}

static void DisplayPlot() {
  if (gPlots.empty()) {
    ImGui::Text("No Plots");
    return;
  }
  double now = wpi::Now() * 1.0e-6;
  for (size_t i = 0; i < gPlots.size(); ++i) {
    ImGui::PushID(i);
    gPlots[i].EmitPlot(now, i);
    ImGui::PopID();
  }
  ImGui::Text("(Right double click for more settings)");
}

static void DisplayPlotSettings() {
  if (ImGui::Button("Add new plot")) {
    gPlots.emplace_back();
  }
  for (size_t i = 0; i < gPlots.size(); ++i) {
    gPlots[i].EmitSettings(i);
  }
}

static void* PlotSeries_ReadOpen(ImGuiContext* ctx,
                                 ImGuiSettingsHandler* handler,
                                 const char* name) {
  wpi::StringRef plotIndexStr, id;
  std::tie(plotIndexStr, id) = wpi::StringRef{name}.split(',');
  unsigned int plotIndex;
  if (plotIndexStr.getAsInteger(10, plotIndex)) return nullptr;
  if (plotIndex >= gPlots.size()) gPlots.resize(plotIndex + 1);
  auto& plot = gPlots[plotIndex];
  auto it = std::find_if(
      plot.m_series.begin(), plot.m_series.end(),
      [&](const auto& elem) { return elem && elem->GetId() == id; });
  if (it != plot.m_series.end()) return it->get();
  return plot.m_series.emplace_back(std::make_unique<PlotSeries>(id)).get();
}

static void PlotSeries_ReadLine(ImGuiContext* ctx,
                                ImGuiSettingsHandler* handler, void* entry,
                                const char* lineStr) {
  auto element = static_cast<PlotSeries*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  element->ReadIni(name, value);
}

static void PlotSeries_WriteAll(ImGuiContext* ctx,
                                ImGuiSettingsHandler* handler,
                                ImGuiTextBuffer* out_buf) {
  for (size_t i = 0; i < gPlots.size(); ++i) {
    for (const auto& series : gPlots[i].m_series) {
      out_buf->appendf("[PlotSeries][%d,%s]\n", static_cast<int>(i),
                       series->GetId().c_str());
      series->WriteIni(out_buf);
      out_buf->append("\n");
    }
  }
}

void PlotGui::Initialize() {
  gPlots.Initialize();

  // hook ini handler for PlotSeries to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "PlotSeries";
  iniHandler.TypeHash = ImHashStr("PlotSeries");
  iniHandler.ReadOpenFn = PlotSeries_ReadOpen;
  iniHandler.ReadLineFn = PlotSeries_ReadLine;
  iniHandler.WriteAllFn = PlotSeries_WriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  // HALSimGui::AddExecute([] { ImPlot::ShowDemoWindow(); });
  HALSimGui::AddWindow("Plot", DisplayPlot);
  HALSimGui::SetDefaultWindowPos("Plot", 600, 75);
  HALSimGui::SetDefaultWindowSize("Plot", 300, 200);

  HALSimGui::AddWindow("Plot Settings", DisplayPlotSettings);
  HALSimGui::SetDefaultWindowPos("Plot Settings", 902, 75);
  HALSimGui::SetDefaultWindowSize("Plot Settings", 120, 200);
}
