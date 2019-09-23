/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SimDeviceGui.h"

#include <stdint.h>

#include <vector>

#include <hal/SimDevice.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <mockdata/SimDeviceData.h>
#include <wpi/StringMap.h>

#include "HALSimGui.h"

using namespace halsimgui;

namespace {
struct ElementInfo {
  bool open = false;
  bool visible = true;
};
}  // namespace

static std::vector<std::function<void()>> gDeviceExecutors;
static wpi::StringMap<ElementInfo> gElements;

// read/write open state to ini file
static void* DevicesReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                             const char* name) {
  return &gElements[name];
}

static void DevicesReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                            void* entry, const char* lineStr) {
  ElementInfo* element = static_cast<ElementInfo*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (name == "open") {
    int num;
    if (value.getAsInteger(10, num)) return;
    element->open = num;
  }
}

static void DevicesWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                            ImGuiTextBuffer* out_buf) {
  for (auto&& entry : gElements) {
    out_buf->appendf("[Device][%s]\nopen=%d\n\n", entry.getKey().data(),
                     entry.getValue().open ? 1 : 0);
  }
}

void SimDeviceGui::Hide(const char* name) { gElements[name].visible = false; }

void SimDeviceGui::Add(std::function<void()> execute) {
  if (execute) gDeviceExecutors.emplace_back(std::move(execute));
}

bool SimDeviceGui::StartDevice(const char* label, ImGuiTreeNodeFlags flags) {
  auto& element = gElements[label];
  if (!element.visible) return false;

  if (ImGui::CollapsingHeader(
          label, flags | (element.open ? ImGuiTreeNodeFlags_DefaultOpen : 0))) {
    ImGui::PushID(label);
    element.open = true;
    return true;
  }
  element.open = false;
  return false;
}

void SimDeviceGui::FinishDevice() { ImGui::PopID(); }

bool DisplayValueImpl(const char* name, bool readonly, HAL_Value* value,
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

bool SimDeviceGui::DisplayValue(const char* name, bool readonly,
                                HAL_Value* value, const char** options,
                                int32_t numOptions) {
  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
  return DisplayValueImpl(name, readonly, value, options, numOptions);
}

static void SimDeviceDisplayValue(const char* name, void*,
                                  HAL_SimValueHandle handle, HAL_Bool readonly,
                                  const HAL_Value* value) {
  int32_t numOptions = 0;
  const char** options = nullptr;

  if (value->type == HAL_ENUM)
    options = HALSIM_GetSimValueEnumOptions(handle, &numOptions);

  HAL_Value valueCopy = *value;
  if (DisplayValueImpl(name, readonly, &valueCopy, options, numOptions))
    HAL_SetSimValue(handle, valueCopy);
}

static void SimDeviceDisplayDevice(const char* name, void*,
                                   HAL_SimDeviceHandle handle) {
  auto it = gElements.find(name);
  if (it != gElements.end() && !it->second.visible) return;

  if (SimDeviceGui::StartDevice(name)) {
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
  // hook ini handler to save device settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "Device";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = DevicesReadOpen;
  iniHandler.ReadLineFn = DevicesReadLine;
  iniHandler.WriteAllFn = DevicesWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

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

HAL_Bool HALSIMGUI_DeviceTreeStartDevice(const char* label, int32_t flags) {
  return SimDeviceGui::StartDevice(label, flags);
}

void HALSIMGUI_DeviceTreeFinishDevice(void) { SimDeviceGui::FinishDevice(); }

}  // extern "C"
