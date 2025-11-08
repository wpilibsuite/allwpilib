// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/other/DeviceTree.hpp"

#include <cinttypes>
#include <string>

#include <imgui.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/ContextInternal.hpp"
#include "wpi/glass/DataSource.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::glass;

void DeviceTreeModel::Update() {
  for (auto&& display : m_displays) {
    if (display.first) {
      display.first->Update();
    }
  }
}

bool DeviceTreeModel::Exists() {
  for (auto&& display : m_displays) {
    if (display.first && display.first->Exists()) {
      return true;
    }
  }
  return false;
}

void DeviceTreeModel::Display() {
  for (auto&& display : m_displays) {
    if (display.second) {
      display.second(display.first);
    }
  }
}

void wpi::glass::HideDevice(const char* id) {
  gContext->deviceHidden[id] = true;
}

bool wpi::glass::BeginDevice(const char* id, ImGuiTreeNodeFlags flags) {
  if (gContext->deviceHidden[id]) {
    return false;
  }

  PushID(id);

  // build label
  std::string& name = GetStorage().GetString("name");
  char label[128];
  if (name.empty()) {
    wpi::util::format_to_n_c_str(label, sizeof(label), "{}###header", id);
  } else {
    wpi::util::format_to_n_c_str(label, sizeof(label), "{}###header", name);
  }

  bool open = CollapsingHeader(label, flags);
  PopupEditName("header", &name);

  if (!open) {
    PopID();
  }
  return open;
}

void wpi::glass::EndDevice() {
  PopID();
}

static bool DeviceBooleanImpl(const char* name, bool readonly, bool* value) {
  if (readonly) {
    ImGui::LabelText(name, "%s", *value ? "true" : "false");
  } else {
    static const char* boolOptions[] = {"false", "true"};
    int val = *value ? 1 : 0;
    if (ImGui::Combo(name, &val, boolOptions, 2)) {
      *value = val;
      return true;
    }
  }
  return false;
}

static bool DeviceDoubleImpl(const char* name, bool readonly, double* value) {
  if (readonly) {
    ImGui::LabelText(name, "%.6f", *value);
    return false;
  } else {
    return ImGui::InputDouble(name, value, 0, 0, "%.6f",
                              ImGuiInputTextFlags_EnterReturnsTrue);
  }
}

static bool DeviceEnumImpl(const char* name, bool readonly, int* value,
                           const char** options, int32_t numOptions) {
  if (readonly) {
    if (*value < 0 || *value >= numOptions) {
      ImGui::LabelText(name, "%d (unknown)", *value);
    } else {
      ImGui::LabelText(name, "%s", options[*value]);
    }
    return false;
  } else {
    return ImGui::Combo(name, value, options, numOptions);
  }
}

static bool DeviceIntImpl(const char* name, bool readonly, int32_t* value) {
  if (readonly) {
    ImGui::LabelText(name, "%" PRId32, *value);
    return false;
  } else {
    return ImGui::InputScalar(name, ImGuiDataType_S32, value, nullptr, nullptr,
                              nullptr, ImGuiInputTextFlags_EnterReturnsTrue);
  }
}

static bool DeviceLongImpl(const char* name, bool readonly, int64_t* value) {
  if (readonly) {
    ImGui::LabelText(name, "%" PRId64, *value);
    return false;
  } else {
    return ImGui::InputScalar(name, ImGuiDataType_S64, value, nullptr, nullptr,
                              nullptr, ImGuiInputTextFlags_EnterReturnsTrue);
  }
}

template <typename F, typename... Args>
static inline bool DeviceValueImpl(const char* name, bool readonly,
                                   const DataSource* source, F&& func,
                                   Args... args) {
  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
  if (!source) {
    return func(name, readonly, args...);
  } else {
    ImGui::PushID(name);
    bool rv = func("", readonly, args...);
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Selectable(name);
    source->EmitDrag();
    ImGui::PopID();
    return rv;
  }
}

bool wpi::glass::DeviceBoolean(const char* name, bool readonly, bool* value,
                          const DataSource* source) {
  return DeviceValueImpl(name, readonly, source, DeviceBooleanImpl, value);
}

bool wpi::glass::DeviceDouble(const char* name, bool readonly, double* value,
                         const DataSource* source) {
  return DeviceValueImpl(name, readonly, source, DeviceDoubleImpl, value);
}

bool wpi::glass::DeviceEnum(const char* name, bool readonly, int* value,
                       const char** options, int32_t numOptions,
                       const DataSource* source) {
  return DeviceValueImpl(name, readonly, source, DeviceEnumImpl, value, options,
                         numOptions);
}

bool wpi::glass::DeviceInt(const char* name, bool readonly, int32_t* value,
                      const DataSource* source) {
  return DeviceValueImpl(name, readonly, source, DeviceIntImpl, value);
}

bool wpi::glass::DeviceLong(const char* name, bool readonly, int64_t* value,
                       const DataSource* source) {
  return DeviceValueImpl(name, readonly, source, DeviceLongImpl, value);
}
