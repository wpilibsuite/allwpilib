/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <hal/Types.h>
#include <hal/Value.h>

#ifdef __cplusplus
#include <functional>

#include <imgui.h>
#endif

extern "C" {

void HALSIMGUI_DeviceTreeHide(const char* name);
void HALSIMGUI_DeviceTreeAdd(void* param, void (*execute)(void*));
HAL_Bool HALSIMGUI_DeviceTreeDisplayValue(const char* name, HAL_Bool readonly,
                                          struct HAL_Value* value,
                                          const char** options,
                                          int32_t numOptions);
HAL_Bool HALSIMGUI_DeviceTreeStartDevice(const char* label, int32_t flags);
void HALSIMGUI_DeviceTreeFinishDevice(void);

}  // extern "C"

#ifdef __cplusplus

namespace halsimgui {

class SimDeviceGui {
 public:
  static void Initialize();

  /**
   * Hides device on tree.
   *
   * @param name device name
   */
  static void Hide(const char* name);

  /**
   * Adds device to tree.  The execute function is called from within the
   * device tree window context on every frame, so it should implement an
   * TreeNodeEx() block for each device to display.
   *
   * @param execute execute function
   */
  static void Add(std::function<void()> execute);

  /**
   * Displays device value formatted the same way as SimDevice device values.
   *
   * @param name value name
   * @param readonly prevent value from being modified by the user
   * @param value value contents (modified in place)
   * @param options options array for enum values
   * @param numOptions size of options array for enum values
   * @return True if value was modified by the user
   */
  static bool DisplayValue(const char* name, bool readonly, HAL_Value* value,
                           const char** options = nullptr,
                           int32_t numOptions = 0);

  /**
   * Wraps ImGui::CollapsingHeader() to provide consistency and open
   * persistence.  As with the ImGui function, returns true if the tree node
   * is expanded.  If returns true, call StopDevice() to finish the block.
   *
   * @param label label
   * @param flags ImGuiTreeNodeFlags flags
   * @return True if expanded
   */
  static bool StartDevice(const char* label, ImGuiTreeNodeFlags flags = 0);

  /**
   * Finish a device block started with StartDevice().
   */
  static void FinishDevice();
};

}  // namespace halsimgui

#endif  // __cplusplus
