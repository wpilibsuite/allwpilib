/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "EncoderGui.h"

#include <limits>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/EncoderData.h>
#include <hal/simulation/SimDeviceData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {

class EncoderNameAccessor {
 public:
  void GetLabel(char* buf, size_t size, const char* defaultName, int index,
                int channelA, int channelB) const {
    const char* displayName = HALSIM_GetEncoderDisplayName(index);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
    } else {
      std::snprintf(buf, size, "%s[%d,%d]###Name%d", defaultName, channelA,
                    channelB, index);
    }
  }
};

struct EncoderInfo : public EncoderNameAccessor, public OpenInfo {
  bool ReadIni(wpi::StringRef name, wpi::StringRef value) {
    if (OpenInfo::ReadIni(name, value)) return true;
    return false;
  }
  void WriteIni(ImGuiTextBuffer* out) { OpenInfo::WriteIni(out); }
};

class EncoderSource {
 public:
  EncoderSource(const wpi::Twine& id, int32_t index, int channelA, int channelB)
      : distancePerPulse(id + " Dist/Count"),
        count(id + " Count"),
        period(id + " Period"),
        direction(id + " Direction"),
        distance(id + " Distance"),
        rate(id + " Rate"),
        m_index{index},
        m_channelA{channelA},
        m_channelB{channelB},
        m_distancePerPulseCallback{
            HALSIM_RegisterEncoderDistancePerPulseCallback(
                index, DistancePerPulseCallbackFunc, this, true)},
        m_countCallback{HALSIM_RegisterEncoderCountCallback(
            index, CountCallbackFunc, this, true)},
        m_periodCallback{HALSIM_RegisterEncoderPeriodCallback(
            index, PeriodCallbackFunc, this, true)},
        m_directionCallback{HALSIM_RegisterEncoderDirectionCallback(
            index, DirectionCallbackFunc, this, true)} {
    direction.SetDigital(true);
  }

  EncoderSource(int32_t index, int channelA, int channelB)
      : EncoderSource("Encoder[" + wpi::Twine(channelA) + wpi::Twine(',') +
                          wpi::Twine(channelB) + wpi::Twine(']'),
                      index, channelA, channelB) {}

  explicit EncoderSource(int32_t index)
      : EncoderSource(index, HALSIM_GetEncoderDigitalChannelA(index),
                      HALSIM_GetEncoderDigitalChannelB(index)) {}

  ~EncoderSource() {
    if (m_distancePerPulseCallback != 0)
      HALSIM_CancelEncoderDistancePerPulseCallback(m_index,
                                                   m_distancePerPulseCallback);
    if (m_countCallback != 0)
      HALSIM_CancelEncoderCountCallback(m_index, m_countCallback);
    if (m_periodCallback != 0)
      HALSIM_CancelEncoderCountCallback(m_index, m_periodCallback);
    if (m_directionCallback != 0)
      HALSIM_CancelEncoderCountCallback(m_index, m_directionCallback);
  }

  void SetName(const wpi::Twine& name) {
    if (name.str().empty()) {
      distancePerPulse.SetName("");
      count.SetName("");
      period.SetName("");
      direction.SetName("");
      distance.SetName("");
      rate.SetName("");
    } else {
      distancePerPulse.SetName(name + " Distance/Count");
      count.SetName(name + " Count");
      period.SetName(name + " Period");
      direction.SetName(name + " Direction");
      distance.SetName(name + " Distance");
      rate.SetName(name + " Rate");
    }
  }

  int32_t GetIndex() const { return m_index; }
  int GetChannelA() const { return m_channelA; }
  int GetChannelB() const { return m_channelB; }

  GuiDataSource distancePerPulse;
  GuiDataSource count;
  GuiDataSource period;
  GuiDataSource direction;
  GuiDataSource distance;
  GuiDataSource rate;

 private:
  static void DistancePerPulseCallbackFunc(const char*, void* param,
                                           const HAL_Value* value) {
    if (value->type == HAL_DOUBLE) {
      auto self = static_cast<EncoderSource*>(param);
      double distPerPulse = value->data.v_double;
      self->distancePerPulse.SetValue(distPerPulse);
      self->distance.SetValue(self->count.GetValue() * distPerPulse);
      double period = self->period.GetValue();
      if (period == 0)
        self->rate.SetValue(std::numeric_limits<double>::infinity());
      else if (period == std::numeric_limits<double>::infinity())
        self->rate.SetValue(0);
      else
        self->rate.SetValue(static_cast<float>(distPerPulse / period));
    }
  }

  static void CountCallbackFunc(const char*, void* param,
                                const HAL_Value* value) {
    if (value->type == HAL_INT) {
      auto self = static_cast<EncoderSource*>(param);
      double count = value->data.v_int;
      self->count.SetValue(count);
      self->distance.SetValue(count * self->distancePerPulse.GetValue());
    }
  }

