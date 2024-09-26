// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "EncoderSimGui.h"

#include <stdint.h>

#include <limits>
#include <memory>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <glass/hardware/Encoder.h>
#include <hal/Ports.h>
#include <hal/simulation/EncoderData.h>
#include <hal/simulation/SimDeviceData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {

class EncoderSimModel : public glass::EncoderModel {
 public:
  EncoderSimModel(std::string_view id, int32_t index, int channelA,
                  int channelB)
      : m_distancePerPulse(fmt::format("{} Dist/Count", id)),
        m_count(fmt::format("{} Count", id)),
        m_period(fmt::format("{} Period", id)),
        m_direction(fmt::format("{} Direction", id)),
        m_distance(fmt::format("{} Distance", id)),
        m_rate(fmt::format("{} Rate", id)),
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
    m_direction.SetDigital(true);
  }

  EncoderSimModel(int32_t index, int channelA, int channelB)
      : EncoderSimModel(fmt::format("Encoder[{},{}]", channelA, channelB),
                        index, channelA, channelB) {}

  explicit EncoderSimModel(int32_t index)
      : EncoderSimModel(index, HALSIM_GetEncoderDigitalChannelA(index),
                        HALSIM_GetEncoderDigitalChannelB(index)) {}

  ~EncoderSimModel() override {
    if (m_distancePerPulseCallback != 0) {
      HALSIM_CancelEncoderDistancePerPulseCallback(m_index,
                                                   m_distancePerPulseCallback);
    }
    if (m_countCallback != 0) {
      HALSIM_CancelEncoderCountCallback(m_index, m_countCallback);
    }
    if (m_periodCallback != 0) {
      HALSIM_CancelEncoderCountCallback(m_index, m_periodCallback);
    }
    if (m_directionCallback != 0) {
      HALSIM_CancelEncoderCountCallback(m_index, m_directionCallback);
    }
  }

  void Update() override {}

  bool Exists() override { return HALSIM_GetEncoderInitialized(m_index); }

  int32_t GetIndex() const { return m_index; }

  const char* GetSimDevice() const override {
    if (auto simDevice = HALSIM_GetEncoderSimDevice(m_index)) {
      return HALSIM_GetSimDeviceName(simDevice);
    } else {
      return nullptr;
    }
  }

  int GetChannelA() const override { return m_channelA; }
  int GetChannelB() const override { return m_channelB; }

  glass::DataSource* GetDistancePerPulseData() override {
    return &m_distancePerPulse;
  }
  glass::DataSource* GetCountData() override { return &m_count; }
  glass::DataSource* GetPeriodData() override { return &m_period; }
  glass::DataSource* GetDirectionData() override { return &m_direction; }
  glass::DataSource* GetDistanceData() override { return &m_distance; }
  glass::DataSource* GetRateData() override { return &m_rate; }

  double GetMaxPeriod() override { return HALSIM_GetEncoderMaxPeriod(m_index); }
  bool GetReverseDirection() override {
    return HALSIM_GetEncoderReverseDirection(m_index);
  }

  void SetDistancePerPulse(double val) override {
    HALSIM_SetEncoderDistancePerPulse(m_index, val);
  }
  void SetCount(int val) override { HALSIM_SetEncoderCount(m_index, val); }
  void SetPeriod(double val) override { HALSIM_SetEncoderPeriod(m_index, val); }
  void SetDirection(bool val) override {
    HALSIM_SetEncoderDirection(m_index, val);
  }
  void SetDistance(double val) override {
    HALSIM_SetEncoderDistance(m_index, val);
  }
  void SetRate(double val) override { HALSIM_SetEncoderRate(m_index, val); }

  void SetMaxPeriod(double val) override {
    HALSIM_SetEncoderMaxPeriod(m_index, val);
  }
  void SetReverseDirection(bool val) override {
    HALSIM_SetEncoderReverseDirection(m_index, val);
  }

 private:
  static void DistancePerPulseCallbackFunc(const char*, void* param,
                                           const HAL_Value* value) {
    if (value->type == HAL_DOUBLE) {
      auto self = static_cast<EncoderSimModel*>(param);
      double distPerPulse = value->data.v_double;
      self->m_distancePerPulse.SetValue(distPerPulse);
      self->m_distance.SetValue(self->m_count.GetValue() * distPerPulse);
      double period = self->m_period.GetValue();
      if (period == 0) {
        self->m_rate.SetValue(std::numeric_limits<double>::infinity());
      } else if (period == std::numeric_limits<double>::infinity()) {
        self->m_rate.SetValue(0);
      } else {
        self->m_rate.SetValue(static_cast<float>(distPerPulse / period));
      }
    }
  }

  static void CountCallbackFunc(const char*, void* param,
                                const HAL_Value* value) {
    if (value->type == HAL_INT) {
      auto self = static_cast<EncoderSimModel*>(param);
      double count = value->data.v_int;
      self->m_count.SetValue(count);
      self->m_distance.SetValue(count * self->m_distancePerPulse.GetValue());
    }
  }

  static void PeriodCallbackFunc(const char*, void* param,
                                 const HAL_Value* value) {
    if (value->type == HAL_DOUBLE) {
      auto self = static_cast<EncoderSimModel*>(param);
      double period = value->data.v_double;
      self->m_period.SetValue(period);
      if (period == 0) {
        self->m_rate.SetValue(std::numeric_limits<double>::infinity());
      } else if (period == std::numeric_limits<double>::infinity()) {
        self->m_rate.SetValue(0);
      } else {
        self->m_rate.SetValue(
            static_cast<float>(self->m_distancePerPulse.GetValue() / period));
      }
    }
  }

  static void DirectionCallbackFunc(const char*, void* param,
                                    const HAL_Value* value) {
    if (value->type == HAL_BOOLEAN) {
      static_cast<EncoderSimModel*>(param)->m_direction.SetValue(
          value->data.v_boolean);
    }
  }

  glass::DataSource m_distancePerPulse;
  glass::DataSource m_count;
  glass::DataSource m_period;
  glass::DataSource m_direction;
  glass::DataSource m_distance;
  glass::DataSource m_rate;

  int32_t m_index;
  int m_channelA;
  int m_channelB;
  int32_t m_distancePerPulseCallback;
  int32_t m_countCallback;
  int32_t m_periodCallback;
  int32_t m_directionCallback;
};

