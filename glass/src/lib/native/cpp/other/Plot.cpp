// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Plot.h"

#include <stdint.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_stdlib.h>
#include <implot.h>
#include <implot_internal.h>
#include <wpi/Signal.h>
#include <wpi/SmallString.h>
#include <wpi/SmallVector.h>
#include <wpi/timestamp.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"
#include "glass/support/ColorSetting.h"
#include "glass/support/EnumSetting.h"
#include "glass/support/ExtraGuiWidgets.h"

using namespace glass;

static constexpr int kAxisCount = 3;

namespace {
class PlotView;

struct PlotSeriesRef {
  PlotView* view;
  size_t plotIndex;
  size_t seriesIndex;
};

class PlotSeries {
  explicit PlotSeries(Storage& storage);

 public:
  PlotSeries(Storage& storage, std::string_view id);
  PlotSeries(Storage& storage, DataSource* source);
  PlotSeries(Storage& storage, DataSource* source, int yAxis);

  const std::string& GetId() const { return m_id; }

  void CheckSource();

  void SetSource(DataSource* source);
  DataSource* GetSource() const { return m_source; }

  enum Action { kNone, kMoveUp, kMoveDown, kDelete };
  Action EmitPlot(PlotView& view, double now, size_t i, size_t plotIndex);
  void EmitSettings(size_t i);
  void EmitDragDropPayload(PlotView& view, size_t i, size_t plotIndex);

  const char* GetName() const;

  int GetYAxis() const { return m_yAxis; }
  void SetYAxis(int yAxis) { m_yAxis = yAxis; }

  void SetColor(const ImVec4& color) { m_color.SetColor(color); }

 private:
  bool IsDigital() const {
    return m_digital.GetValue() == kDigital ||
           (m_digital.GetValue() == kAuto && m_source && m_source->IsDigital());
  }
  void AppendValue(double value, int64_t time);

  // source linkage
  DataSource* m_source = nullptr;
  wpi::sig::ScopedConnection m_sourceCreatedConn;
  wpi::sig::ScopedConnection m_newValueConn;
  std::string& m_id;

  // user settings
  std::string& m_name;
  int& m_yAxis;
  static constexpr float kDefaultColor[4] = {0.0, 0.0, 0.0, IMPLOT_AUTO};
  ColorSetting m_color;
  EnumSetting m_marker;
  float& m_weight;

  enum Digital { kAuto, kDigital, kAnalog };
  EnumSetting m_digital;
  int& m_digitalBitHeight;
  int& m_digitalBitGap;

  // value storage
  static constexpr int kMaxSize = 20000;
  static constexpr double kTimeGap = 0.05;
  std::atomic<int> m_size = 0;
  std::atomic<int> m_offset = 0;
  ImPlotPoint m_data[kMaxSize];
};

class Plot {
 public:
  explicit Plot(Storage& storage);

  void DragDropTarget(PlotView& view, size_t i, bool inPlot);
  void EmitPlot(PlotView& view, double now, bool paused, size_t i);
  void EmitSettings(size_t i);

  const std::string& GetName() const { return m_name; }

  std::vector<std::unique_ptr<PlotSeries>> m_series;
  std::vector<std::unique_ptr<Storage>>& m_seriesStorage;

  // Returns base height; does not include actual plot height if auto-sized.
  int GetAutoBaseHeight(bool* isAuto, size_t i);

  void SetAutoHeight(int height) {
    if (m_autoHeight) {
      m_height = height;
    }
  }

  void SetColor(const ImVec4& color) { m_backgroundColor.SetColor(color); }

 private:
  void EmitSettingsLimits(int axis);
  void DragDropAccept(PlotView& view, size_t i, int yAxis);

  bool m_paused = false;

