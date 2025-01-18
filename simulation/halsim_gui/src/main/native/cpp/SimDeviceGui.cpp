// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SimDeviceGui.h"

#include <stdint.h>

#include <memory>
#include <utility>

#include <fmt/format.h>
#include <glass/other/DeviceTree.h>
#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>
#include <wpi/DenseMap.h>
#include <wpi/StringExtras.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
#define DEFINE_SIMVALUESOURCE(Type, TYPE, v_type)                          \
  class Sim##Type##ValueSource : public glass::Type##Source {              \
   public:                                                                 \
    explicit Sim##Type##ValueSource(HAL_SimValueHandle handle,             \
                                    const char* device, const char* name)  \
        : Type##Source(fmt::format("{}-{}", device, name)),                \
          m_callback{HALSIM_RegisterSimValueChangedCallback(               \
              handle, this, CallbackFunc, true)} {}                        \
    ~Sim##Type##ValueSource() override {                                   \
      if (m_callback != 0) {                                               \
        HALSIM_CancelSimValueChangedCallback(m_callback);                  \
      }                                                                    \
    }                                                                      \
                                                                           \
   private:                                                                \
    static void CallbackFunc(const char*, void* param, HAL_SimValueHandle, \
                             int32_t, const HAL_Value* value) {            \
      auto source = static_cast<Sim##Type##ValueSource*>(param);           \
      if (value->type == HAL_##TYPE) {                                     \
        source->SetValue(value->data.v_##v_type);                          \
      }                                                                    \
    }                                                                      \
                                                                           \
    int32_t m_callback;                                                    \
  };

DEFINE_SIMVALUESOURCE(Boolean, BOOLEAN, boolean)
DEFINE_SIMVALUESOURCE(Double, DOUBLE, double)

class SimIntegerValueSource : public glass::IntegerSource {
 public:
  explicit SimIntegerValueSource(HAL_SimValueHandle handle, const char* device,
                                 const char* name)
      : IntegerSource(fmt::format("{}-{}", device, name)),
        m_callback{HALSIM_RegisterSimValueChangedCallback(
            handle, this, CallbackFunc, true)} {}
  ~SimIntegerValueSource() override {
    if (m_callback != 0) {
      HALSIM_CancelSimValueChangedCallback(m_callback);
    }
  }

 private:
  static void CallbackFunc(const char*, void* param, HAL_SimValueHandle,
                           int32_t, const HAL_Value* value) {
    auto source = static_cast<SimIntegerValueSource*>(param);
    if (value->type == HAL_ENUM) {
      source->SetValue(value->data.v_enum);
    } else if (value->type == HAL_INT) {
      source->SetValue(value->data.v_int);
    } else if (value->type == HAL_LONG) {
      source->SetValue(value->data.v_long);
    }
  }

  int32_t m_callback;
};

class SimDevicesModel : public glass::Model {
 public:
  void Update() override;
  bool Exists() override { return true; }

  glass::DataSource* GetSource(HAL_SimValueHandle handle) {
    return m_sources[handle].get();
  }

 private:
  wpi::DenseMap<HAL_SimValueHandle, std::unique_ptr<glass::DataSource>>
      m_sources;
};
}  // namespace

static SimDevicesModel* gSimDevicesModel;
static bool gSimDevicesShowPrefix = false;

void SimDevicesModel::Update() {
  HALSIM_EnumerateSimDevices(
      "", this, [](const char* name, void* self, HAL_SimDeviceHandle handle) {
        struct Data {
          SimDevicesModel* self;
          const char* device;
        } data = {static_cast<SimDevicesModel*>(self), name};
        HALSIM_EnumerateSimValues(
            handle, &data,
            [](const char* name, void* dataV, HAL_SimValueHandle handle,
               int32_t direction, const HAL_Value* value) {
              auto data = static_cast<Data*>(dataV);
              auto& source = data->self->m_sources[handle];
              switch (value->type) {
                case HAL_BOOLEAN:
                  if (!dynamic_cast<SimBooleanValueSource*>(source.get())) {
                    source = std::make_unique<SimBooleanValueSource>(
                        handle, data->device, name);
                  }
                  break;
                case HAL_DOUBLE:
                  if (!dynamic_cast<SimDoubleValueSource*>(source.get())) {
                    source = std::make_unique<SimDoubleValueSource>(
                        handle, data->device, name);
                  }
                  break;
                case HAL_ENUM:
                case HAL_INT:
                case HAL_LONG:
                  if (!dynamic_cast<SimIntegerValueSource*>(source.get())) {
                    source = std::make_unique<SimIntegerValueSource>(
                        handle, data->device, name);
                  }
                  break;
                default:
                  source.reset();
                  break;
              }
            });
      });
}

