/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DriverStationGui.h"

#include <atomic>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <hal/simulation/DriverStationData.h>
#include <hal/simulation/MockHooks.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <wpi/Format.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "ExtraGuiWidgets.h"
#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {

struct SystemJoystick {
  bool present = false;
  int axisCount = 0;
  const float* axes = nullptr;
  int buttonCount = 0;
  const unsigned char* buttons = nullptr;
  int hatCount = 0;
  const unsigned char* hats = nullptr;
  const char* name = nullptr;
  bool isGamepad = false;
  GLFWgamepadstate gamepadState;

  bool anyButtonPressed = false;

  void Update(int i);
};

struct RobotJoystick {
  NameInfo name;
  std::string guid;
  const SystemJoystick* sys = nullptr;
  bool useGamepad = false;

  HAL_JoystickDescriptor desc;
  HAL_JoystickAxes axes;
  HAL_JoystickButtons buttons;
  HAL_JoystickPOVs povs;

  void Clear();
  void Update();
  void SetHAL(int i);
  void GetHAL(int i);
  bool IsButtonPressed(int i) { return (buttons.buttons & (1u << i)) != 0; }
};

class JoystickSource {
 public:
  explicit JoystickSource(int index);
  ~JoystickSource() {
    HALSIM_CancelDriverStationNewDataCallback(m_callback);
    for (int i = 0; i < buttonCount; ++i) delete buttons[i];
  }
  JoystickSource(const JoystickSource&) = delete;
  JoystickSource& operator=(const JoystickSource&) = delete;

  int axisCount;
  int buttonCount;
  int povCount;
  std::unique_ptr<GuiDataSource> axes[HAL_kMaxJoystickAxes];
  // use pointer instead of unique_ptr to allow it to be passed directly
  // to DrawLEDSources()
  GuiDataSource* buttons[32];
  std::unique_ptr<GuiDataSource> povs[HAL_kMaxJoystickPOVs];

 private:
  static void CallbackFunc(const char*, void* param, const HAL_Value*);

  int m_index;
  int32_t m_callback;
};
}  // namespace

// system joysticks
static SystemJoystick gSystemJoysticks[GLFW_JOYSTICK_LAST + 1];
static int gNumSystemJoysticks = 0;

// robot joysticks
static RobotJoystick gRobotJoysticks[HAL_kMaxJoysticks];
static std::unique_ptr<JoystickSource> gJoystickSources[HAL_kMaxJoysticks];

static bool gDisableDS = false;
static bool gZeroDisconnectedJoysticks = true;
static std::atomic<bool>* gDSSocketConnected = nullptr;

static inline bool IsDSDisabled() {
  return gDisableDS || (gDSSocketConnected && *gDSSocketConnected);
}

JoystickSource::JoystickSource(int index) : m_index{index} {
  HAL_JoystickAxes halAxes;
  HALSIM_GetJoystickAxes(index, &halAxes);
  axisCount = halAxes.count;
  for (int i = 0; i < axisCount; ++i) {
    axes[i] = std::make_unique<GuiDataSource>("Joystick[" + wpi::Twine{index} +
                                              "] Axis[" + wpi::Twine{i} +
                                              wpi::Twine{']'});
  }

  HAL_JoystickButtons halButtons;
  HALSIM_GetJoystickButtons(index, &halButtons);
  buttonCount = halButtons.count;
  for (int i = 0; i < buttonCount; ++i) {
    buttons[i] =
        new GuiDataSource("Joystick[" + wpi::Twine{index} + "] Button[" +
                          wpi::Twine{i + 1} + wpi::Twine{']'});
    buttons[i]->SetDigital(true);
  }
  for (int i = buttonCount; i < 32; ++i) buttons[i] = nullptr;

  HAL_JoystickPOVs halPOVs;
  HALSIM_GetJoystickPOVs(index, &halPOVs);
  povCount = halPOVs.count;
  for (int i = 0; i < povCount; ++i) {
    povs[i] = std::make_unique<GuiDataSource>("Joystick[" + wpi::Twine{index} +
                                              "] POV[" + wpi::Twine{i} +
                                              wpi::Twine{']'});
  }

  m_callback =
      HALSIM_RegisterDriverStationNewDataCallback(CallbackFunc, this, true);
}