  std::string& m_name;
  bool& m_visible;
  static constexpr float kDefaultBackgroundColor[4] = {0.0, 0.0, 0.0,
                                                       IMPLOT_AUTO};
  ColorSetting m_backgroundColor;
  bool& m_showPause;
  bool& m_lockPrevX;
  bool& m_legend;
  bool& m_legendOutside;
  bool& m_legendHorizontal;
  int& m_legendLocation;
  bool& m_crosshairs;
  bool& m_mousePosition;
  bool& m_yAxis2;
  bool& m_yAxis3;
  float& m_viewTime;
  bool& m_autoHeight;
  int& m_height;
  struct PlotAxis {
    PlotAxis(Storage& storage, int num);

    std::string& label;
    double& min;
    double& max;
    bool& lockMin;
    bool& lockMax;
    bool apply = false;
    bool& autoFit;
    bool& logScale;
    bool& invert;
    bool& opposite;
    bool& gridLines;
    bool& tickMarks;
    bool& tickLabels;
  };
  std::vector<PlotAxis> m_axis;
  ImPlotRange m_xaxisRange;  // read from plot, used for lockPrevX
};

class PlotView : public View {
 public:
  PlotView(PlotProvider* provider, Storage& storage);

  void Display() override;
  void Settings() override;
  bool HasSettings() override;

  void MovePlot(PlotView* fromView, size_t fromIndex, size_t toIndex);

  void MovePlotSeries(PlotView* fromView, size_t fromPlotIndex,
                      size_t fromSeriesIndex, size_t toPlotIndex,
                      size_t toSeriesIndex, int yAxis = -1);

  PlotProvider* m_provider;
  std::vector<std::unique_ptr<Storage>>& m_plotsStorage;
  std::vector<std::unique_ptr<Plot>> m_plots;
};

}  // namespace

PlotSeries::PlotSeries(Storage& storage)
    : m_id{storage.GetString("id")},
      m_name{storage.GetString("name")},
      m_yAxis{storage.GetInt("yAxis", 0)},
      m_color{storage.GetFloatArray("color", kDefaultColor)},
      m_marker{storage.GetString("marker"),
               0,
               {"None", "Circle", "Square", "Diamond", "Up", "Down", "Left",
                "Right", "Cross", "Plus", "Asterisk"}},
      m_weight{storage.GetFloat("weight", IMPLOT_AUTO)},
      m_digital{
          storage.GetString("digital"), kAuto, {"Auto", "Digital", "Analog"}},
      m_digitalBitHeight{storage.GetInt("digitalBitHeight", 8)},
      m_digitalBitGap{storage.GetInt("digitalBitGap", 4)} {}

PlotSeries::PlotSeries(Storage& storage, std::string_view id)
    : PlotSeries{storage} {
  m_id = id;
  if (DataSource* source = DataSource::Find(id)) {
    SetSource(source);
    return;
  }
  CheckSource();
}

PlotSeries::PlotSeries(Storage& storage, DataSource* source)
    : PlotSeries{storage} {
  SetSource(source);
  m_id = source->GetId();
}

PlotSeries::PlotSeries(Storage& storage, DataSource* source, int yAxis)
    : PlotSeries{storage, source} {
  m_yAxis = yAxis;
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

  // add initial value
  AppendValue(source->GetValue(), 0);

  m_newValueConn = source->valueChanged.connect_connection(
      [this](double value, int64_t time) { AppendValue(value, time); });
}

