/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SimDeviceGui.h"

#include <stdint.h>

#include <functional>
#include <memory>
#include <vector>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>
#include <imgui.h>
#include <wpi/DenseMap.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaverInfo.h"
#include "IniSaverString.h"

using namespace halsimgui;

namespace {

struct ElementInfo : public OpenInfo {
  bool ReadIni(wpi::StringRef name, wpi::StringRef value) {
    if (OpenInfo::ReadIni(name, value)) return true;
    return false;
  }
  void WriteIni(ImGuiTextBuffer* out) { OpenInfo::WriteIni(out); }

  const char* GetDisplayName(const char* defaultName,
                             HAL_SimDeviceHandle handle) const {
    const char* displayName = HALSIM_GetSimDeviceDisplayName(handle);
    if (displayName[0] == '\0') {
      return defaultName;
    } else {
      return displayName;
    }
  }

  bool visible = true;  // not saved
};

class SimValueSource : public GuiDataSource {
 public:
  explicit SimValueSource(HAL_SimValueHandle handle, const char* device,
                          const char* name)
      : GuiDataSource(wpi::Twine{device} + wpi::Twine{'-'} + name),
        m_callback{HALSIM_RegisterSimValueChangedCallback(
            handle, this, CallbackFunc, true)} {}
  ~SimValueSource() {
    if (m_callback != 0) HALSIM_CancelSimValueChangedCallback(m_callback);
  }

