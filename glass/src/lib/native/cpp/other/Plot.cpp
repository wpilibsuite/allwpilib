/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/other/Plot.h"

#include <stdint.h>

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <implot.h>
#include <wpigui.h>
#include <wpi/Signal.h>
#include <wpi/SmallString.h>
#include <wpi/SmallVector.h>
#include <wpi/timestamp.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/support/ExtraGuiWidgets.h"

using namespace glass;

namespace {
class PlotView;

struct PlotSeriesRef {
  PlotView* view;
  size_t plotIndex;
  size_t seriesIndex;
};

class PlotSeries {
 public:
  explicit PlotSeries(wpi::StringRef id);
  explicit PlotSeries(DataSource* source, int yAxis = 0);

  const std::string& GetId() const { return m_id; }

  void CheckSource();

  void SetSource(DataSource* source);
  DataSource* GetSource() const { return m_source; }

  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out);

  enum Action { kNone, kMoveUp, kMoveDown, kDelete };
  Action EmitPlot(PlotView& view, double now, size_t i, size_t plotIndex);
  void EmitSettings(size_t i);
  void EmitDragDropPayload(PlotView& view, size_t i, size_t plotIndex);

  const char* GetName() const;

  int GetYAxis() const { return m_yAxis; }
  void SetYAxis(int yAxis) { m_yAxis = yAxis; }

 private:
  bool IsDigital() const {
    return m_digital == kDigital ||
           (m_digital == kAuto && m_source && m_source->IsDigital());
  }
  void AppendValue(double value, uint64_t time);

  // source linkage
  DataSource* m_source = nullptr;
  wpi::sig::ScopedConnection m_sourceCreatedConn;
  wpi::sig::ScopedConnection m_newValueConn;
  std::string m_id;

  // user settings
  std::string m_name;
  int m_yAxis = 0;
  ImVec4 m_color = IMPLOT_AUTO_COL;
  int m_marker = 0;
  float m_weight = IMPLOT_AUTO;

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

class Plot {
 public:
  Plot();

  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out);

  void DragDropTarget(PlotView& view, size_t i, bool inPlot);
  void EmitPlot(PlotView& view, double now, bool paused, size_t i);
  void EmitSettings(size_t i);

  const std::string& GetName() const { return m_name; }

  std::vector<std::unique_ptr<PlotSeries>> m_series;

 private:
  void EmitSettingsLimits(int axis);

  std::string m_name;
  bool m_visible = true;
  bool m_showPause = true;
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

class PlotView : public View {
 public:
  explicit PlotView(PlotProvider* provider) : m_provider{provider} {}

  void Display() override;

  void MovePlot(PlotView* fromView, size_t fromIndex, size_t toIndex);

  void MovePlotSeries(PlotView* fromView, size_t fromPlotIndex,
                      size_t fromSeriesIndex, size_t toPlotIndex,
                      size_t toSeriesIndex, int yAxis = -1);

  PlotProvider* m_provider;
  std::vector<std::unique_ptr<Plot>> m_plots;
};

}  // namespace

PlotSeries::PlotSeries(wpi::StringRef id) : m_id(id) {
  if (DataSource* source = DataSource::Find(id)) {
    SetSource(source);
    return;
  }
  CheckSource();
}

PlotSeries::PlotSeries(DataSource* source, int yAxis) : m_yAxis(yAxis) {
  SetSource(source);
}

void PlotSeries::CheckSource() {
  if (!m_newValueConn.connected() && !m_sourceCreatedConn.connected()) {
    m_source = nullptr;
    m_sourceCreatedConn = DataSource::sourceCreated.connect_connection(
        [this](const char* id, DataSource* source) {
          if (m_id == id) {
            SetSource(source);
            m_sourceCreatedConn.disconnect();
          }
        });
  }
}

void PlotSeries::SetSource(DataSource* source) {
  m_source = source;
  m_id = source->GetId();

  // add initial value
  m_data[m_size++] = ImPlotPoint{wpi::Now() * 1.0e-6, source->GetValue()};

  m_newValueConn = source->valueChanged.connect_connection(
      [this](double value, uint64_t time) { AppendValue(value, time); });
}