void JoystickSource::CallbackFunc(const char*, void* param, const HAL_Value*) {
  auto self = static_cast<JoystickSource*>(param);

  HAL_JoystickAxes halAxes;
  HALSIM_GetJoystickAxes(self->m_index, &halAxes);
  for (int i = 0; i < halAxes.count; ++i) {
    if (auto axis = self->axes[i].get()) axis->SetValue(halAxes.axes[i]);
  }

  HAL_JoystickButtons halButtons;
  HALSIM_GetJoystickButtons(self->m_index, &halButtons);
  for (int i = 0; i < halButtons.count; ++i) {
    if (auto button = self->buttons[i])
      button->SetValue((halButtons.buttons & (1u << i)) != 0 ? 1 : 0);
  }

  HAL_JoystickPOVs halPOVs;
  HALSIM_GetJoystickPOVs(self->m_index, &halPOVs);
  for (int i = 0; i < halPOVs.count; ++i) {
    if (auto pov = self->povs[i].get()) pov->SetValue(halPOVs.povs[i]);
  }
}

// read/write joystick mapping to ini file
static void* JoystickReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                              const char* name) {
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) return nullptr;
  if (num < 0 || num >= HAL_kMaxJoysticks) return nullptr;
  return &gRobotJoysticks[num];
}

static void JoystickReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                             void* entry, const char* lineStr) {
  RobotJoystick* joy = static_cast<RobotJoystick*>(entry);
  // format: guid=guid or useGamepad=0/1
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (name == "guid") {
    joy->guid = value;
  } else if (name == "useGamepad") {
    int num;
    if (value.getAsInteger(10, num)) return;
    joy->useGamepad = num;
  } else {
    joy->name.ReadIni(name, value);
  }
}

static void JoystickWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                             ImGuiTextBuffer* out_buf) {
  for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
    auto& joy = gRobotJoysticks[i];
    if (!joy.name.HasName() && !joy.sys) continue;
    out_buf->appendf("[Joystick][%d]\nuseGamepad=%d\n", i,
                     joy.useGamepad ? 1 : 0);
    if (joy.name.HasName()) joy.name.WriteIni(out_buf);
    if (joy.sys) {
      const char* guid = glfwGetJoystickGUID(joy.sys - gSystemJoysticks);
      if (guid) out_buf->appendf("guid=%s\n", guid);
    }
    out_buf->append("\n");
  }
}

// read/write DS settings to ini file
static void* DriverStationReadOpen(ImGuiContext* ctx,
                                   ImGuiSettingsHandler* handler,
                                   const char* name) {
  if (name == wpi::StringRef{"Main"}) return &gDisableDS;
  return nullptr;
}

static void DriverStationReadLine(ImGuiContext* ctx,
                                  ImGuiSettingsHandler* handler, void* entry,
                                  const char* lineStr) {
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
  if (name == "disable") {
    int num;
    if (value.getAsInteger(10, num)) return;
    gDisableDS = num;
  } else if (name == "zeroDisconnectedJoysticks") {
    int num;
    if (value.getAsInteger(10, num)) return;
    gZeroDisconnectedJoysticks = num;
  }
}

static void DriverStationWriteAll(ImGuiContext* ctx,
                                  ImGuiSettingsHandler* handler,
                                  ImGuiTextBuffer* out_buf) {
  out_buf->appendf(
      "[DriverStation][Main]\ndisable=%d\nzeroDisconnectedJoysticks=%d\n\n",
      gDisableDS ? 1 : 0, gZeroDisconnectedJoysticks ? 1 : 0);
}

void SystemJoystick::Update(int i) {
  bool wasPresent = present;
  present = glfwJoystickPresent(i);

  if (!present) return;
  axes = glfwGetJoystickAxes(i, &axisCount);
  buttons = glfwGetJoystickButtons(i, &buttonCount);
  hats = glfwGetJoystickHats(i, &hatCount);
  isGamepad = glfwGetGamepadState(i, &gamepadState);

  anyButtonPressed = false;
  for (int j = 0; j < buttonCount; ++j) {
    if (buttons[j]) {
      anyButtonPressed = true;
      break;
    }
  }
  for (int j = 0; j < hatCount; ++j) {
    if (hats[j] != GLFW_HAT_CENTERED) {
      anyButtonPressed = true;
      break;
    }
  }

  if (!present || wasPresent) return;
  name = glfwGetJoystickName(i);

  // try to find matching GUID
  if (const char* guid = glfwGetJoystickGUID(i)) {
    for (auto&& joy : gRobotJoysticks) {
      if (guid == joy.guid) {
        joy.sys = &gSystemJoysticks[i];
        joy.guid.clear();
        break;
      }
    }
  }
}

