// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <imgui.h>
#include <wpi/FunctionExtras.h>

#include "glass/Model.h"

namespace glass {

class DataSource;

/**
 * Model for device tree.
 */
class DeviceTreeModel : public Model {
 public:
  using DisplayFunc = wpi::unique_function<void(Model*)>;

  /**
   * Add a display to the device tree.
   *
   * @param model Model to keep updated (may be nullptr)
   * @param display Display function
   */
  void Add(std::unique_ptr<Model> model, DisplayFunc display) {
    m_displays.emplace_back(model.get(), std::move(display));
    m_ownedModels.emplace_back(std::move(model));
  }

  void Add(Model* model, DisplayFunc display) {
    m_displays.emplace_back(model, std::move(display));
  }

  void Update() override;

  bool Exists() override;

  void Display();

 private:
  std::vector<std::pair<Model*, DisplayFunc>> m_displays;
  std::vector<std::unique_ptr<Model>> m_ownedModels;
};

/**
 * Hides device on tree.
 *
 * @param id device name
 */
void HideDevice(const char* id);

/**
 * Wraps CollapsingHeader() to provide both hiding functionality and open
 * persistence.  As with the ImGui function, returns true if the tree node
 * is visible and expanded.  If returns true, call EndDevice() to finish
 * the block.
 *
 * @param id label
 * @param flags ImGuiTreeNodeFlags flags
 * @return True if expanded
 */
bool BeginDevice(const char* id, ImGuiTreeNodeFlags flags = 0);

/**
 * Finish a device block started with BeginDevice().
 */
void EndDevice();

/**
 * Displays device value.
 *
 * @param name value name
 * @param readonly prevent value from being modified by the user
 * @param value value contents (modified in place)
 * @param source data source for drag source (may be nullptr)
 * @return True if value was modified by the user
 */
bool DeviceBoolean(const char* name, bool readonly, bool* value,
                   const DataSource* source = nullptr);

/**
 * Displays device value.
 *
 * @param name value name
 * @param readonly prevent value from being modified by the user
 * @param value value contents (modified in place)
 * @param source data source for drag source (may be nullptr)
 * @return True if value was modified by the user
 */
bool DeviceDouble(const char* name, bool readonly, double* value,
                  const DataSource* source = nullptr);

/**
 * Displays device value.
 *
 * @param name value name
 * @param readonly prevent value from being modified by the user
 * @param value value contents (modified in place)
 * @param options options array
 * @param numOptions size of options array
 * @param source data source for drag source (may be nullptr)
 * @return True if value was modified by the user
 */
bool DeviceEnum(const char* name, bool readonly, int* value,
                const char** options, int32_t numOptions,
                const DataSource* source = nullptr);

/**
 * Displays device value.
 *
 * @param name value name
 * @param readonly prevent value from being modified by the user
 * @param value value contents (modified in place)
 * @param source data source for drag source (may be nullptr)
 * @return True if value was modified by the user
 */
bool DeviceInt(const char* name, bool readonly, int32_t* value,
               const DataSource* source = nullptr);

/**
 * Displays device value.
 *
 * @param name value name
 * @param readonly prevent value from being modified by the user
 * @param value value contents (modified in place)
 * @param source data source for drag source (may be nullptr)
 * @return True if value was modified by the user
 */
bool DeviceLong(const char* name, bool readonly, int64_t* value,
                const DataSource* source = nullptr);

}  // namespace glass