void PlotSeries::AppendValue(double value, int64_t timeUs) {
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

const char* PlotSeries::GetName() const {
  if (!m_name.empty()) {
    return m_name.c_str();
  }
  if (m_newValueConn.connected()) {
    auto& sourceName = m_source->GetName();
    if (!sourceName.empty()) {
      return sourceName.c_str();
    }
  }
  return m_id.c_str();
}

PlotSeries::Action PlotSeries::EmitPlot(PlotView& view, double now, size_t i,
                                        size_t plotIndex) {
  CheckSource();

  char label[128];
  wpi::format_to_n_c_str(label, sizeof(label), "{}###name{}_{}", GetName(),
                         static_cast<int>(i), static_cast<int>(plotIndex));

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
  auto getter = [](int idx, void* data) {
    auto d = static_cast<GetterData*>(data);
    if (idx == d->size) {
      return ImPlotPoint{
          d->now - d->zeroTime,
          d->data[d->offset == 0 ? d->size - 1 : d->offset - 1].y};
    }
    ImPlotPoint* point;
    if (d->offset + idx < d->size) {
      point = &d->data[d->offset + idx];
    } else {
      point = &d->data[d->offset + idx - d->size];
    }
    return ImPlotPoint{point->x - d->zeroTime, point->y};
  };

  if (m_color.GetColorFloat()[3] == IMPLOT_AUTO) {
    SetColor(ImPlot::GetColormapColor(i));
  }
  ImPlot::SetNextLineStyle(m_color.GetColor(), m_weight);
  if (IsDigital()) {
    ImPlot::PushStyleVar(ImPlotStyleVar_DigitalBitHeight, m_digitalBitHeight);
    ImPlot::PushStyleVar(ImPlotStyleVar_DigitalBitGap, m_digitalBitGap);
    ImPlot::PlotDigitalG(label, getter, &getterData, size + 1);
    ImPlot::PopStyleVar();
    ImPlot::PopStyleVar();
  } else {
    if (ImPlot::GetCurrentPlot()->YAxis(m_yAxis).Enabled) {
      ImPlot::SetAxis(ImAxis_Y1 + m_yAxis);
    } else {
      ImPlot::SetAxis(ImAxis_Y1);
    }
    ImPlot::SetNextMarkerStyle(m_marker.GetValue() - 1);
    ImPlot::PlotLineG(label, getter, &getterData, size + 1);
  }

  // DND source for PlotSeries
  if (ImPlot::BeginDragDropSourceItem(label)) {
    EmitDragDropPayload(view, i, plotIndex);
    ImPlot::EndDragDropSource();
  }

  // Show full source name tooltip
  if (!m_name.empty() && ImPlot::IsLegendEntryHovered(label)) {
    ImGui::SetTooltip("%s", m_id.c_str());
  }

  // Edit settings via popup
  Action rv = kNone;
  if (ImPlot::BeginLegendPopup(label)) {
    ImGui::TextUnformatted(m_id.c_str());
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
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
    m_color.ColorEdit3("Color", ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    if (ImGui::Button("Default")) {
      SetColor(ImPlot::GetColormapColor(i));
    }
  }

  // Line weight
  {
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    ImGui::InputFloat("Weight", &m_weight, 0.1f, 1.0f, "%.1f");
  }

  // Digital
  {
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    m_digital.Combo("Digital");
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
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
      m_marker.Combo("Marker");
    }
  }
}

Plot::PlotAxis::PlotAxis(Storage& storage, int num)
    : label{storage.GetString("label")},
      min{storage.GetDouble("min", 0)},
      max{storage.GetDouble("max", 1)},
      lockMin{storage.GetBool("lockMin", false)},
      lockMax{storage.GetBool("lockMax", false)},
      autoFit{storage.GetBool("autoFit", false)},
      logScale{storage.GetBool("logScale", false)},
      invert{storage.GetBool("invert", false)},
      opposite{storage.GetBool("opposite", num != 0)},
      gridLines{storage.GetBool("gridLines", num == 0)},
      tickMarks{storage.GetBool("tickMarks", true)},
      tickLabels{storage.GetBool("tickLabels", true)} {}

Plot::Plot(Storage& storage)
    : m_seriesStorage{storage.GetChildArray("series")},
      m_name{storage.GetString("name")},
      m_visible{storage.GetBool("visible", true)},
      m_backgroundColor{
          storage.GetFloatArray("backgroundColor", kDefaultBackgroundColor)},
      m_showPause{storage.GetBool("showPause", true)},
      m_lockPrevX{storage.GetBool("lockPrevX", false)},
      m_legend{storage.GetBool("legend", true)},
      m_legendOutside{storage.GetBool("legendOutside", false)},
      m_legendHorizontal{storage.GetBool("legendHorizontal", false)},
      m_legendLocation{
          storage.GetInt("legendLocation", ImPlotLocation_NorthWest)},
      m_crosshairs{storage.GetBool("crosshairs", false)},
      m_mousePosition{storage.GetBool("mousePosition", true)},
      m_yAxis2{storage.GetBool("yaxis2", false)},
      m_yAxis3{storage.GetBool("yaxis3", false)},
      m_viewTime{storage.GetFloat("viewTime", 10)},
      m_autoHeight{storage.GetBool("autoHeight", true)},
      m_height{storage.GetInt("height", 300)} {
  auto& axesStorage = storage.GetChildArray("axis");
  axesStorage.resize(kAxisCount);
  for (int i = 0; i < kAxisCount; ++i) {
    if (!axesStorage[i]) {
      axesStorage[i] = std::make_unique<Storage>();
    }
    m_axis.emplace_back(*axesStorage[i], i);
  }

  // loop over series
  for (auto&& v : m_seriesStorage) {
    m_series.emplace_back(
        std::make_unique<PlotSeries>(*v, v->ReadString("id")));
  }
}

void Plot::DragDropAccept(PlotView& view, size_t i, int yAxis) {
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
      m_seriesStorage.emplace_back(std::make_unique<Storage>());
      m_series.emplace_back(std::make_unique<PlotSeries>(
          *m_seriesStorage.back(), source, yAxis == -1 ? 0 : yAxis));
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

void Plot::DragDropTarget(PlotView& view, size_t i, bool inPlot) {
  if (inPlot) {
    if (ImPlot::BeginDragDropTargetPlot() ||
        ImPlot::BeginDragDropTargetLegend()) {
      DragDropAccept(view, i, -1);
      ImPlot::EndDragDropTarget();
    }
    for (int y = 0; y < kAxisCount; ++y) {
      if (ImPlot::GetCurrentPlot()->YAxis(y).Enabled &&
          ImPlot::BeginDragDropTargetAxis(ImAxis_Y1 + y)) {
        DragDropAccept(view, i, y);
        ImPlot::EndDragDropTarget();
      }
    }
  } else if (ImGui::BeginDragDropTarget()) {
    DragDropAccept(view, i, -1);
    ImGui::EndDragDropTarget();
  }
}

void Plot::EmitPlot(PlotView& view, double now, bool paused, size_t i) {
  if (!m_visible) {
    return;
  }

  bool lockX = (i != 0 && m_lockPrevX);

  if (!lockX && m_showPause && ImGui::Button(m_paused ? "Resume" : "Pause")) {
    m_paused = !m_paused;
  }

  char label[128];
  wpi::format_to_n_c_str(label, sizeof(label), "{}###plot{}", m_name,
                         static_cast<int>(i));

  ImPlotFlags plotFlags = (m_legend ? 0 : ImPlotFlags_NoLegend) |
                          (m_crosshairs ? ImPlotFlags_Crosshairs : 0) |
                          (m_mousePosition ? 0 : ImPlotFlags_NoMouseText);

  if (ImPlot::BeginPlot(label, ImVec2(-1, m_height), plotFlags)) {
    if (m_backgroundColor.GetColorFloat()[3] == IMPLOT_AUTO) {
      SetColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
    }
    ImPlot::PushStyleColor(ImPlotCol_PlotBg, m_backgroundColor.GetColor());

    // setup legend
    if (m_legend) {
      ImPlotLegendFlags legendFlags =
          (m_legendOutside ? ImPlotLegendFlags_Outside : 0) |
          (m_legendHorizontal ? ImPlotLegendFlags_Horizontal : 0);
      ImPlot::SetupLegend(m_legendLocation, legendFlags);
    }

    // setup x axis
    ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoMenus);
    if (lockX) {
      ImPlot::SetupAxisLimits(ImAxis_X1, view.m_plots[i - 1]->m_xaxisRange.Min,
                              view.m_plots[i - 1]->m_xaxisRange.Max,
                              ImGuiCond_Always);
    } else {
      // also force-pause plots if overall timing is paused
      double zeroTime = GetZeroTime() * 1.0e-6;
      ImPlot::SetupAxisLimits(
          ImAxis_X1, now - zeroTime - m_viewTime, now - zeroTime,
          (paused || m_paused) ? ImGuiCond_Once : ImGuiCond_Always);
    }

    // setup y axes
    for (int i = 0; i < kAxisCount; ++i) {
      if ((i == 1 && !m_yAxis2) || (i == 2 && !m_yAxis3)) {
        continue;
      }
      ImPlotAxisFlags flags =
          (m_axis[i].lockMin ? ImPlotAxisFlags_LockMin : 0) |
          (m_axis[i].lockMax ? ImPlotAxisFlags_LockMax : 0) |
          (m_axis[i].autoFit ? ImPlotAxisFlags_AutoFit : 0) |
          (m_axis[i].invert ? ImPlotAxisFlags_Invert : 0) |
          (m_axis[i].opposite ? ImPlotAxisFlags_Opposite : 0) |
          (m_axis[i].gridLines ? 0 : ImPlotAxisFlags_NoGridLines) |
          (m_axis[i].tickMarks ? 0 : ImPlotAxisFlags_NoTickMarks) |
          (m_axis[i].tickLabels ? 0 : ImPlotAxisFlags_NoTickLabels);
      ImPlot::SetupAxis(
          ImAxis_Y1 + i,
          m_axis[i].label.empty() ? nullptr : m_axis[i].label.c_str(), flags);
      ImPlot::SetupAxisLimits(
          ImAxis_Y1 + i, m_axis[i].min, m_axis[i].max,
          m_axis[i].apply ? ImGuiCond_Always : ImGuiCond_Once);
      ImPlot::SetupAxisScale(ImAxis_Y1 + i, m_axis[i].logScale
                                                ? ImPlotScale_Log10
                                                : ImPlotScale_Linear);
      m_axis[i].apply = false;
    }

    ImPlot::SetupFinish();

    for (size_t j = 0; j < m_series.size(); ++j) {
      switch (m_series[j]->EmitPlot(view, now, j, i)) {
        case PlotSeries::kMoveUp:
          if (j > 0) {
            std::swap(m_seriesStorage[j - 1], m_seriesStorage[j]);
            std::swap(m_series[j - 1], m_series[j]);
          }
          break;
        case PlotSeries::kMoveDown:
          if (j < (m_series.size() - 1)) {
            std::swap(m_seriesStorage[j], m_seriesStorage[j + 1]);
            std::swap(m_series[j], m_series[j + 1]);
          }
          break;
        case PlotSeries::kDelete:
          m_seriesStorage.erase(m_seriesStorage.begin() + j);
          m_series.erase(m_series.begin() + j);
          break;
        default:
          break;
      }
    }
    DragDropTarget(view, i, true);
    m_xaxisRange = ImPlot::GetPlotLimits().X;

    ImPlotPlot* plot = ImPlot::GetCurrentPlot();

    ImPlot::PopStyleColor();
    ImPlot::EndPlot();

    // copy plot settings back to storage
    m_legend = (plot->Flags & ImPlotFlags_NoLegend) == 0;
    m_crosshairs = (plot->Flags & ImPlotFlags_Crosshairs) != 0;
    m_legendOutside =
        (plot->Items.Legend.Flags & ImPlotLegendFlags_Outside) != 0;
    m_legendHorizontal =
        (plot->Items.Legend.Flags & ImPlotLegendFlags_Horizontal) != 0;
    m_legendLocation = plot->Items.Legend.Location;

    for (int i = 0; i < kAxisCount; ++i) {
      if ((i == 1 && !m_yAxis2) || (i == 2 && !m_yAxis3)) {
        continue;
      }
      auto flags = plot->Axes[ImAxis_Y1 + i].Flags;
      m_axis[i].lockMin = (flags & ImPlotAxisFlags_LockMin) != 0;
      m_axis[i].lockMax = (flags & ImPlotAxisFlags_LockMax) != 0;
      m_axis[i].autoFit = (flags & ImPlotAxisFlags_AutoFit) != 0;
      m_axis[i].invert = (flags & ImPlotAxisFlags_Invert) != 0;
      m_axis[i].opposite = (flags & ImPlotAxisFlags_Opposite) != 0;
      m_axis[i].gridLines = (flags & ImPlotAxisFlags_NoGridLines) == 0;
      m_axis[i].tickMarks = (flags & ImPlotAxisFlags_NoTickMarks) == 0;
      m_axis[i].tickLabels = (flags & ImPlotAxisFlags_NoTickLabels) == 0;
      m_axis[i].logScale = plot->Axes[ImAxis_Y1 + i].Scale == ImPlotScale_Log10;
    }
  }
}

void Plot::EmitSettingsLimits(int axis) {
  ImGui::Indent();
  ImGui::PushID(axis);

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 10);
  ImGui::InputText("Label", &m_axis[axis].label);
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 3.5);
  ImGui::InputDouble("Min", &m_axis[axis].min, 0, 0, "%.3f");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 3.5);
  ImGui::InputDouble("Max", &m_axis[axis].max, 0, 0, "%.3f");
  ImGui::SameLine();
  if (ImGui::Button("Apply")) {
    m_axis[axis].apply = true;
  }

  ImGui::TextUnformatted("Lock Axis");
  ImGui::SameLine();
  ImGui::Checkbox("Min##minlock", &m_axis[axis].lockMin);
  ImGui::SameLine();
  ImGui::Checkbox("Max##maxlock", &m_axis[axis].lockMax);

  ImGui::PopID();
  ImGui::Unindent();
}