static int HatToAngle(unsigned char hat) {
  switch (hat) {
    case GLFW_HAT_UP:
      return 0;
    case GLFW_HAT_RIGHT:
      return 90;
    case GLFW_HAT_DOWN:
      return 180;
    case GLFW_HAT_LEFT:
      return 270;
    case GLFW_HAT_RIGHT_UP:
      return 45;
    case GLFW_HAT_RIGHT_DOWN:
      return 135;
    case GLFW_HAT_LEFT_UP:
      return 315;
    case GLFW_HAT_LEFT_DOWN:
      return 225;
    default:
      return -1;
  }
}

void RobotJoystick::Clear() {
  std::memset(&desc, 0, sizeof(desc));
  desc.type = -1;
  std::memset(&axes, 0, sizeof(axes));
  std::memset(&buttons, 0, sizeof(buttons));
  std::memset(&povs, 0, sizeof(povs));
}

void RobotJoystick::Update() {
  Clear();

  if (!sys || !sys->present) return;

  // use gamepad mappings if present and enabled
  const float* sysAxes;
  const unsigned char* sysButtons;
  if (sys->isGamepad && useGamepad) {
    sysAxes = sys->gamepadState.axes;
    // don't remap on windows
#ifdef _WIN32
    sysButtons = sys->buttons;
#else
    sysButtons = sys->gamepadState.buttons;
#endif
  } else {
    sysAxes = sys->axes;
    sysButtons = sys->buttons;
  }

  // copy into HAL structures
  desc.isXbox = sys->isGamepad ? 1 : 0;
  desc.type = sys->isGamepad ? 21 : 20;
  std::strncpy(desc.name, sys->name, 256);
  desc.axisCount = (std::min)(sys->axisCount, HAL_kMaxJoystickAxes);
  // desc.axisTypes ???
  desc.buttonCount = (std::min)(sys->buttonCount, 32);
  desc.povCount = (std::min)(sys->hatCount, HAL_kMaxJoystickPOVs);

  buttons.count = desc.buttonCount;
  for (int j = 0; j < buttons.count; ++j)
    buttons.buttons |= (sysButtons[j] ? 1u : 0u) << j;

  axes.count = desc.axisCount;
  if (sys->isGamepad && useGamepad) {
    // the FRC DriverStation maps gamepad (XInput) trigger values to 0-1 range
    // on axis 2 and 3.
    axes.axes[0] = sysAxes[0];
    axes.axes[1] = sysAxes[1];
    axes.axes[2] = 0.5 + sysAxes[4] / 2.0;
    axes.axes[3] = 0.5 + sysAxes[5] / 2.0;
    axes.axes[4] = sysAxes[2];
    axes.axes[5] = sysAxes[3];

    // the start button for gamepads is not mapped on the FRC DriverStation
    // platforms, so remove it if present
    if (buttons.count == 11) {
      --desc.buttonCount;
      --buttons.count;
      buttons.buttons =
          (buttons.buttons & 0xff) | ((buttons.buttons >> 1) & 0x300);
    }
  } else {
    std::memcpy(axes.axes, sysAxes, axes.count * sizeof(&axes.axes[0]));
  }

  povs.count = desc.povCount;
  for (int j = 0; j < povs.count; ++j) povs.povs[j] = HatToAngle(sys->hats[j]);
}

void RobotJoystick::SetHAL(int i) {
  if (!gZeroDisconnectedJoysticks && (!sys || !sys->present)) return;
  // set at HAL level
  HALSIM_SetJoystickDescriptor(i, &desc);
  HALSIM_SetJoystickAxes(i, &axes);
  HALSIM_SetJoystickButtons(i, &buttons);
  HALSIM_SetJoystickPOVs(i, &povs);
}

void RobotJoystick::GetHAL(int i) {
  HALSIM_GetJoystickDescriptor(i, &desc);
  HALSIM_GetJoystickAxes(i, &axes);
  HALSIM_GetJoystickButtons(i, &buttons);
  HALSIM_GetJoystickPOVs(i, &povs);
}