 private:
  static void CallbackFunc(const char*, void* param, HAL_SimValueHandle,
                           HAL_Bool, const HAL_Value* value) {
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

}  // namespace

static std::vector<std::function<void()>> gDeviceExecutors;
static IniSaverString<ElementInfo> gElements{"Device"};
static wpi::DenseMap<HAL_SimValueHandle, std::unique_ptr<SimValueSource>>
    gSimValueSources;

static void UpdateSimValueSources() {
  HALSIM_EnumerateSimDevices(
      "", nullptr, [](const char* name, void*, HAL_SimDeviceHandle handle) {
        HALSIM_EnumerateSimValues(
            handle, const_cast<char*>(name),
            [](const char* name, void* deviceV, HAL_SimValueHandle handle,
               HAL_Bool readonly, const HAL_Value* value) {
              auto device = static_cast<const char*>(deviceV);
              auto& source = gSimValueSources[handle];
              if (!source) {
                source = std::make_unique<SimValueSource>(handle, device, name);
              }
            });
      });
}

void SimDeviceGui::Hide(const char* name) { gElements[name].visible = false; }

void SimDeviceGui::Add(std::function<void()> execute) {
  if (execute) gDeviceExecutors.emplace_back(std::move(execute));
}

bool SimDeviceGui::StartDevice(const char* label, const char* displayName,
                               ImGuiTreeNodeFlags flags) {
  auto& element = gElements[label];
  if (!element.visible) return false;

  bool open = ImGui::CollapsingHeader(
      displayName,
      flags | (element.IsOpen() ? ImGuiTreeNodeFlags_DefaultOpen : 0));
  element.SetOpen(open);

  if (open) ImGui::PushID(label);
  return open;
}

void SimDeviceGui::FinishDevice() { ImGui::PopID(); }

static bool DisplayValueImpl(const char* name, bool readonly, HAL_Value* value,
                             const char** options, int32_t numOptions) {
  // read-only
  if (readonly) {
    switch (value->type) {
      case HAL_BOOLEAN:
        ImGui::LabelText(name, "%s", value->data.v_boolean ? "true" : "false");
        break;
      case HAL_DOUBLE:
        ImGui::LabelText(name, "%.6f", value->data.v_double);
        break;
      case HAL_ENUM: {
        int current = value->data.v_enum;
        if (current < 0 || current >= numOptions)
          ImGui::LabelText(name, "%d (unknown)", current);
        else
          ImGui::LabelText(name, "%s", options[current]);
        break;
      }
      case HAL_INT:
        ImGui::LabelText(name, "%d", static_cast<int>(value->data.v_int));
        break;
      case HAL_LONG:
        ImGui::LabelText(name, "%lld",
                         static_cast<long long int>(  // NOLINT(runtime/int)
                             value->data.v_long));
        break;
      default:
        break;
    }
    return false;
  }

  // writable
  switch (value->type) {
    case HAL_BOOLEAN: {
      static const char* boolOptions[] = {"false", "true"};
      int val = value->data.v_boolean ? 1 : 0;
      if (ImGui::Combo(name, &val, boolOptions, 2)) {
        value->data.v_boolean = val;
        return true;
      }
      break;
    }
    case HAL_DOUBLE: {
      if (ImGui::InputDouble(name, &value->data.v_double, 0, 0, "%.6f",
                             ImGuiInputTextFlags_EnterReturnsTrue))
        return true;
      break;
    }
    case HAL_ENUM: {
      int current = value->data.v_enum;
      if (ImGui::Combo(name, &current, options, numOptions)) {
        value->data.v_enum = current;
        return true;
      }
      break;
    }
    case HAL_INT: {
      if (ImGui::InputScalar(name, ImGuiDataType_S32, &value->data.v_int,
                             nullptr, nullptr, nullptr,
                             ImGuiInputTextFlags_EnterReturnsTrue))
        return true;
      break;
    }
    case HAL_LONG: {
      if (ImGui::InputScalar(name, ImGuiDataType_S64, &value->data.v_long,
                             nullptr, nullptr, nullptr,
                             ImGuiInputTextFlags_EnterReturnsTrue))
        return true;
      break;
    }
    default:
      break;
  }
  return false;
}

static bool DisplayValueSourceImpl(const char* name, bool readonly,
                                   HAL_Value* value,
                                   const GuiDataSource* source,
                                   const char** options, int32_t numOptions) {
  if (!source)
    return DisplayValueImpl(name, readonly, value, options, numOptions);
  ImGui::PushID(name);
  bool rv = DisplayValueImpl("", readonly, value, options, numOptions);
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Selectable(name);
  source->EmitDrag();
  ImGui::PopID();
  return rv;
}

bool SimDeviceGui::DisplayValue(const char* name, bool readonly,
                                HAL_Value* value, const char** options,
                                int32_t numOptions) {
  return DisplayValueSource(name, readonly, value, nullptr, options,
                            numOptions);
}

bool SimDeviceGui::DisplayValueSource(const char* name, bool readonly,
                                      HAL_Value* value,
                                      const GuiDataSource* source,
                                      const char** options,
                                      int32_t numOptions) {
  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
  return DisplayValueSourceImpl(name, readonly, value, source, options,
                                numOptions);
}

static void SimDeviceDisplayValue(const char* name, void*,
                                  HAL_SimValueHandle handle, HAL_Bool readonly,
                                  const HAL_Value* value) {
  int32_t numOptions = 0;
  const char** options = nullptr;

  if (value->type == HAL_ENUM)
    options = HALSIM_GetSimValueEnumOptions(handle, &numOptions);

  HAL_Value valueCopy = *value;
  if (DisplayValueSourceImpl(name, readonly, &valueCopy,
                             gSimValueSources[handle].get(), options,
                             numOptions))
    HAL_SetSimValue(handle, valueCopy);
}

static void SimDeviceDisplayDevice(const char* name, void*,
                                   HAL_SimDeviceHandle handle) {
  auto it = gElements.find(name);
  if (it != gElements.end() && !it->second.visible) return;

  const char* displayName = it->second.GetDisplayName(name, handle);

  if (SimDeviceGui::StartDevice(name, displayName)) {
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
    HALSIM_EnumerateSimValues(handle, nullptr, SimDeviceDisplayValue);
    ImGui::PopItemWidth();
    SimDeviceGui::FinishDevice();
  }
}

static void DisplayDeviceTree() {
  for (auto&& execute : gDeviceExecutors) {
    if (execute) execute();
  }
  HALSIM_EnumerateSimDevices("", nullptr, SimDeviceDisplayDevice);
}

void SimDeviceGui::Initialize() {
  gElements.Initialize();
  HALSimGui::AddExecute(UpdateSimValueSources);
  HALSimGui::AddWindow("Other Devices", DisplayDeviceTree);
  HALSimGui::SetDefaultWindowPos("Other Devices", 1025, 20);
  HALSimGui::SetDefaultWindowSize("Other Devices", 250, 695);
}

extern "C" {

void HALSIMGUI_DeviceTreeAdd(void* param, void (*execute)(void*)) {
  if (execute) SimDeviceGui::Add([=] { execute(param); });
}

void HALSIMGUI_DeviceTreeHide(const char* name) { SimDeviceGui::Hide(name); }

HAL_Bool HALSIMGUI_DeviceTreeDisplayValue(const char* name, HAL_Bool readonly,
                                          struct HAL_Value* value,
                                          const char** options,
                                          int32_t numOptions) {
  return SimDeviceGui::DisplayValue(name, readonly, value, options, numOptions);
}

}  // extern "C"