void Plot::EmitSettings(size_t i) {
  ImGui::Text("Edit plot name:");
  ImGui::InputText("##editname", &m_name);
  ImGui::Checkbox("Visible", &m_visible);
  m_backgroundColor.ColorEdit3("Background color",
                               ImGuiColorEditFlags_NoInputs);
  ImGui::SameLine();
  if (ImGui::Button("Default")) {
    SetColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
  }
  ImGui::Checkbox("Show Pause Button", &m_showPause);
  if (i != 0) {
    ImGui::Checkbox("Lock X-axis to previous plot", &m_lockPrevX);
  }
  ImGui::TextUnformatted("Primary Y-Axis");
  EmitSettingsLimits(0);
  ImGui::Checkbox("2nd Y-Axis", &m_yAxis2);
  if (m_yAxis2) {
    EmitSettingsLimits(1);
  }
  ImGui::Checkbox("3rd Y-Axis", &m_yAxis3);
  if (m_yAxis3) {
    EmitSettingsLimits(2);
  }
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
  ImGui::InputFloat("View Time (s)", &m_viewTime, 0.1f, 1.0f, "%.1f");
  ImGui::Checkbox("Auto Height", &m_autoHeight);
  if (!m_autoHeight) {
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    if (ImGui::InputInt("Height", &m_height, 10)) {
      if (m_height < 0) {
        m_height = 0;
      }
    }
  }
}