void PlotSeries::AppendValue(double value, uint64_t timeUs) {
  double time = (timeUs != 0 ? timeUs : wpi::Now()) * 1.0e-6;
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
  if (name == "name") {
    m_name = value;
    return true;
  }
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
  } else if (name == "weight") {
    std::sscanf(value.data(), "%f", &m_weight);
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
  out->appendf(
      "name=%s\nyAxis=%d\ncolor=%u\nmarker=%d\nweight=%f\ndigital=%d\n"
      "digitalBitHeight=%d\ndigitalBitGap=%d\n",
      m_name.c_str(), m_yAxis, static_cast<ImU32>(ImColor(m_color)), m_marker,
      m_weight, m_digital, m_digitalBitHeight, m_digitalBitGap);
}

const char* PlotSeries::GetName() const {
  if (!m_name.empty()) return m_name.c_str();
  if (m_newValueConn.connected()) {
    auto sourceName = m_source->GetName();
    if (sourceName[0] != '\0') return sourceName;
  }
  return m_id.c_str();
}

PlotSeries::Action PlotSeries::EmitPlot(PlotView& view, double now, size_t i,
                                        size_t plotIndex) {
  CheckSource();

  char label[128];
  std::snprintf(label, sizeof(label), "%s###name", GetName());

  int size = m_size;
  int offset = m_offset;

  // need to have last value at current time, so need to create fake last value
  // we handle the offset logic ourselves to avoid wrap issues with size + 1
  struct GetterData {
    double now;
    double zeroTime;
    ImPlotPoint* data;
    int size;
    int offset;
  };
  GetterData getterData = {now, GetZeroTime() * 1.0e-6, m_data, size, offset};
  auto getter = [](void* data, int idx) {
    auto d = static_cast<GetterData*>(data);
    if (idx == d->size)
      return ImPlotPoint{
          d->now - d->zeroTime,
          d->data[d->offset == 0 ? d->size - 1 : d->offset - 1].y};
    ImPlotPoint* point;
    if (d->offset + idx < d->size)
      point = &d->data[d->offset + idx];
    else
      point = &d->data[d->offset + idx - d->size];
    return ImPlotPoint{point->x - d->zeroTime, point->y};
  };

  if (m_color.w == IMPLOT_AUTO_COL.w) m_color = ImPlot::GetColormapColor(i);
  ImPlot::SetNextLineStyle(m_color, m_weight);
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

  // DND source for PlotSeries
  if (ImPlot::BeginLegendDragDropSource(label)) {
    EmitDragDropPayload(view, i, plotIndex);
    ImPlot::EndLegendDragDropSource();
  }

  // Edit settings via popup
  Action rv = kNone;
  if (ImPlot::BeginLegendPopup(label)) {
    if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
    ImGui::Text("Edit series name:");
    ImGui::InputText("##editname", &m_name);
    if (ImGui::Button("Move Up")) {
      ImGui::CloseCurrentPopup();
      rv = kMoveUp;
    }
    ImGui::SameLine();
    if (ImGui::Button("Move Down")) {
      ImGui::CloseCurrentPopup();
      rv = kMoveDown;
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
      ImGui::CloseCurrentPopup();
      rv = kDelete;
    }
    EmitSettings(i);
    ImPlot::EndLegendPopup();
  }

  return rv;
}

void PlotSeries::EmitDragDropPayload(PlotView& view, size_t i,
                                     size_t plotIndex) {
  PlotSeriesRef ref = {&view, plotIndex, i};
  ImGui::SetDragDropPayload("PlotSeries", &ref, sizeof(ref));
  ImGui::TextUnformatted(GetName());
}

