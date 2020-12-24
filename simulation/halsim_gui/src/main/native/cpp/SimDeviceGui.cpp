/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SimDeviceGui.h"

#include <glass/other/DeviceTree.h>
#include <stdint.h>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>
#include <wpi/DenseMap.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
class SimValueSource : public glass::DataSource {
 public:
  explicit SimValueSource(HAL_SimValueHandle handle, const char* device,
                          const char* name)
      : DataSource(wpi::Twine{device} + wpi::Twine{'-'} + name),
        m_callback{HALSIM_RegisterSimValueChangedCallback(
            handle, this, CallbackFunc, true)} {}
  ~SimValueSource() {
    if (m_callback != 0) HALSIM_CancelSimValueChangedCallback(m_callback);
  }

 private:
  static void CallbackFunc(const char*, void* param, HAL_SimValueHandle,
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
  wpi::DenseMap<HAL_SimValueHandle, std::unique_ptr<SimValueSource>> m_sources;
};
}  // namespace

static SimDevicesModel* gSimDevicesModel;

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

static void DisplaySimDevice(const char* name, void* data,
                             HAL_SimDeviceHandle handle) {
  // only show "Foo" portion of "Accel:Foo"
  auto [type, id] = wpi::StringRef{name}.split(':');
  if (id.empty()) id = type;
  if (glass::BeginDevice(id.data())) {
    HALSIM_EnumerateSimValues(handle, data, DisplaySimValue);
    glass::EndDevice();
  }
}

void SimDeviceGui::Initialize() {
  HALSimGui::halProvider.Register(
      "Other Devices", [] { return true; },
      [] { return std::make_unique<glass::DeviceTreeModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetDefaultPos(1025, 20);
        win->SetDefaultSize(250, 695);
        return glass::MakeFunctionView(
            [=] { static_cast<glass::DeviceTreeModel*>(model)->Display(); });
      });
  HALSimGui::halProvider.ShowDefault("Other Devices");

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
  static auto model = HALSimGui::halProvider.GetModel("Other Devices");
  assert(model);
  return *static_cast<glass::DeviceTreeModel*>(model);
}
