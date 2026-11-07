// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/gui/SimDeviceGui.hpp"

#include <stdint.h>

#include <algorithm>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/glass/other/DeviceTree.hpp"
#include "wpi/hal/SimDevice.hpp"
#include "wpi/hal/simulation/SimDeviceData.h"
#include "wpi/halsim/gui/HALDataSource.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace halsimgui;

namespace {
#define DEFINE_SIMVALUESOURCE(Type, TYPE, v_type)                          \
  class Sim##Type##ValueSource : public wpi::glass::Type##Source {         \
   public:                                                                 \
    explicit Sim##Type##ValueSource(HAL_SimValueHandle handle,             \
                                    const char* device, const char* name)  \
        : Type##Source(std::format("{}-{}", device, name)),                \
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

class SimIntegerValueSource : public wpi::glass::IntegerSource {
 public:
  explicit SimIntegerValueSource(HAL_SimValueHandle handle, const char* device,
                                 const char* name)
      : IntegerSource(std::format("{}-{}", device, name)),
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

class SimDevicesModel : public wpi::glass::Model {
 public:
  void Update() override;
  bool Exists() override { return true; }

  wpi::glass::DataSource* GetSource(HAL_SimValueHandle handle) {
    return m_sources[handle].get();
  }

 private:
  wpi::util::DenseMap<HAL_SimValueHandle,
                      std::unique_ptr<wpi::glass::DataSource>>
      m_sources;
};

struct SimDeviceTreeDevice {
  std::string id;
  HAL_SimDeviceHandle handle;
};

struct SimDeviceTreeNode {
  SimDeviceTreeNode(std::string_view name, std::string_view id)
      : name{name}, id{id} {}