void PlotSeries::EmitSettings(size_t i) {
  // Line color
  {
    ImGui::ColorEdit3("Color", &m_color.x, ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    if (ImGui::Button("Default")) m_color = ImPlot::GetColormapColor(i);
  }

  // Line weight
  {
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    ImGui::InputFloat("Weight", &m_weight, 0.1f, 1.0f, "%.1f");
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
}

Plot::Plot() {
  for (int i = 0; i < 3; ++i) {
    m_axisRange[i] = PlotRange{};
  }
}

bool Plot::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name == "name") {
    m_name = value;
    return true;
  } else if (name == "visible") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_visible = num != 0;
    return true;
  } else if (name == "showPause") {
    int num;
    if (value.getAsInteger(10, num)) return true;
    m_showPause = num != 0;
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
  out->appendf(
      "name=%s\nvisible=%d\nshowPause=%d\nlockPrevX=%d\nlegend=%d\n"
      "yaxis2=%d\nyaxis3=%d\nviewTime=%d\nheight=%d\n",
      m_name.c_str(), m_visible ? 1 : 0, m_showPause ? 1 : 0,
      m_lockPrevX ? 1 : 0, (m_plotFlags & ImPlotFlags_Legend) ? 1 : 0,
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

void Plot::DragDropTarget(PlotView& view, size_t i, bool inPlot) {
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
    auto source = *static_cast<DataSource**>(payload->Data);
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
    view.MovePlotSeries(ref->view, ref->plotIndex, ref->seriesIndex, i,
                        m_series.size(), yAxis);
  } else if (const ImGuiPayload* payload =
                 ImGui::AcceptDragDropPayload("Plot")) {
    auto ref = static_cast<const PlotSeriesRef*>(payload->Data);
    view.MovePlot(ref->view, ref->plotIndex, i);
  }
}

void Plot::EmitPlot(PlotView& view, double now, bool paused, size_t i) {
  if (!m_visible) return;

  bool lockX = (i != 0 && m_lockPrevX);

  if (!lockX && m_showPause && ImGui::Button(m_paused ? "Resume" : "Pause"))
    m_paused = !m_paused;

  char label[128];
  std::snprintf(label, sizeof(label), "%s##plot", m_name.c_str());

  if (lockX) {
    ImPlot::SetNextPlotLimitsX(view.m_plots[i - 1]->m_xaxisRange.Min,
                               view.m_plots[i - 1]->m_xaxisRange.Max,
                               ImGuiCond_Always);
  } else {
    // also force-pause plots if overall timing is paused
    double zeroTime = GetZeroTime() * 1.0e-6;
    ImPlot::SetNextPlotLimitsX(
        now - zeroTime - m_viewTime, now - zeroTime,
        (paused || m_paused) ? ImGuiCond_Once : ImGuiCond_Always);
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
      ImGui::PushID(j);
      switch (m_series[j]->EmitPlot(view, now, j, i)) {
        case PlotSeries::kMoveUp:
          if (j > 0) std::swap(m_series[j - 1], m_series[j]);
          break;
        case PlotSeries::kMoveDown:
          if (j < (m_series.size() - 1))
            std::swap(m_series[j], m_series[j + 1]);
          break;
        case PlotSeries::kDelete:
          m_series.erase(m_series.begin() + j);
          break;
        default:
          break;
      }
      ImGui::PopID();
    }
    DragDropTarget(view, i, true);
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

void Plot::EmitSettings(size_t i) {
  ImGui::Text("Edit plot name:");
  ImGui::InputText("##editname", &m_name);
  ImGui::Checkbox("Visible", &m_visible);
  ImGui::Checkbox("Show Pause Button", &m_showPause);
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
}

void PlotView::Display() {
  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::Button("Add plot"))
      m_plots.emplace_back(std::make_unique<Plot>());

    for (size_t i = 0; i < m_plots.size(); ++i) {
      auto& plot = m_plots[i];
      ImGui::PushID(i);

      char name[64];
      if (!plot->GetName().empty())
        std::snprintf(name, sizeof(name), "%s", plot->GetName().c_str());
      else
        std::snprintf(name, sizeof(name), "Plot %d", static_cast<int>(i));

      char label[90];
      std::snprintf(label, sizeof(label), "%s###header%d", name,
                    static_cast<int>(i));

      bool open = ImGui::CollapsingHeader(label);

      // DND source and target for Plot
      if (ImGui::BeginDragDropSource()) {
        PlotSeriesRef ref = {this, i, 0};
        ImGui::SetDragDropPayload("Plot", &ref, sizeof(ref));
        ImGui::TextUnformatted(name);
        ImGui::EndDragDropSource();
      }
      plot->DragDropTarget(*this, i, false);

      if (open) {
        if (ImGui::Button("Move Up")) {
          if (i > 0) std::swap(m_plots[i - 1], plot);
        }

        ImGui::SameLine();
        if (ImGui::Button("Move Down")) {
          if (i < (m_plots.size() - 1)) std::swap(plot, m_plots[i + 1]);
        }

        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
          m_plots.erase(m_plots.begin() + i);
          ImGui::PopID();
          continue;
        }

        plot->EmitSettings(i);
      }

      ImGui::PopID();
    }

    ImGui::EndPopup();
  }

  if (m_plots.empty()) {
    if (ImGui::Button("Add plot"))
      m_plots.emplace_back(std::make_unique<Plot>());

    // Make "add plot" button a DND target for Plot
    if (!ImGui::BeginDragDropTarget()) return;
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Plot")) {
      auto ref = static_cast<const PlotSeriesRef*>(payload->Data);
      MovePlot(ref->view, ref->plotIndex, 0);
    }
  }

  double now = wpi::Now() * 1.0e-6;
  for (size_t i = 0; i < m_plots.size(); ++i) {
    ImGui::PushID(i);
    m_plots[i]->EmitPlot(*this, now, m_provider->IsPaused(), i);
    ImGui::PopID();
  }
}

