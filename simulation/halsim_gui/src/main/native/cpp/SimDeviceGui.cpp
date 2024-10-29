// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SimDeviceGui.h"

#include <stdint.h>

#include <memory>

#include <fmt/format.h>
#include <glass/other/DeviceTree.h>
#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>
#include <wpi/StringExtras.h>

#include "HALSimGui.h"

using namespace halsimgui;

SimValueSource::SimValueSource(HAL_SimValueHandle handle, const char* device,
                               const char* name)
    : DataSource(fmt::format("{}-{}", device, name)),
      m_callback{HALSIM_RegisterSimValueChangedCallback(handle, this,
                                                        CallbackFunc, true)} {}

SimValueSource::~SimValueSource() {
  if (m_callback != 0) {
    HALSIM_CancelSimValueChangedCallback(m_callback);
  }
}

void SimValueSource::CallbackFunc(const char*, void* param, HAL_SimValueHandle,
                                  int32_t, const HAL_Value* value) {
  auto source = static_cast<SimValueSource*>(param);
  if (value->type == HAL_BOOLEAN) {
    source->SetValue(value->data.v_boolean);
    source->SetDigital(true);
  } else if (value->type == HAL_DOUBLE) {
    source->SetValue(value->data.v_double);
    source->SetDigital(false);
  }
}

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
              if (!source) {
                source = std::make_unique<SimValueSource>(handle, data->device,
                                                          name);
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

static bool gSimDevicesShowPrefix = false;

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

void halsimgui::InitializeDeviceTree(glass::DeviceTreeModel& deviceTree,
                                     SimDevicesModel* model) {
  deviceTree.Add(model, [](glass::Model* model) {
    HALSIM_EnumerateSimDevices("", static_cast<SimDevicesModel*>(model),
                               DisplaySimDevice);
  });
}

void halsimgui::DisplayDeviceTree(glass::DeviceTreeModel* model) {
  if (ImGui::BeginPopupContextItem()) {
    ImGui::Checkbox("Show prefix", &gSimDevicesShowPrefix);
    ImGui::EndPopup();
  }
  model->Display();
}