  std::string name;
  std::string id;
  std::vector<SimDeviceTreeDevice> devices;
  std::vector<SimDeviceTreeNode> children;
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
      if (wpi::glass::DeviceBoolean(name, direction == HAL_SIM_VALUE_OUTPUT, &v,
                                    model->GetSource(handle))) {
        valueCopy.data.v_boolean = v ? 1 : 0;
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    }
    case HAL_DOUBLE:
      if (wpi::glass::DeviceDouble(name, direction == HAL_SIM_VALUE_OUTPUT,
                                   &valueCopy.data.v_double,
                                   model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    case HAL_ENUM: {
      int32_t numOptions = 0;
      const char** options = HALSIM_GetSimValueEnumOptions(handle, &numOptions);
      if (wpi::glass::DeviceEnum(name, direction == HAL_SIM_VALUE_OUTPUT,
                                 &valueCopy.data.v_enum, options, numOptions,
                                 model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    }
    case HAL_INT:
      if (wpi::glass::DeviceInt(name, direction == HAL_SIM_VALUE_OUTPUT,
                                &valueCopy.data.v_int,
                                model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    case HAL_LONG:
      if (wpi::glass::DeviceLong(name, direction == HAL_SIM_VALUE_OUTPUT,
                                 &valueCopy.data.v_long,
                                 model->GetSource(handle))) {
        HAL_SetSimValue(handle, valueCopy);
      }
      break;
    default:
      break;
  }
}

static std::string GetDisplaySimDeviceId(const char* name) {
  std::string_view id{name};
  if (!gSimDevicesShowPrefix) {
    // only show "Foo" portion of "Accel:Foo"
    std::string_view type;
    std::tie(type, id) = wpi::util::split(id, ':');
    if (id.empty()) {
      id = type;
    }
  }

  return std::string{id};
}

static SimDeviceTreeNode& GetOrAddChild(SimDeviceTreeNode& node,
                                        std::string_view name) {
  auto childIt =
      std::find_if(node.children.begin(), node.children.end(),
                   [&](const auto& child) { return child.name == name; });
  if (childIt == node.children.end()) {
    std::string id = node.id;
    if (!id.empty()) {
      id += '/';
    }
    id += name;
    node.children.emplace_back(name, id);
    return node.children.back();
  }
  return *childIt;
}

static void AddSimDevice(SimDeviceTreeNode& root, std::string id,
                         HAL_SimDeviceHandle handle) {
  if (wpi::glass::IsDeviceHidden(id)) {
    return;
  }

  SimDeviceTreeNode* node = &root;
  wpi::util::split(id, '/', -1, false, [&](std::string_view segment) {
    node = &GetOrAddChild(*node, segment);
  });
  if (node == &root) {
    node = &GetOrAddChild(root, id);
  }
  node->devices.push_back({std::move(id), handle});
}

static void SortSimDeviceTree(SimDeviceTreeNode& node) {
  std::sort(
      node.children.begin(), node.children.end(),
      [](const auto& lhs, const auto& rhs) { return lhs.name < rhs.name; });

  for (auto&& child : node.children) {
    SortSimDeviceTree(child);
  }
}

static bool BeginSimDeviceTreeNode(const SimDeviceTreeNode& node) {
  auto flags = ImGuiTreeNodeFlags_DefaultOpen;
  bool& open = wpi::glass::GetStorage()
                   .GetChild("deviceTree")
                   .GetChild(node.id)
                   .GetBool("open", true);
  ImGui::SetNextItemOpen(open);

  std::string label = std::format("{}###{}", node.name, node.id);
  open = ImGui::CollapsingHeader(label.c_str(), flags);
  if (open) {
    ImGui::Indent();
  }
  return open;
}

static void EndSimDeviceTreeNode() {
  ImGui::Unindent();
}

static void DisplaySimDevice(SimDevicesModel* model,
                             const SimDeviceTreeDevice& device,
                             std::string_view label) {
  if (wpi::glass::BeginDevice(device.id, label)) {
    HALSIM_EnumerateSimValues(device.handle, model, DisplaySimValue);
    wpi::glass::EndDevice();
  }
}

static void DisplaySimDeviceTree(SimDevicesModel* model,
                                 const SimDeviceTreeNode& node) {
  if (node.children.empty()) {
    for (auto&& device : node.devices) {
      DisplaySimDevice(model, device, node.name);
    }
    return;
  }

  if (BeginSimDeviceTreeNode(node)) {
    for (auto&& device : node.devices) {
      DisplaySimDevice(model, device, node.name);
    }
    for (auto&& child : node.children) {
      DisplaySimDeviceTree(model, child);
    }
    EndSimDeviceTreeNode();
  }
}

static void DisplaySimDevices(SimDevicesModel* model) {
  SimDeviceTreeNode root{"", ""};
  HALSIM_EnumerateSimDevices(
      "", &root, [](const char* name, void* data, HAL_SimDeviceHandle handle) {
        AddSimDevice(*static_cast<SimDeviceTreeNode*>(data),
                     GetDisplaySimDeviceId(name), handle);
      });

  SortSimDeviceTree(root);

  for (auto&& child : root.children) {
    DisplaySimDeviceTree(model, child);
  }
}

void SimDeviceGui::Initialize() {
  HALSimGui::halProvider->Register(
      "Other Devices", [] { return true; },
      [] { return std::make_unique<wpi::glass::DeviceTreeModel>(); },
      [](wpi::glass::Window* win, wpi::glass::Model* model) {
        win->SetDefaultPos(1025, 20);
        win->SetDefaultSize(250, 695);
        win->DisableRenamePopup();
        return wpi::glass::MakeFunctionView([=] {
          if (ImGui::BeginPopupContextItem()) {
            ImGui::Checkbox("Show prefix", &gSimDevicesShowPrefix);
            ImGui::EndPopup();
          }
          static_cast<wpi::glass::DeviceTreeModel*>(model)->Display();
        });
      });
  HALSimGui::halProvider->ShowDefault("Other Devices");

  auto model = std::make_unique<SimDevicesModel>();
  gSimDevicesModel = model.get();
  GetDeviceTree().Add(std::move(model), [](wpi::glass::Model* model) {
    DisplaySimDevices(static_cast<SimDevicesModel*>(model));
  });
}

wpi::glass::DataSource* SimDeviceGui::GetValueSource(
    HAL_SimValueHandle handle) {
  return gSimDevicesModel->GetSource(handle);
}

wpi::glass::DeviceTreeModel& SimDeviceGui::GetDeviceTree() {
  static auto model = HALSimGui::halProvider->GetModel("Other Devices");
  assert(model);
  return *static_cast<wpi::glass::DeviceTreeModel*>(model);
}