void PlotView::MovePlot(PlotView* fromView, size_t fromIndex, size_t toIndex) {
  if (fromView == this) {
    if (fromIndex == toIndex) return;
    auto val = std::move(m_plots[fromIndex]);
    m_plots.insert(m_plots.begin() + toIndex, std::move(val));
    m_plots.erase(m_plots.begin() + fromIndex + (fromIndex > toIndex ? 1 : 0));
  } else {
    auto val = std::move(fromView->m_plots[fromIndex]);
    m_plots.insert(m_plots.begin() + toIndex, std::move(val));
    fromView->m_plots.erase(fromView->m_plots.begin() + fromIndex);
  }
}

void PlotView::MovePlotSeries(PlotView* fromView, size_t fromPlotIndex,
                              size_t fromSeriesIndex, size_t toPlotIndex,
                              size_t toSeriesIndex, int yAxis) {
  if (fromView == this && fromPlotIndex == toPlotIndex) {
    // need to handle this specially as the index of the old location changes
    if (fromSeriesIndex != toSeriesIndex) {
      auto& plotSeries = m_plots[fromPlotIndex]->m_series;
      auto val = std::move(plotSeries[fromSeriesIndex]);
      // only set Y-axis if actually set
      if (yAxis != -1) val->SetYAxis(yAxis);
      plotSeries.insert(plotSeries.begin() + toSeriesIndex, std::move(val));
      plotSeries.erase(plotSeries.begin() + fromSeriesIndex +
                       (fromSeriesIndex > toSeriesIndex ? 1 : 0));
    }
  } else {
    auto& fromPlot = *fromView->m_plots[fromPlotIndex];
    auto& toPlot = *m_plots[toPlotIndex];
    // always set Y-axis if moving plots
    fromPlot.m_series[fromSeriesIndex]->SetYAxis(yAxis == -1 ? 0 : yAxis);
    toPlot.m_series.insert(toPlot.m_series.begin() + toSeriesIndex,
                           std::move(fromPlot.m_series[fromSeriesIndex]));
    fromPlot.m_series.erase(fromPlot.m_series.begin() + fromSeriesIndex);
  }
}