int Plot::GetAutoBaseHeight(bool* isAuto, size_t i) {
  *isAuto = m_autoHeight;

  if (!m_visible) {
    return 0;
  }

  int height = m_autoHeight ? 0 : m_height;

  // Pause button
  if ((i == 0 || !m_lockPrevX) && m_showPause) {
    height += ImGui::GetFrameHeightWithSpacing();
  }

  return height;
}

PlotView::PlotView(PlotProvider* provider, Storage& storage)
    : m_provider{provider}, m_plotsStorage{storage.GetChildArray("plots")} {
  // loop over plots
  for (auto&& v : m_plotsStorage) {
    // create plot
    m_plots.emplace_back(std::make_unique<Plot>(*v));
  }
}

void PlotView::Display() {
  if (m_plots.empty()) {
    if (ImGui::Button("Add plot")) {
      m_plotsStorage.emplace_back(std::make_unique<Storage>());
      m_plots.emplace_back(std::make_unique<Plot>(*m_plotsStorage.back()));
    }

    // Make "add plot" button a DND target for Plot
    if (!ImGui::BeginDragDropTarget()) {
      return;
    }
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Plot")) {
      auto ref = static_cast<const PlotSeriesRef*>(payload->Data);
      MovePlot(ref->view, ref->plotIndex, 0);
    }
    return;
  }

  // Auto-size plots.  This requires two passes: the first pass to get the
  // total height, the second to actually set the height after averaging it
  // across all auto-sized heights.
  int availHeight = ImGui::GetContentRegionAvail().y;
  int numAuto = 0;
  for (size_t i = 0; i < m_plots.size(); ++i) {
    bool isAuto;
    availHeight -= m_plots[i]->GetAutoBaseHeight(&isAuto, i);
    availHeight -= ImGui::GetStyle().ItemSpacing.y;
    if (isAuto) {
      ++numAuto;
    }
  }
  if (numAuto > 0) {
    availHeight /= numAuto;
    for (size_t i = 0; i < m_plots.size(); ++i) {
      m_plots[i]->SetAutoHeight(availHeight);
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
    if (fromIndex == toIndex) {
      return;
    }

    auto st = std::move(m_plotsStorage[fromIndex]);
    m_plotsStorage.insert(m_plotsStorage.begin() + toIndex, std::move(st));
    m_plotsStorage.erase(m_plotsStorage.begin() + fromIndex +
                         (fromIndex > toIndex ? 1 : 0));

    auto val = std::move(m_plots[fromIndex]);
    m_plots.insert(m_plots.begin() + toIndex, std::move(val));
    m_plots.erase(m_plots.begin() + fromIndex + (fromIndex > toIndex ? 1 : 0));
  } else {
    auto st = std::move(fromView->m_plotsStorage[fromIndex]);
    m_plotsStorage.insert(m_plotsStorage.begin() + toIndex, std::move(st));
    fromView->m_plotsStorage.erase(fromView->m_plotsStorage.begin() +
                                   fromIndex);

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
      auto& seriesStorage = m_plots[fromPlotIndex]->m_seriesStorage;
      auto st = std::move(seriesStorage[fromSeriesIndex]);
      seriesStorage.insert(seriesStorage.begin() + toSeriesIndex,
                           std::move(st));
      seriesStorage.erase(seriesStorage.begin() + fromSeriesIndex +
                          (fromSeriesIndex > toSeriesIndex ? 1 : 0));

      auto& plotSeries = m_plots[fromPlotIndex]->m_series;
      auto val = std::move(plotSeries[fromSeriesIndex]);
      // only set Y-axis if actually set
      if (yAxis != -1) {
        val->SetYAxis(yAxis);
      }
      plotSeries.insert(plotSeries.begin() + toSeriesIndex, std::move(val));
      plotSeries.erase(plotSeries.begin() + fromSeriesIndex +
                       (fromSeriesIndex > toSeriesIndex ? 1 : 0));
    }
  } else {
    auto& fromPlot = *fromView->m_plots[fromPlotIndex];
    auto& toPlot = *m_plots[toPlotIndex];
    // always set Y-axis if moving plots
    fromPlot.m_series[fromSeriesIndex]->SetYAxis(yAxis == -1 ? 0 : yAxis);

    toPlot.m_seriesStorage.insert(
        toPlot.m_seriesStorage.begin() + toSeriesIndex,
        std::move(fromPlot.m_seriesStorage[fromSeriesIndex]));
    fromPlot.m_seriesStorage.erase(fromPlot.m_seriesStorage.begin() +
                                   fromSeriesIndex);

    toPlot.m_series.insert(toPlot.m_series.begin() + toSeriesIndex,
                           std::move(fromPlot.m_series[fromSeriesIndex]));
    fromPlot.m_series.erase(fromPlot.m_series.begin() + fromSeriesIndex);
  }
}