static void DriverStationExecute() {
  // update sources
  for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
    auto& source = gJoystickSources[i];
    int32_t axisCount, buttonCount, povCount;
    HALSIM_GetJoystickCounts(i, &axisCount, &buttonCount, &povCount);
    if (axisCount != 0 || buttonCount != 0 || povCount != 0) {
      if (!source || source->axisCount != axisCount ||
          source->buttonCount != buttonCount || source->povCount != povCount) {
        source.reset();
        source = std::make_unique<JoystickSource>(i);
      }
    } else {
      source.reset();
    }
  }

  static bool prevDisableDS = false;

  bool disableDS = IsDSDisabled();
  if (disableDS && !prevDisableDS) {
    HALSimGui::SetWindowVisibility("System Joysticks", HALSimGui::kDisabled);
  } else if (!disableDS && prevDisableDS) {
    HALSimGui::SetWindowVisibility("System Joysticks", HALSimGui::kShow);
  }
  prevDisableDS = disableDS;
  if (disableDS) return;

  double curTime = glfwGetTime();

  // update system joysticks
  for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i) {
    gSystemJoysticks[i].Update(i);
    if (gSystemJoysticks[i].present) gNumSystemJoysticks = i + 1;
  }

  // update robot joysticks
  for (auto&& joy : gRobotJoysticks) joy.Update();

  bool isEnabled = HALSIM_GetDriverStationEnabled();
  bool isAuto = HALSIM_GetDriverStationAutonomous();
  bool isTest = HALSIM_GetDriverStationTest();

  // Robot state
  {
    ImGui::SetNextWindowPos(ImVec2{5, 20}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Robot State", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Selectable("Disabled", !isEnabled))
      HALSIM_SetDriverStationEnabled(false);
    if (ImGui::Selectable("Autonomous", isEnabled && isAuto && !isTest)) {
      HALSIM_SetDriverStationAutonomous(true);
      HALSIM_SetDriverStationTest(false);
      HALSIM_SetDriverStationEnabled(true);
    }
    if (ImGui::Selectable("Teleoperated", isEnabled && !isAuto && !isTest)) {
      HALSIM_SetDriverStationAutonomous(false);
      HALSIM_SetDriverStationTest(false);
      HALSIM_SetDriverStationEnabled(true);
    }
    if (ImGui::Selectable("Test", isEnabled && isTest)) {
      HALSIM_SetDriverStationAutonomous(false);
      HALSIM_SetDriverStationTest(true);
      HALSIM_SetDriverStationEnabled(true);
    }
    ImGui::End();
  }

  // Update HAL
  for (int i = 0; i < HAL_kMaxJoysticks; ++i) gRobotJoysticks[i].SetHAL(i);

  // Send new data every 20 ms (may be slower depending on GUI refresh rate)
  static double lastNewDataTime = 0.0;
  if ((curTime - lastNewDataTime) > 0.02 && !HALSIM_IsTimingPaused()) {
    lastNewDataTime = curTime;
    HALSIM_NotifyDriverStationNewData();
  }
}