PlotProvider::PlotProvider(const wpi::Twine& iniName)
    : WindowManager{iniName + "Window"},
      m_plotSaver{iniName, this, false},
      m_seriesSaver{iniName + "Series", this, true} {}

PlotProvider::~PlotProvider() {}

void PlotProvider::GlobalInit() {
  WindowManager::GlobalInit();
  wpi::gui::AddInit([this] {
    m_plotSaver.Initialize();
    m_seriesSaver.Initialize();
  });
}

void PlotProvider::DisplayMenu() {
  for (size_t i = 0; i < m_windows.size(); ++i) {
    m_windows[i]->DisplayMenuItem();
    // provide method to destroy the plot window
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Destroy Plot Window")) {
        m_windows.erase(m_windows.begin() + i);
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  if (ImGui::MenuItem("New Plot Window")) {
    char id[32];
    std::snprintf(id, sizeof(id), "Plot <%d>",
                  static_cast<int>(m_windows.size()));
    AddWindow(id, std::make_unique<PlotView>(this));
  }
}

void PlotProvider::DisplayWindows() {
  // create views if not already created
  for (auto&& window : m_windows) {
    if (!window->HasView()) window->SetView(std::make_unique<PlotView>(this));
  }
  WindowManager::DisplayWindows();
}

PlotProvider::IniSaver::IniSaver(const wpi::Twine& typeName,
                                 PlotProvider* provider, bool forSeries)
    : IniSaverBase{typeName}, m_provider{provider}, m_forSeries{forSeries} {}

void* PlotProvider::IniSaver::IniReadOpen(const char* name) {
  auto [viewId, plotNumStr] = wpi::StringRef{name}.split('#');
  wpi::StringRef seriesId;
  if (m_forSeries) {
    std::tie(plotNumStr, seriesId) = plotNumStr.split('#');
    if (seriesId.empty()) return nullptr;
  }
  unsigned int plotNum;
  if (plotNumStr.getAsInteger(10, plotNum)) return nullptr;

  // get or create window
  auto win = m_provider->GetOrAddWindow(viewId, true);
  if (!win) return nullptr;

  // get or create view
  auto view = static_cast<PlotView*>(win->GetView());
  if (!view) {
    win->SetView(std::make_unique<PlotView>(m_provider));
    view = static_cast<PlotView*>(win->GetView());
  }

  // get or create plot
  if (view->m_plots.size() <= plotNum) view->m_plots.resize(plotNum + 1);
  auto& plot = view->m_plots[plotNum];
  if (!plot) plot = std::make_unique<Plot>();

  // early exit for plot data
  if (!m_forSeries) return plot.get();

  // get or create series
  return plot->m_series.emplace_back(std::make_unique<PlotSeries>(seriesId))
      .get();
}

void PlotProvider::IniSaver::IniReadLine(void* entry, const char* lineStr) {
  auto [name, value] = wpi::StringRef{lineStr}.split('=');
  name = name.trim();
  value = value.trim();
  if (m_forSeries)
    static_cast<PlotSeries*>(entry)->ReadIni(name, value);
  else
    static_cast<Plot*>(entry)->ReadIni(name, value);
}

void PlotProvider::IniSaver::IniWriteAll(ImGuiTextBuffer* out_buf) {
  for (auto&& win : m_provider->m_windows) {
    auto view = static_cast<PlotView*>(win->GetView());
    auto id = win->GetId();
    for (size_t i = 0; i < view->m_plots.size(); ++i) {
      if (m_forSeries) {
        // Loop over series
        for (auto&& series : view->m_plots[i]->m_series) {
          out_buf->appendf("[%s][%s#%d#%s]\n", GetTypeName(), id.data(),
                           static_cast<int>(i), series->GetId().c_str());
          series->WriteIni(out_buf);
          out_buf->append("\n");
        }
      } else {
        // Just the plot
        out_buf->appendf("[%s][%s#%d]\n", GetTypeName(), id.data(),
                         static_cast<int>(i));
        view->m_plots[i]->WriteIni(out_buf);
        out_buf->append("\n");
      }
    }
  }
}