class EncodersSimModel : public glass::EncodersModel {
 public:
  EncodersSimModel() : m_models(HAL_GetNumEncoders()) {}

  void Update() override;

  bool Exists() override { return true; }

  void ForEachEncoder(
      wpi::function_ref<void(glass::EncoderModel& model, int index)> func)
      override;

 private:
  std::vector<std::unique_ptr<EncoderSimModel>> m_models;
};
}  // namespace

void EncodersSimModel::Update() {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    auto& model = m_models[i];
    if (HALSIM_GetEncoderInitialized(i)) {
      if (!model) {
        model = std::make_unique<EncoderSimModel>(i);
      }
    } else {
      model.reset();
    }
  }
}

void EncodersSimModel::ForEachEncoder(
    wpi::function_ref<void(glass::EncoderModel& model, int index)> func) {
  for (int32_t i = 0, iend = static_cast<int32_t>(m_models.size()); i < iend;
       ++i) {
    if (auto model = m_models[i].get()) {
      func(*model, i);
    }
  }
}

static bool EncodersAnyInitialized() {
  static const int32_t num = HAL_GetNumEncoders();
  for (int32_t i = 0; i < num; ++i) {
    if (HALSIM_GetEncoderInitialized(i)) {
      return true;
    }
  }
  return false;
}

void EncoderSimGui::Initialize() {
  HALSimGui::halProvider->Register(
      "Encoders", EncodersAnyInitialized,
      [] { return std::make_unique<EncodersSimModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(5, 250);
        return glass::MakeFunctionView(
            [=] { DisplayEncoders(static_cast<EncodersSimModel*>(model)); });
      });
}

glass::EncodersModel& EncoderSimGui::GetEncodersModel() {
  static auto model = HALSimGui::halProvider->GetModel("Encoders");
  assert(model);
  return *static_cast<glass::EncodersModel*>(model);
}