static void DisplayFMS() {
  bool fmsAttached = HALSIM_GetDriverStationFmsAttached();
  bool dsAttached = HALSIM_GetDriverStationDsAttached();
  static const char* stations[] = {"Red 1",  "Red 2",  "Red 3",
                                   "Blue 1", "Blue 2", "Blue 3"};
  int allianceStationId = HALSIM_GetDriverStationAllianceStationId();
  double matchTime = HALSIM_GetDriverStationMatchTime();
  HAL_MatchInfo matchInfo;
  HALSIM_GetMatchInfo(&matchInfo);

  if (IsDSDisabled()) {
    if (!HALSIM_GetDriverStationEnabled())
      ImGui::Text("Robot State: Disabled");
    else if (HALSIM_GetDriverStationTest())
      ImGui::Text("Robot State: Test");
    else if (HALSIM_GetDriverStationAutonomous())
      ImGui::Text("Robot State: Autonomous");
    else
      ImGui::Text("Robot State: Teleoperated");

    ImGui::Text("FMS Attached: %s", fmsAttached ? "Yes" : "No");
    ImGui::Text("DS Attached: %s", dsAttached ? "Yes" : "No");
    ImGui::Text("Alliance Station: %s", stations[allianceStationId]);
    ImGui::Text("Match Time: %.1f", matchTime);
    ImGui::Text("Game Specific: %s", matchInfo.gameSpecificMessage);
    return;
  }

  double curTime = glfwGetTime();

  // FMS Attached
  if (ImGui::Checkbox("FMS Attached", &fmsAttached))
    HALSIM_SetDriverStationFmsAttached(fmsAttached);

  // DS Attached
  if (ImGui::Checkbox("DS Attached", &dsAttached))
    HALSIM_SetDriverStationDsAttached(dsAttached);

  // Alliance Station
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  if (ImGui::Combo("Alliance Station", &allianceStationId, stations, 6))
    HALSIM_SetDriverStationAllianceStationId(
        static_cast<HAL_AllianceStationID>(allianceStationId));

  // Match Time
  static bool matchTimeEnabled = true;
  ImGui::Checkbox("Match Time Enabled", &matchTimeEnabled);

  static double startMatchTime = 0.0;
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  if (ImGui::InputDouble("Match Time", &matchTime, 0, 0, "%.1f",
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
    HALSIM_SetDriverStationMatchTime(matchTime);
    startMatchTime = curTime - matchTime;
  } else if (!HALSIM_GetDriverStationEnabled() || HALSIM_IsTimingPaused()) {
    startMatchTime = curTime - matchTime;
  } else if (matchTimeEnabled) {
    HALSIM_SetDriverStationMatchTime(curTime - startMatchTime);
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    HALSIM_SetDriverStationMatchTime(0.0);
    startMatchTime = curTime;
  }

  // Game Specific Message
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  if (ImGui::InputText("Game Specific",
                       reinterpret_cast<char*>(matchInfo.gameSpecificMessage),
                       sizeof(matchInfo.gameSpecificMessage),
                       ImGuiInputTextFlags_EnterReturnsTrue)) {
    matchInfo.gameSpecificMessageSize =
        std::strlen(reinterpret_cast<char*>(matchInfo.gameSpecificMessage));
    HALSIM_SetMatchInfo(&matchInfo);
  }
}

static void DisplaySystemJoysticks() {
  ImGui::Text("(Drag and drop to Joysticks)");
  int numShowJoysticks = gNumSystemJoysticks < 6 ? 6 : gNumSystemJoysticks;
  for (int i = 0; i < numShowJoysticks; ++i) {
    auto& joy = gSystemJoysticks[i];
    wpi::SmallString<128> label;
    wpi::raw_svector_ostream os(label);
    os << wpi::format("%d: %s", i, joy.name);

    // highlight if any buttons pressed
    if (joy.anyButtonPressed)
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
    ImGui::Selectable(label.c_str(), false,
                      joy.present ? ImGuiSelectableFlags_None
                                  : ImGuiSelectableFlags_Disabled);
    if (joy.anyButtonPressed) ImGui::PopStyleColor();

    // drag and drop sources are the low level joysticks
    if (ImGui::BeginDragDropSource()) {
      SystemJoystick* joyPtr = &joy;
      ImGui::SetDragDropPayload("Joystick", &joyPtr, sizeof(joyPtr));
      ImGui::Text("%d: %s", i, joy.name);
      ImGui::EndDragDropSource();
    }
  }
}

static void DisplayJoysticks() {
  bool disableDS = IsDSDisabled();
  // imgui doesn't size columns properly with autoresize, so force it
  ImGui::Dummy(ImVec2(ImGui::GetFontSize() * 10 * HAL_kMaxJoysticks, 0));

  ImGui::Columns(HAL_kMaxJoysticks, "Joysticks", false);
  for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
    auto& joy = gRobotJoysticks[i];
    char label[128];
    joy.name.GetLabel(label, sizeof(label), "Joystick", i);
    if (!disableDS && joy.sys) {
      ImGui::Selectable(label, false);
      if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("Joystick", &joy.sys, sizeof(joy.sys));
        ImGui::Text("%d: %s", static_cast<int>(joy.sys - gSystemJoysticks),
                    joy.sys->name);
        ImGui::EndDragDropSource();
      }
    } else {
      ImGui::Selectable(label, false, ImGuiSelectableFlags_Disabled);
    }
    if (!disableDS && ImGui::BeginDragDropTarget()) {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Joystick")) {
        IM_ASSERT(payload->DataSize == sizeof(SystemJoystick*));
        SystemJoystick* payload_sys =
            *static_cast<SystemJoystick* const*>(payload->Data);
        // clear it from the other joysticks
        for (auto&& joy2 : gRobotJoysticks) {
          if (joy2.sys == payload_sys) joy2.sys = nullptr;
        }
        joy.sys = payload_sys;
        joy.guid.clear();
        joy.useGamepad = false;
      }
      ImGui::EndDragDropTarget();
    }
    joy.name.PopupEditName(i);
    ImGui::NextColumn();
  }
  ImGui::Separator();

  for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
    auto& joy = gRobotJoysticks[i];
    auto source = gJoystickSources[i].get();

    if (disableDS) joy.GetHAL(i);

    if ((disableDS && joy.desc.type != 0) || (joy.sys && joy.sys->present)) {
      // update GUI display
      ImGui::PushID(i);
      if (disableDS) {
        ImGui::Text("%s", joy.desc.name);
        ImGui::Text("Gamepad: %s", joy.desc.isXbox ? "Yes" : "No");
      } else {
        ImGui::Text("%d: %s", static_cast<int>(joy.sys - gSystemJoysticks),
                    joy.sys->name);

        if (joy.sys->isGamepad) ImGui::Checkbox("Map gamepad", &joy.useGamepad);
      }

      for (int j = 0; j < joy.axes.count; ++j) {
        if (source && source->axes[j]) {
          char label[64];
          std::snprintf(label, sizeof(label), "Axis[%d]", j);
          ImGui::Selectable(label);
          source->axes[j]->EmitDrag();
          ImGui::SameLine();
          ImGui::Text(": %.3f", joy.axes.axes[j]);
        } else {
          ImGui::Text("Axis[%d]: %.3f", j, joy.axes.axes[j]);
        }
      }

      for (int j = 0; j < joy.povs.count; ++j) {
        if (source && source->povs[j]) {
          char label[64];
          std::snprintf(label, sizeof(label), "POVs[%d]", j);
          ImGui::Selectable(label);
          source->povs[j]->EmitDrag();
          ImGui::SameLine();
          ImGui::Text(": %d", joy.povs.povs[j]);
        } else {
          ImGui::Text("POVs[%d]: %d", j, joy.povs.povs[j]);
        }
      }

      // show buttons as multiple lines of LED indicators, 8 per line
      static const ImU32 color = IM_COL32(255, 255, 102, 255);
      wpi::SmallVector<int, 64> buttons;
      buttons.resize(joy.buttons.count);
      for (int j = 0; j < joy.buttons.count; ++j)
        buttons[j] = joy.IsButtonPressed(j) ? 1 : -1;
      DrawLEDSources(buttons.data(), source ? source->buttons : nullptr,
                     buttons.size(), 8, &color);
      ImGui::PopID();
    } else {
      ImGui::Text("Unassigned");
    }
    ImGui::NextColumn();
  }
  ImGui::Columns(1);
}