static void DisplaySimValue(const char* name, void* data,
                            HAL_SimValueHandle handle, int32_t direction,
                            const HAL_Value* value) {
  auto model = static_cast<SimDevicesModel*>(data);

  HAL_Value valueCopy = *value;

  switch (value->type) {
    case HAL_BOOLEAN: {
      bool v = value->data.v_boolean;
      if (glass::DeviceBoolean(name, direction == HAL_SimValueOutput, &v,
                               model->GetSource(handle))) {
        valueCopy.data.v_boolean = v ? 1 : 0;
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    }
    case HAL_DOUBLE:
      if (glass::DeviceDouble(name, direction == HAL_SimValueOutput,
                              &valueCopy.data.v_double,
                              model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    case HAL_ENUM: {
      int32_t numOptions = 0;
      const char** options = HALSIM_GetSimValueEnumOptions(handle, &numOptions);
      if (glass::DeviceEnum(name, direction == HAL_SimValueOutput,
                            &valueCopy.data.v_enum, options, numOptions,
                            model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    }
    case HAL_INT:
      if (glass::DeviceInt(name, direction == HAL_SimValueOutput,
                           &valueCopy.data.v_int, model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    case HAL_LONG:
      if (glass::DeviceLong(name, direction == HAL_SimValueOutput,
                            &valueCopy.data.v_long, model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    default:
      break;
  }
}

static void DisplaySimDevice(const char* name, void* data,
                             HAL_SimDeviceHandle handle) {
  std::string_view id{name};
  if (!gSimDevicesShowPrefix) {
    // only show "Foo" portion of "Accel:Foo"
    std::string_view type;
    std::tie(type, id) = wpi::split(id, ':');
    if (id.empty()) {
      id = type;
    }
  }
  if (glass::BeginDevice(id.data())) {
    HALSIM_EnumerateSimValues(handle, data, DisplaySimValue);
    glass::EndDevice();
  }
}

void SimDeviceGui::Initialize() {
  HALSimGui::halProvider->Register(
      "Other Devices", [] { return true; },
      [] { return std::make_unique<glass::DeviceTreeModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetDefaultPos(1025, 20);
        win->SetDefaultSize(250, 695);
        win->DisableRenamePopup();
        return glass::MakeFunctionView([=] {
          if (ImGui::BeginPopupContextItem()) {
            ImGui::Checkbox("Show prefix", &gSimDevicesShowPrefix);
            ImGui::EndPopup();
          }
          static_cast<glass::DeviceTreeModel*>(model)->Display();
        });
      });
  HALSimGui::halProvider->ShowDefault("Other Devices");

  auto model = std::make_unique<SimDevicesModel>();
  gSimDevicesModel = model.get();
  GetDeviceTree().Add(std::move(model), [](glass::Model* model) {
    HALSIM_EnumerateSimDevices("", static_cast<SimDevicesModel*>(model),
                               DisplaySimDevice);
  });
}

glass::DataSource* SimDeviceGui::GetValueSource(HAL_SimValueHandle handle) {
  return gSimDevicesModel->GetSource(handle);
}

glass::DeviceTreeModel& SimDeviceGui::GetDeviceTree() {
  static auto model = HALSimGui::halProvider->GetModel("Other Devices");
  assert(model);
  return *static_cast<glass::DeviceTreeModel*>(model);
}