  static void PeriodCallbackFunc(const char*, void* param,
                                 const HAL_Value* value) {
    if (value->type == HAL_DOUBLE) {
      auto self = static_cast<EncoderSource*>(param);
      double period = value->data.v_double;
      self->period.SetValue(period);
      if (period == 0)
        self->rate.SetValue(std::numeric_limits<double>::infinity());
      else if (period == std::numeric_limits<double>::infinity())
        self->rate.SetValue(0);
      else
        self->rate.SetValue(
            static_cast<float>(self->distancePerPulse.GetValue() / period));
    }
  }

  static void DirectionCallbackFunc(const char*, void* param,
                                    const HAL_Value* value) {
    if (value->type == HAL_BOOLEAN) {
      static_cast<EncoderSource*>(param)->direction.SetValue(
          value->data.v_boolean);
    }
  }

  int32_t m_index;
  int m_channelA;
  int m_channelB;
  int32_t m_distancePerPulseCallback;
  int32_t m_countCallback;
  int32_t m_periodCallback;
  int32_t m_directionCallback;
};

}  // namespace

static IniSaver<EncoderInfo> gEncoders{"Encoder"};  // indexed by channel A
static std::vector<std::unique_ptr<EncoderSource>> gEncoderSources;

static void UpdateEncoderSources() {
  for (int i = 0, iend = gEncoderSources.size(); i < iend; ++i) {
    auto& source = gEncoderSources[i];
    if (HALSIM_GetEncoderInitialized(i)) {
      if (!source) {
        source = std::make_unique<EncoderSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayEncoders() {
  bool hasAny = false;
  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  for (int i = 0, iend = gEncoderSources.size(); i < iend; ++i) {
    if (auto source = gEncoderSources[i].get()) {
      hasAny = true;
      if (auto simDevice = HALSIM_GetEncoderSimDevice(i)) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        ImGui::Text("%s", HALSIM_GetSimDeviceName(simDevice));
        ImGui::PopStyleColor();
      } else {
        int chA = source->GetChannelA();
        int chB = source->GetChannelB();

        // build header name
        auto& info = gEncoders[chA];
        char name[128];
        info.GetLabel(name, sizeof(name), "Encoder", i, chA, chB);

        // header
        bool open = ImGui::CollapsingHeader(
            name, gEncoders[chA].IsOpen() ? ImGuiTreeNodeFlags_DefaultOpen : 0);
        info.SetOpen(open);

        if (open) {
          ImGui::PushID(i);
          // distance per pulse
          double distancePerPulse = source->distancePerPulse.GetValue();
          source->distancePerPulse.LabelText("Dist/Count", "%.6f",
                                             distancePerPulse);

          // count
          int count = source->count.GetValue();
          if (ImGui::InputInt("##input", &count))
            HALSIM_SetEncoderCount(i, count);
          ImGui::SameLine();
          if (ImGui::Button("Reset")) HALSIM_SetEncoderCount(i, 0);
          ImGui::SameLine();
          ImGui::Selectable("Count");
          source->count.EmitDrag();

          // max period
          double maxPeriod = HALSIM_GetEncoderMaxPeriod(i);
          ImGui::LabelText("Max Period", "%.6f", maxPeriod);

          // period
          double period = source->period.GetValue();
          if (source->period.InputDouble("Period", &period, 0, 0, "%.6g"))
            HALSIM_SetEncoderPeriod(i, period);

          // reverse direction
          ImGui::LabelText(
              "Reverse Direction", "%s",
              HALSIM_GetEncoderReverseDirection(i) ? "true" : "false");

          // direction
          static const char* options[] = {"reverse", "forward"};
          int direction = source->direction.GetValue() ? 1 : 0;
          if (source->direction.Combo("Direction", &direction, options, 2))
            HALSIM_SetEncoderDirection(i, direction);

          // distance
          double distance = source->distance.GetValue();
          if (source->distance.InputDouble("Distance", &distance, 0, 0, "%.6g"))
            HALSIM_SetEncoderDistance(i, distance);

          // rate
          double rate = source->rate.GetValue();
          if (source->rate.InputDouble("Rate", &rate, 0, 0, "%.6g"))
            HALSIM_SetEncoderRate(i, rate);

          ImGui::PopID();
        }
      }
    }
  }
  ImGui::PopItemWidth();
  if (!hasAny) ImGui::Text("No encoders");
}

void EncoderGui::Initialize() {
  gEncoders.Initialize();
  gEncoderSources.resize(HAL_GetNumEncoders());
  HALSimGui::AddExecute(UpdateEncoderSources);
  HALSimGui::AddWindow("Encoders", DisplayEncoders,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Encoders", 5, 250);
}