PlotProvider::PlotProvider(Storage& storage) : WindowManager{storage} {
  storage.SetCustomApply([this] {
    // loop over windows
    for (auto&& windowkv : m_storage.GetChildren()) {
      // get or create window
      auto win = GetOrAddWindow(windowkv.key(), true);
      if (!win) {
        continue;
      }

      // get or create view
      auto view = static_cast<PlotView*>(win->GetView());
      if (!view) {
        win->SetView(std::make_unique<PlotView>(this, windowkv.value()));
        view = static_cast<PlotView*>(win->GetView());
      }
    }
  });
  storage.SetCustomClear([this] {
    EraseWindows();
    m_storage.EraseChildren();
  });
}

void PlotView::Settings() {
  if (ImGui::Button("Add plot")) {
    m_plotsStorage.emplace_back(std::make_unique<Storage>());
    m_plots.emplace_back(std::make_unique<Plot>(*m_plotsStorage.back()));
  }

  for (size_t i = 0; i < m_plots.size(); ++i) {
    auto& plot = m_plots[i];
    ImGui::PushID(i);

    char name[64];
    if (!plot->GetName().empty()) {
      wpi::format_to_n_c_str(name, sizeof(name), "{}", plot->GetName().c_str());
    } else {
      wpi::format_to_n_c_str(name, sizeof(name), "Plot {}",
                             static_cast<int>(i));
    }

    char label[90];
    wpi::format_to_n_c_str(label, sizeof(label), "{}###header{}", name,
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
        if (i > 0) {
          std::swap(m_plotsStorage[i - 1], m_plotsStorage[i]);
          std::swap(m_plots[i - 1], plot);
        }
      }

      ImGui::SameLine();
      if (ImGui::Button("Move Down")) {
        if (i < (m_plots.size() - 1)) {
          std::swap(m_plotsStorage[i], m_plotsStorage[i + 1]);
          std::swap(plot, m_plots[i + 1]);
        }
      }

      ImGui::SameLine();
      if (ImGui::Button("Delete")) {
        m_plotsStorage.erase(m_plotsStorage.begin() + i);
        m_plots.erase(m_plots.begin() + i);
        ImGui::PopID();
        continue;
      }

      plot->EmitSettings(i);
    }

    ImGui::PopID();
  }
}

bool PlotView::HasSettings() {
  return true;
}

PlotProvider::~PlotProvider() = default;

void PlotProvider::DisplayMenu() {
  // use index-based loop due to possible RemoveWindow call
  for (size_t i = 0; i < m_windows.size(); ++i) {
    m_windows[i]->DisplayMenuItem();
    // provide method to destroy the plot window
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Destroy Plot Window")) {
        RemoveWindow(i);
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  if (ImGui::MenuItem("New Plot Window")) {
    // this is an inefficient algorithm, but the number of windows is small
    char id[32];
    size_t numWindows = m_windows.size();
    for (size_t i = 0; i <= numWindows; ++i) {
      wpi::format_to_n_c_str(id, sizeof(id), "Plot <{}>", static_cast<int>(i));

      bool match = false;
      for (size_t j = 0; j < numWindows; ++j) {
        if (m_windows[j]->GetId() == id) {
          match = true;
          break;
        }
      }
      if (!match) {
        break;
      }
    }
    if (auto win = AddWindow(
            id, std::make_unique<PlotView>(this, m_storage.GetChild(id)))) {
      win->SetDefaultSize(700, 400);
    }
  }
}