static void DriverStationOptionMenu() {
  if (gDSSocketConnected && *gDSSocketConnected) {
    ImGui::MenuItem("Turn off DS (real DS connected)", nullptr, true, false);
  } else {
    ImGui::MenuItem("Turn off DS", nullptr, &gDisableDS);
    ImGui::MenuItem("Zero disconnected joysticks", nullptr,
                    &gZeroDisconnectedJoysticks, true);
  }
}

void DriverStationGui::Initialize() {
  // hook ini handler to save joystick settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "Joystick";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = JoystickReadOpen;
  iniHandler.ReadLineFn = JoystickReadLine;
  iniHandler.WriteAllFn = JoystickWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  // hook ini handler to save DS settings
  iniHandler.TypeName = "DriverStation";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = DriverStationReadOpen;
  iniHandler.ReadLineFn = DriverStationReadLine;
  iniHandler.WriteAllFn = DriverStationWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  HALSimGui::AddExecute(DriverStationExecute);
  HALSimGui::AddWindow("FMS", DisplayFMS, ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::AddWindow("System Joysticks", DisplaySystemJoysticks,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::AddWindow("Joysticks", DisplayJoysticks,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::AddOptionMenu(DriverStationOptionMenu);

  HALSimGui::SetDefaultWindowPos("FMS", 5, 540);
  HALSimGui::SetDefaultWindowPos("System Joysticks", 5, 385);
  HALSimGui::SetDefaultWindowPos("Joysticks", 250, 465);
}

void DriverStationGui::SetDSSocketExtension(void* data) {
  gDSSocketConnected = static_cast<std::atomic<bool>*>(data);
}
