// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DriverStationGui.h"

#include <glass/other/FMS.h>
#include <glass/support/ExtraGuiWidgets.h>
#include <glass/support/IniSaverInfo.h>

#include <algorithm>
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
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>
#include <wpigui.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {

struct HALJoystickData {
  HALJoystickData() {
    std::memset(&desc, 0, sizeof(desc));
    desc.type = -1;
    std::memset(&axes, 0, sizeof(axes));
    std::memset(&buttons, 0, sizeof(buttons));
    std::memset(&povs, 0, sizeof(povs));
  }
  HAL_JoystickDescriptor desc;
  HAL_JoystickAxes axes;
  HAL_JoystickButtons buttons;
  HAL_JoystickPOVs povs;
};

class SystemJoystick {
 public:
  virtual ~SystemJoystick() = default;

  bool IsPresent() const { return m_present; }
  bool IsAnyButtonPressed() const { return m_anyButtonPressed; }
  bool IsGamepad() const { return m_isGamepad; }

  virtual void SettingsDisplay() {}
  virtual void Update() = 0;
  virtual const char* GetName() const = 0;
  virtual void GetData(HALJoystickData* data, bool mapGamepad) const = 0;
  virtual const char* GetGUID() const = 0;
  virtual int GetIndex() const = 0;

 protected:
  bool m_present = false;
  bool m_anyButtonPressed = false;
  bool m_isGamepad = false;
};

class GlfwSystemJoystick : public SystemJoystick {
 public:
  explicit GlfwSystemJoystick(int i) : m_index{i} {}

  void Update() override;
  const char* GetName() const override { return m_name ? m_name : "(null)"; }
  void GetData(HALJoystickData* data, bool mapGamepad) const override;
  const char* GetGUID() const override { return glfwGetJoystickGUID(m_index); }
  int GetIndex() const override { return m_index; }

 private:
  int m_index;
  int m_axisCount = 0;
  const float* m_axes = nullptr;
  int m_buttonCount = 0;
  const unsigned char* m_buttons = nullptr;
  int m_hatCount = 0;
  const unsigned char* m_hats = nullptr;
  const char* m_name = nullptr;
  GLFWgamepadstate m_gamepadState;
};

class KeyboardJoystick : public SystemJoystick {
 public:
  explicit KeyboardJoystick(int index);

  void SettingsDisplay() override;
  void Update() override;
  const char* GetName() const override { return m_name; }
  void GetData(HALJoystickData* data, bool mapGamepad) const override {
    *data = m_data;
  }
  const char* GetGUID() const override { return m_guid; }
  int GetIndex() const override { return m_index + GLFW_JOYSTICK_LAST + 1; }

  void ClearKey(int key);
  virtual const char* GetKeyName(int key) const = 0;

  void ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out_buf) const;

 protected:
  void EditKey(const char* label, int* key);

  int m_index;
  char m_name[20];
  char m_guid[20];

  static int* s_keyEdit;

  HALJoystickData m_data;

  struct AxisConfig {
    int incKey = -1;
    int decKey = -1;
    float keyRate = 0.05f;
    float decayRate = 0.05f;
    float maxAbsValue = 1.0f;
  };
  AxisConfig m_axisConfig[HAL_kMaxJoystickAxes];

  static constexpr int kMaxButtonCount = 32;
  int m_buttonKey[kMaxButtonCount];

  struct PovConfig {
    int key0 = -1;
    int key45 = -1;
    int key90 = -1;
    int key135 = -1;
    int key180 = -1;
    int key225 = -1;
    int key270 = -1;
    int key315 = -1;
  };

  PovConfig m_povConfig[HAL_kMaxJoystickPOVs];
};

class GlfwKeyboardJoystick : public KeyboardJoystick {
 public:
  explicit GlfwKeyboardJoystick(int index, bool noDefaults = false);

  const char* GetKeyName(int key) const override;
};

struct RobotJoystick {
  glass::NameInfo name;
  std::string guid;
  const SystemJoystick* sys = nullptr;
  bool useGamepad = false;

  HALJoystickData data;

  void Clear() { data = HALJoystickData{}; }
  void Update();
  void SetHAL(int i);
  void GetHAL(int i);
  bool IsButtonPressed(int i) {
    return (data.buttons.buttons & (1u << i)) != 0;
  }
};

class JoystickModel {
 public:
  explicit JoystickModel(int index);
  ~JoystickModel() {
    HALSIM_CancelDriverStationNewDataCallback(m_callback);
    for (int i = 0; i < buttonCount; ++i) {
      delete buttons[i];
    }
  }
  JoystickModel(const JoystickModel&) = delete;
  JoystickModel& operator=(const JoystickModel&) = delete;

  int axisCount;
  int buttonCount;
  int povCount;
  std::unique_ptr<glass::DataSource> axes[HAL_kMaxJoystickAxes];
  // use pointer instead of unique_ptr to allow it to be passed directly
  // to DrawLEDSources()
  glass::DataSource* buttons[32];
  std::unique_ptr<glass::DataSource> povs[HAL_kMaxJoystickPOVs];

 private:
  static void CallbackFunc(const char*, void* param, const HAL_Value*);

  int m_index;
  int32_t m_callback;
};

class FMSSimModel : public glass::FMSModel {
 public:
  FMSSimModel();

  glass::DataSource* GetFmsAttachedData() override { return &m_fmsAttached; }
  glass::DataSource* GetDsAttachedData() override { return &m_dsAttached; }
  glass::DataSource* GetAllianceStationIdData() override {
    return &m_allianceStationId;
  }
  glass::DataSource* GetMatchTimeData() override { return &m_matchTime; }
  glass::DataSource* GetEStopData() override { return &m_estop; }
  glass::DataSource* GetEnabledData() override { return &m_enabled; }
  glass::DataSource* GetTestData() override { return &m_test; }
  glass::DataSource* GetAutonomousData() override { return &m_autonomous; }
  wpi::StringRef GetGameSpecificMessage(
      wpi::SmallVectorImpl<char>& buf) override {
    HAL_MatchInfo info;
    HALSIM_GetMatchInfo(&info);
    buf.clear();
    buf.append(info.gameSpecificMessage,
               info.gameSpecificMessage + info.gameSpecificMessageSize);
    return wpi::StringRef(buf.begin(), buf.size());
  }

  void SetFmsAttached(bool val) override {
    HALSIM_SetDriverStationFmsAttached(val);
  }
  void SetDsAttached(bool val) override {
    HALSIM_SetDriverStationDsAttached(val);
  }
  void SetAllianceStationId(int val) override {
    HALSIM_SetDriverStationAllianceStationId(
        static_cast<HAL_AllianceStationID>(val));
  }
  void SetMatchTime(double val) override {
    HALSIM_SetDriverStationMatchTime(val);
    int32_t status = 0;
    m_startMatchTime = HAL_GetFPGATime(&status) * 1.0e-6 - val;
  }
  void SetEStop(bool val) override { HALSIM_SetDriverStationEStop(val); }
  void SetEnabled(bool val) override { HALSIM_SetDriverStationEnabled(val); }
  void SetTest(bool val) override { HALSIM_SetDriverStationTest(val); }
  void SetAutonomous(bool val) override {
    HALSIM_SetDriverStationAutonomous(val);
  }
  void SetGameSpecificMessage(const char* val) override {
    HALSIM_SetGameSpecificMessage(val);
  }

  void Update() override;

  bool Exists() override { return true; }

  bool IsReadOnly() override;

  bool m_matchTimeEnabled = true;

 private:
  glass::DataSource m_fmsAttached{"FMS:FMSAttached"};
  glass::DataSource m_dsAttached{"FMS:DSAttached"};
  glass::DataSource m_allianceStationId{"FMS:AllianceStationID"};
  glass::DataSource m_matchTime{"FMS:MatchTime"};
  glass::DataSource m_estop{"FMS:EStop"};
  glass::DataSource m_enabled{"FMS:RobotEnabled"};
  glass::DataSource m_test{"FMS:TestMode"};
  glass::DataSource m_autonomous{"FMS:AutonomousMode"};
  double m_startMatchTime = 0.0;
  double m_prevTime = 0.0;
};

}  // namespace

// system joysticks
static std::vector<std::unique_ptr<SystemJoystick>> gGlfwJoysticks;
static int gNumGlfwJoysticks = 0;
static std::vector<std::unique_ptr<GlfwKeyboardJoystick>> gKeyboardJoysticks;

// robot joysticks
static RobotJoystick gRobotJoysticks[HAL_kMaxJoysticks];
static std::unique_ptr<JoystickModel> gJoystickSources[HAL_kMaxJoysticks];

// FMS
static std::unique_ptr<FMSSimModel> gFMSModel;

// Window management
DSManager DriverStationGui::dsManager{"DSManager"};

static bool gDisableDS = false;
static bool gZeroDisconnectedJoysticks = true;
static bool gUseEnableDisableHotkeys = false;
static bool gUseEstopHotkey = false;
static std::atomic<bool>* gDSSocketConnected = nullptr;

static inline bool IsDSDisabled() {
  return gDisableDS || (gDSSocketConnected && *gDSSocketConnected);
}

JoystickModel::JoystickModel(int index) : m_index{index} {
  HAL_JoystickAxes halAxes;
  HALSIM_GetJoystickAxes(index, &halAxes);
  axisCount = halAxes.count;
  for (int i = 0; i < axisCount; ++i) {
    axes[i] = std::make_unique<glass::DataSource>(
        "Joystick[" + wpi::Twine{index} + "] Axis[" + wpi::Twine{i} +
        wpi::Twine{']'});
  }

  HAL_JoystickButtons halButtons;
  HALSIM_GetJoystickButtons(index, &halButtons);
  buttonCount = halButtons.count;
  for (int i = 0; i < buttonCount; ++i) {
    buttons[i] =
        new glass::DataSource("Joystick[" + wpi::Twine{index} + "] Button[" +
                              wpi::Twine{i + 1} + wpi::Twine{']'});
    buttons[i]->SetDigital(true);
  }
  for (int i = buttonCount; i < 32; ++i) {
    buttons[i] = nullptr;
  }

  HAL_JoystickPOVs halPOVs;
  HALSIM_GetJoystickPOVs(index, &halPOVs);
  povCount = halPOVs.count;
  for (int i = 0; i < povCount; ++i) {
    povs[i] = std::make_unique<glass::DataSource>(
        "Joystick[" + wpi::Twine{index} + "] POV[" + wpi::Twine{i} +
        wpi::Twine{']'});
  }

  m_callback =
      HALSIM_RegisterDriverStationNewDataCallback(CallbackFunc, this, true);
}

void JoystickModel::CallbackFunc(const char*, void* param, const HAL_Value*) {
  auto self = static_cast<JoystickModel*>(param);

  HAL_JoystickAxes halAxes;
  HALSIM_GetJoystickAxes(self->m_index, &halAxes);
  for (int i = 0; i < halAxes.count; ++i) {
    if (auto axis = self->axes[i].get()) {
      axis->SetValue(halAxes.axes[i]);
    }
  }

  HAL_JoystickButtons halButtons;
  HALSIM_GetJoystickButtons(self->m_index, &halButtons);
  for (int i = 0; i < halButtons.count; ++i) {
    if (auto button = self->buttons[i]) {
      button->SetValue((halButtons.buttons & (1u << i)) != 0 ? 1 : 0);
    }
  }

  HAL_JoystickPOVs halPOVs;
  HALSIM_GetJoystickPOVs(self->m_index, &halPOVs);
  for (int i = 0; i < halPOVs.count; ++i) {
    if (auto pov = self->povs[i].get()) {
      pov->SetValue(halPOVs.povs[i]);
    }
  }
}

// read/write joystick mapping to ini file
static void* JoystickReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                              const char* name) {
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) {
    return nullptr;
  }
  if (num < 0 || num >= HAL_kMaxJoysticks) {
    return nullptr;
  }
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
    if (value.getAsInteger(10, num)) {
      return;
    }
    joy->useGamepad = num;
  } else {
    joy->name.ReadIni(name, value);
  }
}

static void JoystickWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                             ImGuiTextBuffer* out_buf) {
  for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
    auto& joy = gRobotJoysticks[i];
    if (!joy.name.HasName() && !joy.sys) {
      continue;
    }
    out_buf->appendf("[Joystick][%d]\nuseGamepad=%d\n", i,
                     joy.useGamepad ? 1 : 0);
    if (joy.name.HasName()) {
      joy.name.WriteIni(out_buf);
    }
    if (joy.sys) {
      const char* guid = joy.sys->GetGUID();
      if (guid) {
        out_buf->appendf("guid=%s\n", guid);
      }
    }
    out_buf->append("\n");
  }
}

// read/write keyboard joystick mapping to ini file
static void* KeyboardJoystickReadOpen(ImGuiContext* ctx,
                                      ImGuiSettingsHandler* handler,
                                      const char* name) {
  int num;
  if (wpi::StringRef{name}.getAsInteger(10, num)) {
    return nullptr;
  }
  if (num < 0 || num >= static_cast<int>(gKeyboardJoysticks.size())) {
    return nullptr;
  }
  auto joy = gKeyboardJoysticks[num].get();
  *joy = GlfwKeyboardJoystick(num, true);
  return joy;
}

static void KeyboardJoystickReadLine(ImGuiContext* ctx,
                                     ImGuiSettingsHandler* handler, void* entry,
                                     const char* lineStr) {
  auto joy = static_cast<KeyboardJoystick*>(entry);
  // format: guid=guid or useGamepad=0/1
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  joy->ReadIni(name.trim(), value.trim());
}

static void KeyboardJoystickWriteAll(ImGuiContext* ctx,
                                     ImGuiSettingsHandler* handler,
                                     ImGuiTextBuffer* out_buf) {
  for (unsigned int i = 0; i < gKeyboardJoysticks.size(); ++i) {
    out_buf->appendf("[KeyboardJoystick][%u]\n", i);
    gKeyboardJoysticks[i]->WriteIni(out_buf);
    out_buf->append("\n");
  }
}

// read/write DS settings to ini file
static void* DriverStationReadOpen(ImGuiContext* ctx,
                                   ImGuiSettingsHandler* handler,
                                   const char* name) {
  if (name == wpi::StringRef{"Main"}) {
    return &gDisableDS;
  }
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
    if (value.getAsInteger(10, num)) {
      return;
    }
    gDisableDS = num;
  } else if (name == "zeroDisconnectedJoysticks") {
    int num;
    if (value.getAsInteger(10, num)) {
      return;
    }
    gZeroDisconnectedJoysticks = num;
  } else if (name == "enableDisableKeys") {
    int num;
    if (value.getAsInteger(10, num)) {
      return;
    }
    gUseEnableDisableHotkeys = num;
  } else if (name == "estopKey") {
    int num;
    if (value.getAsInteger(10, num)) {
      return;
    }
    gUseEstopHotkey = num;
  }
}

static void DriverStationWriteAll(ImGuiContext* ctx,
                                  ImGuiSettingsHandler* handler,
                                  ImGuiTextBuffer* out_buf) {
  out_buf->appendf(
      "[DriverStation][Main]\ndisable=%d\nzeroDisconnectedJoysticks=%d\n"
      "enableDisableKeys=%d\nestopKey=%d\n\n",
      gDisableDS ? 1 : 0, gZeroDisconnectedJoysticks ? 1 : 0,
      gUseEnableDisableHotkeys ? 1 : 0, gUseEstopHotkey ? 1 : 0);
}

void GlfwSystemJoystick::Update() {
  bool wasPresent = m_present;
  m_present = glfwJoystickPresent(m_index);

  if (!m_present) {
    return;
  }
  m_axes = glfwGetJoystickAxes(m_index, &m_axisCount);
  m_buttons = glfwGetJoystickButtons(m_index, &m_buttonCount);
  m_hats = glfwGetJoystickHats(m_index, &m_hatCount);
  m_isGamepad = glfwGetGamepadState(m_index, &m_gamepadState);

  m_anyButtonPressed = false;
  for (int j = 0; j < m_buttonCount; ++j) {
    if (m_buttons[j]) {
      m_anyButtonPressed = true;
      break;
    }
  }
  for (int j = 0; j < m_hatCount; ++j) {
    if (m_hats[j] != GLFW_HAT_CENTERED) {
      m_anyButtonPressed = true;
      break;
    }
  }

  if (!m_present || wasPresent) {
    return;
  }
  m_name = glfwGetJoystickName(m_index);

  // try to find matching GUID
  if (const char* guid = glfwGetJoystickGUID(m_index)) {
    for (auto&& joy : gRobotJoysticks) {
      if (guid == joy.guid) {
        joy.sys = this;
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

void GlfwSystemJoystick::GetData(HALJoystickData* data, bool mapGamepad) const {
  if (!m_present) {
    return;
  }

  // use gamepad mappings if present and enabled
  const float* sysAxes;
  const unsigned char* sysButtons;
  if (m_isGamepad && mapGamepad) {
    sysAxes = m_gamepadState.axes;
    // don't remap on windows
#ifdef _WIN32
    sysButtons = m_buttons;
#else
    sysButtons = m_gamepadState.buttons;
#endif
  } else {
    sysAxes = m_axes;
    sysButtons = m_buttons;
  }

  // copy into HAL structures
  data->desc.isXbox = m_isGamepad ? 1 : 0;
  data->desc.type = m_isGamepad ? 21 : 20;
  std::strncpy(data->desc.name, m_name, sizeof(data->desc.name) - 1);
  data->desc.name[sizeof(data->desc.name) - 1] = '\0';
  data->desc.axisCount = (std::min)(m_axisCount, HAL_kMaxJoystickAxes);
  // desc.axisTypes ???
  data->desc.buttonCount = (std::min)(m_buttonCount, 32);
  data->desc.povCount = (std::min)(m_hatCount, HAL_kMaxJoystickPOVs);

  data->buttons.count = data->desc.buttonCount;
  for (int j = 0; j < data->buttons.count; ++j) {
    data->buttons.buttons |= (sysButtons[j] ? 1u : 0u) << j;
  }

  data->axes.count = data->desc.axisCount;
  if (m_isGamepad && mapGamepad) {
    // the FRC DriverStation maps gamepad (XInput) trigger values to 0-1 range
    // on axis 2 and 3.
    data->axes.axes[0] = sysAxes[0];
    data->axes.axes[1] = sysAxes[1];
    data->axes.axes[2] = 0.5 + sysAxes[4] / 2.0;
    data->axes.axes[3] = 0.5 + sysAxes[5] / 2.0;
    data->axes.axes[4] = sysAxes[2];
    data->axes.axes[5] = sysAxes[3];

    // the start button for gamepads is not mapped on the FRC DriverStation
    // platforms, so remove it if present
    if (data->buttons.count == 11) {
      --data->desc.buttonCount;
      --data->buttons.count;
      data->buttons.buttons = (data->buttons.buttons & 0xff) |
                              ((data->buttons.buttons >> 1) & 0x300);
    }
  } else {
    std::memcpy(data->axes.axes, sysAxes,
                data->axes.count * sizeof(&data->axes.axes[0]));
  }

  data->povs.count = data->desc.povCount;
  for (int j = 0; j < data->povs.count; ++j) {
    data->povs.povs[j] = HatToAngle(m_hats[j]);
  }
}

KeyboardJoystick::KeyboardJoystick(int index) : m_index{index} {
  std::snprintf(m_name, sizeof(m_name), "Keyboard %d", index);
  std::snprintf(m_guid, sizeof(m_guid), "Keyboard%d", index);

  // init axes
  m_data.axes.count = 0;

  // init buttons
  m_data.buttons.count = 0;
  for (int i = 0; i < kMaxButtonCount; ++i) {
    m_buttonKey[i] = -1;
  }

  // init POVs
  m_data.povs.count = 0;

  // init desc structure
  m_data.desc.isXbox = 0;
  m_data.desc.type = 20;
  std::strncpy(m_data.desc.name, m_name, 256);
}

int* KeyboardJoystick::s_keyEdit = nullptr;

void KeyboardJoystick::EditKey(const char* label, int* key) {
  ImGui::PushID(label);
  ImGui::Text("%s", label);
  ImGui::SameLine();
  char editLabel[32];
  std::snprintf(editLabel, sizeof(editLabel), "%s###edit",
                s_keyEdit == key ? "(press key)" : GetKeyName(*key));
  if (ImGui::SmallButton(editLabel)) {
    s_keyEdit = key;
  }
  ImGui::SameLine();
  if (ImGui::SmallButton("Clear")) {
    *key = -1;
  }
  ImGui::PopID();
}

void KeyboardJoystick::SettingsDisplay() {
  if (s_keyEdit) {
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); ++i) {
      if (io.KeysDown[i]) {
        // remove all other uses
        for (auto&& joy : gKeyboardJoysticks) {
          joy->ClearKey(i);
        }
        *s_keyEdit = i;
        s_keyEdit = nullptr;
        break;
      }
    }
  }

  char label[64];
  ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
  // axes
  if (ImGui::CollapsingHeader("Axes", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::PushID("Axes");
    int axisCount = m_data.axes.count;
    if (ImGui::InputInt("Count", &axisCount)) {
      if (axisCount < 0) {
        axisCount = 0;
      }
      if (axisCount > HAL_kMaxJoystickAxes) {
        axisCount = HAL_kMaxJoystickAxes;
      }
      m_data.axes.count = axisCount;
    }
    for (int i = 0; i < axisCount; ++i) {
      std::snprintf(label, sizeof(label), "Axis %d", i);
      if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
        EditKey("Increase", &m_axisConfig[i].incKey);
        EditKey("Decrease", &m_axisConfig[i].decKey);
        ImGui::InputFloat("Key Rate", &m_axisConfig[i].keyRate);
        ImGui::InputFloat("Decay Rate", &m_axisConfig[i].decayRate);

        float maxAbsValue = m_axisConfig[i].maxAbsValue;
        if (ImGui::InputFloat("Max Absolute Value", &maxAbsValue)) {
          m_axisConfig[i].maxAbsValue = std::clamp(maxAbsValue, -1.0f, 1.0f);
        }

        ImGui::TreePop();
      }
    }
    ImGui::PopID();
  }

  // buttons
  if (ImGui::CollapsingHeader("Buttons", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::PushID("Buttons");
    int buttonCount = m_data.buttons.count;
    if (ImGui::InputInt("Count", &buttonCount)) {
      if (buttonCount < 0) {
        buttonCount = 0;
      }
      if (buttonCount > kMaxButtonCount) {
        buttonCount = kMaxButtonCount;
      }
      m_data.buttons.count = buttonCount;
    }
    for (int i = 0; i < buttonCount; ++i) {
      std::snprintf(label, sizeof(label), "Button %d", i + 1);
      EditKey(label, &m_buttonKey[i]);
    }
    ImGui::PopID();
  }

  // povs
  if (ImGui::CollapsingHeader("POVs", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::PushID("POVs");
    int povCount = m_data.povs.count;
    if (ImGui::InputInt("Count", &povCount)) {
      if (povCount < 0) {
        povCount = 0;
      }
      if (povCount > HAL_kMaxJoystickPOVs) {
        povCount = HAL_kMaxJoystickPOVs;
      }
      m_data.povs.count = povCount;
    }
    for (int i = 0; i < povCount; ++i) {
      std::snprintf(label, sizeof(label), "POV %d", i);
      if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
        EditKey("  0 deg", &m_povConfig[i].key0);
        EditKey(" 45 deg", &m_povConfig[i].key45);
        EditKey(" 90 deg", &m_povConfig[i].key90);
        EditKey("135 deg", &m_povConfig[i].key135);
        EditKey("180 deg", &m_povConfig[i].key180);
        EditKey("225 deg", &m_povConfig[i].key225);
        EditKey("270 deg", &m_povConfig[i].key270);
        EditKey("315 deg", &m_povConfig[i].key315);
        ImGui::TreePop();
      }
    }
    ImGui::PopID();
  }

  ImGui::PopItemWidth();
}

void KeyboardJoystick::Update() {
  ImGuiIO& io = ImGui::GetIO();

  if (m_data.axes.count > 0 || m_data.buttons.count > 0 ||
      m_data.povs.count > 0) {
    m_present = true;
  }

  // axes
  for (int i = 0; i < m_data.axes.count; ++i) {
    auto& config = m_axisConfig[i];
    float& axisValue = m_data.axes.axes[i];
    // increase/decrease while key held down (to saturation); decay back to 0
    if (config.incKey >= 0 && io.KeysDown[config.incKey]) {
      axisValue += config.keyRate;
      if (axisValue > config.maxAbsValue) {
        axisValue = config.maxAbsValue;
      }
    } else if (axisValue > 0) {
      if (axisValue < config.decayRate) {
        axisValue = 0;
      } else {
        axisValue -= config.decayRate;
      }
    }

    if (config.decKey >= 0 && io.KeysDown[config.decKey]) {
      axisValue -= config.keyRate;
      if (axisValue < -config.maxAbsValue) {
        axisValue = -config.maxAbsValue;
      }
    } else if (axisValue < 0) {
      if (axisValue > -config.decayRate) {
        axisValue = 0;
      } else {
        axisValue += config.decayRate;
      }
    }
  }

  // buttons
  m_data.buttons.buttons = 0;
  m_anyButtonPressed = false;
  for (int i = 0; i < m_data.buttons.count; ++i) {
    if (m_buttonKey[i] >= 0 && io.KeysDown[m_buttonKey[i]]) {
      m_data.buttons.buttons |= 1u << i;
      m_anyButtonPressed = true;
    }
  }

  // povs
  for (int i = 0; i < m_data.povs.count; ++i) {
    auto& config = m_povConfig[i];
    auto& povValue = m_data.povs.povs[i];
    povValue = -1;
    if (config.key0 >= 0 && io.KeysDown[config.key0]) {
      povValue = 0;
    } else if (config.key45 >= 0 && io.KeysDown[config.key45]) {
      povValue = 45;
    } else if (config.key90 >= 0 && io.KeysDown[config.key90]) {
      povValue = 90;
    } else if (config.key135 >= 0 && io.KeysDown[config.key135]) {
      povValue = 135;
    } else if (config.key180 >= 0 && io.KeysDown[config.key180]) {
      povValue = 180;
    } else if (config.key225 >= 0 && io.KeysDown[config.key225]) {
      povValue = 225;
    } else if (config.key270 >= 0 && io.KeysDown[config.key270]) {
      povValue = 270;
    } else if (config.key315 >= 0 && io.KeysDown[config.key315]) {
      povValue = 315;
    }
  }

  // try to find matching GUID
  for (auto&& joy : gRobotJoysticks) {
    if (m_guid == joy.guid) {
      joy.sys = this;
      joy.guid.clear();
      break;
    }
  }

  // update desc
  m_data.desc.axisCount = m_data.axes.count;
  m_data.desc.buttonCount = m_data.buttons.count;
  m_data.desc.povCount = m_data.povs.count;
}

void KeyboardJoystick::ClearKey(int key) {
  for (auto&& config : m_axisConfig) {
    if (config.incKey == key) {
      config.incKey = -1;
    }
    if (config.decKey == key) {
      config.decKey = -1;
    }
  }
  for (auto&& buttonKey : m_buttonKey) {
    if (buttonKey == key) {
      buttonKey = -1;
    }
  }
  for (auto&& config : m_povConfig) {
    if (config.key0 == key) {
      config.key0 = -1;
    }
    if (config.key45 == key) {
      config.key45 = -1;
    }
    if (config.key90 == key) {
      config.key90 = -1;
    }
    if (config.key135 == key) {
      config.key135 = -1;
    }
    if (config.key180 == key) {
      config.key180 = -1;
    }
    if (config.key225 == key) {
      config.key225 = -1;
    }
    if (config.key270 == key) {
      config.key270 = -1;
    }
    if (config.key315 == key) {
      config.key315 = -1;
    }
  }
}

void KeyboardJoystick::ReadIni(wpi::StringRef name, wpi::StringRef value) {
  if (name.startswith("axis")) {
    name = name.drop_front(4);
    if (name == "Count") {
      int v;
      if (value.getAsInteger(10, v)) {
        return;
      }
      m_data.axes.count = (std::min)(v, HAL_kMaxJoystickAxes);
      return;
    }

    unsigned int index;
    if (name.consumeInteger(10, index)) {
      return;
    }
    if (index >= HAL_kMaxJoystickAxes) {
      return;
    }
    if (name == "incKey") {
      int v;
      if (value.getAsInteger(10, v)) {
        return;
      }
      m_axisConfig[index].incKey = v;
    } else if (name == "decKey") {
      int v;
      if (value.getAsInteger(10, v)) {
        return;
      }
      m_axisConfig[index].decKey = v;
    } else if (name == "keyRate") {
      std::sscanf(value.data(), "%f", &m_axisConfig[index].keyRate);
    } else if (name == "decayRate") {
      std::sscanf(value.data(), "%f", &m_axisConfig[index].decayRate);
    } else if (name == "maxAbsValue") {
      std::sscanf(value.data(), "%f", &m_axisConfig[index].maxAbsValue);
    }
  } else if (name.startswith("button")) {
    name = name.drop_front(6);
    if (name == "Count") {
      int v;
      if (value.getAsInteger(10, v)) {
        return;
      }
      m_data.buttons.count = (std::min)(v, kMaxButtonCount);
      return;
    }

    unsigned int index;
    if (name.getAsInteger(10, index)) {
      return;
    }
    if (index >= kMaxButtonCount) {
      return;
    }
    int v;
    if (value.getAsInteger(10, v)) {
      return;
    }
    m_buttonKey[index] = v;
  } else if (name.startswith("pov")) {
    name = name.drop_front(3);
    if (name == "Count") {
      int v;
      if (value.getAsInteger(10, v)) {
        return;
      }
      m_data.povs.count = (std::min)(v, HAL_kMaxJoystickPOVs);
      return;
    }

    unsigned int index;
    if (name.consumeInteger(10, index)) {
      return;
    }
    if (index >= HAL_kMaxJoystickPOVs) {
      return;
    }
    int v;
    if (value.getAsInteger(10, v)) {
      return;
    }
    if (name == "key0") {
      m_povConfig[index].key0 = v;
    } else if (name == "key45") {
      m_povConfig[index].key45 = v;
    } else if (name == "key90") {
      m_povConfig[index].key90 = v;
    } else if (name == "key135") {
      m_povConfig[index].key135 = v;
    } else if (name == "key180") {
      m_povConfig[index].key180 = v;
    } else if (name == "key225") {
      m_povConfig[index].key225 = v;
    } else if (name == "key270") {
      m_povConfig[index].key270 = v;
    } else if (name == "key315") {
      m_povConfig[index].key315 = v;
    }
  }
}

void KeyboardJoystick::WriteIni(ImGuiTextBuffer* out_buf) const {
  out_buf->appendf("axisCount=%d\nbuttonCount=%d\npovCount=%d\n",
                   m_data.axes.count, m_data.buttons.count, m_data.povs.count);
  for (int i = 0; i < m_data.axes.count; ++i) {
    auto& c = m_axisConfig[i];
    out_buf->appendf(
        "axis%dincKey=%d\naxis%ddecKey=%d\naxis%dkeyRate=%f\n"
        "axis%ddecayRate=%f\naxis%dmaxAbsValue=%f\n",
        i, c.incKey, i, c.decKey, i, c.keyRate, i, c.decayRate, i,
        c.maxAbsValue);
  }
  for (int i = 0; i < m_data.buttons.count; ++i) {
    out_buf->appendf("button%d=%d\n", i, m_buttonKey[i]);
  }
  for (int i = 0; i < m_data.povs.count; ++i) {
    auto& c = m_povConfig[i];
    out_buf->appendf(
        "pov%dkey0=%d\npov%dkey45=%d\npov%dkey90=%d\npov%dkey135=%d\n"
        "pov%dkey180=%d\npov%dkey225=%d\npov%dkey270=%d\npov%dkey315=%d\n",
        i, c.key0, i, c.key45, i, c.key90, i, c.key135, i, c.key180, i,
        c.key225, i, c.key270, i, c.key315);
  }
}

GlfwKeyboardJoystick::GlfwKeyboardJoystick(int index, bool noDefaults)
    : KeyboardJoystick{index} {
  if (noDefaults) {
    return;
  }
  // set up a default keyboard config for 0, 1, and 2
  if (index == 0) {
    m_data.axes.count = 3;
    m_axisConfig[0].incKey = GLFW_KEY_D;
    m_axisConfig[0].decKey = GLFW_KEY_A;
    m_axisConfig[1].incKey = GLFW_KEY_S;
    m_axisConfig[1].decKey = GLFW_KEY_W;
    m_axisConfig[2].incKey = GLFW_KEY_R;
    m_axisConfig[2].decKey = GLFW_KEY_E;
    m_axisConfig[2].keyRate = 0.01f;
    m_axisConfig[2].decayRate = 0;  // works like a throttle
    m_data.buttons.count = 4;
    m_buttonKey[0] = GLFW_KEY_Z;
    m_buttonKey[1] = GLFW_KEY_X;
    m_buttonKey[2] = GLFW_KEY_C;
    m_buttonKey[3] = GLFW_KEY_V;
    m_data.povs.count = 1;
    m_povConfig[0].key0 = GLFW_KEY_KP_8;
    m_povConfig[0].key45 = GLFW_KEY_KP_9;
    m_povConfig[0].key90 = GLFW_KEY_KP_6;
    m_povConfig[0].key135 = GLFW_KEY_KP_3;
    m_povConfig[0].key180 = GLFW_KEY_KP_2;
    m_povConfig[0].key225 = GLFW_KEY_KP_1;
    m_povConfig[0].key270 = GLFW_KEY_KP_4;
    m_povConfig[0].key315 = GLFW_KEY_KP_7;
  } else if (index == 1) {
    m_data.axes.count = 2;
    m_axisConfig[0].incKey = GLFW_KEY_L;
    m_axisConfig[0].decKey = GLFW_KEY_J;
    m_axisConfig[1].incKey = GLFW_KEY_K;
    m_axisConfig[1].decKey = GLFW_KEY_I;
    m_data.buttons.count = 4;
    m_buttonKey[0] = GLFW_KEY_M;
    m_buttonKey[1] = GLFW_KEY_COMMA;
    m_buttonKey[2] = GLFW_KEY_PERIOD;
    m_buttonKey[3] = GLFW_KEY_SLASH;
  } else if (index == 2) {
    m_data.axes.count = 2;
    m_axisConfig[0].incKey = GLFW_KEY_RIGHT;
    m_axisConfig[0].decKey = GLFW_KEY_LEFT;
    m_axisConfig[1].incKey = GLFW_KEY_DOWN;
    m_axisConfig[1].decKey = GLFW_KEY_UP;
    m_data.buttons.count = 6;
    m_buttonKey[0] = GLFW_KEY_INSERT;
    m_buttonKey[1] = GLFW_KEY_HOME;
    m_buttonKey[2] = GLFW_KEY_PAGE_UP;
    m_buttonKey[3] = GLFW_KEY_DELETE;
    m_buttonKey[4] = GLFW_KEY_END;
    m_buttonKey[5] = GLFW_KEY_PAGE_DOWN;
  }
}

const char* GlfwKeyboardJoystick::GetKeyName(int key) const {
  if (key < 0) {
    return "(None)";
  }
  const char* name = glfwGetKeyName(key, 0);
  if (name) {
    return name;
  }
  // glfwGetKeyName sometimes doesn't have these keys
  switch (key) {
    case GLFW_KEY_RIGHT:
      return "Right";
    case GLFW_KEY_LEFT:
      return "Left";
    case GLFW_KEY_DOWN:
      return "Down";
    case GLFW_KEY_UP:
      return "Up";
    case GLFW_KEY_INSERT:
      return "Insert";
    case GLFW_KEY_HOME:
      return "Home";
    case GLFW_KEY_PAGE_UP:
      return "PgUp";
    case GLFW_KEY_DELETE:
      return "Delete";
    case GLFW_KEY_END:
      return "End";
    case GLFW_KEY_PAGE_DOWN:
      return "PgDn";
  }
  return "(Unknown)";
}

void RobotJoystick::Update() {
  Clear();
  if (sys) {
    sys->GetData(&data, useGamepad);
  }
}

void RobotJoystick::SetHAL(int i) {
  if (!gZeroDisconnectedJoysticks && (!sys || !sys->IsPresent())) {
    return;
  }
  // set at HAL level
  HALSIM_SetJoystickDescriptor(i, &data.desc);
  HALSIM_SetJoystickAxes(i, &data.axes);
  HALSIM_SetJoystickButtons(i, &data.buttons);
  HALSIM_SetJoystickPOVs(i, &data.povs);
}

void RobotJoystick::GetHAL(int i) {
  HALSIM_GetJoystickDescriptor(i, &data.desc);
  HALSIM_GetJoystickAxes(i, &data.axes);
  HALSIM_GetJoystickButtons(i, &data.buttons);
  HALSIM_GetJoystickPOVs(i, &data.povs);
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
        source = std::make_unique<JoystickModel>(i);
      }
    } else {
      source.reset();
    }
  }

  static bool prevDisableDS = false;

  bool disableDS = IsDSDisabled();
  if (disableDS && !prevDisableDS) {
    if (auto win = HALSimGui::manager.GetWindow("System Joysticks")) {
      win->SetVisibility(glass::Window::kDisabled);
    }
  } else if (!disableDS && prevDisableDS) {
    if (auto win = HALSimGui::manager.GetWindow("System Joysticks")) {
      win->SetVisibility(glass::Window::kShow);
    }
  }
  prevDisableDS = disableDS;
  if (disableDS) {
    return;
  }

  double curTime = glfwGetTime();

  // update system joysticks
  gNumGlfwJoysticks = 0;
  for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i) {
    gGlfwJoysticks[i]->Update();
    if (gGlfwJoysticks[i]->IsPresent()) {
      gNumGlfwJoysticks = i + 1;
    }
  }
  for (auto&& joy : gKeyboardJoysticks) {
    joy->Update();
  }

  // update robot joysticks
  for (auto&& joy : gRobotJoysticks) {
    joy.Update();
  }

  bool isEnabled = HALSIM_GetDriverStationEnabled();
  bool isAuto = HALSIM_GetDriverStationAutonomous();
  bool isTest = HALSIM_GetDriverStationTest();

  // Robot state
  {
    // DS hotkeys
    bool enableHotkey = false;
    bool disableHotkey = false;
    if (gUseEnableDisableHotkeys) {
      ImGuiIO& io = ImGui::GetIO();
      if (io.KeysDown[GLFW_KEY_ENTER] || io.KeysDown[GLFW_KEY_KP_ENTER]) {
        disableHotkey = true;
      } else if (io.KeysDown[GLFW_KEY_LEFT_BRACKET] &&
                 io.KeysDown[GLFW_KEY_RIGHT_BRACKET] &&
                 io.KeysDown[GLFW_KEY_BACKSLASH]) {
        enableHotkey = true;
      }
    }
    if (gUseEstopHotkey) {
      ImGuiIO& io = ImGui::GetIO();
      if (io.KeysDown[GLFW_KEY_SPACE]) {
        HALSIM_SetDriverStationEnabled(false);
      }
    }

    ImGui::SetNextWindowPos(ImVec2{5, 20}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Robot State", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Selectable("Disabled", !isEnabled) || disableHotkey) {
      HALSIM_SetDriverStationEnabled(false);
    }
    if (ImGui::Selectable("Autonomous", isEnabled && isAuto && !isTest)) {
      HALSIM_SetDriverStationAutonomous(true);
      HALSIM_SetDriverStationTest(false);
      HALSIM_SetDriverStationEnabled(true);
    }
    if (ImGui::Selectable("Teleoperated", isEnabled && !isAuto && !isTest) ||
        enableHotkey) {
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
  for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
    gRobotJoysticks[i].SetHAL(i);
  }

  // Send new data every 20 ms (may be slower depending on GUI refresh rate)
  static double lastNewDataTime = 0.0;
  if ((curTime - lastNewDataTime) > 0.02 && !HALSIM_IsTimingPaused()) {
    lastNewDataTime = curTime;
    HALSIM_NotifyDriverStationNewData();
  }
}

FMSSimModel::FMSSimModel() {
  m_fmsAttached.SetDigital(true);
  m_dsAttached.SetDigital(true);
  m_estop.SetDigital(true);
  m_enabled.SetDigital(true);
  m_test.SetDigital(true);
  m_autonomous.SetDigital(true);
}

void FMSSimModel::Update() {
  bool enabled = HALSIM_GetDriverStationEnabled();
  m_fmsAttached.SetValue(HALSIM_GetDriverStationFmsAttached());
  m_dsAttached.SetValue(HALSIM_GetDriverStationDsAttached());
  m_allianceStationId.SetValue(HALSIM_GetDriverStationAllianceStationId());
  m_estop.SetValue(HALSIM_GetDriverStationEStop());
  m_enabled.SetValue(enabled);
  m_test.SetValue(HALSIM_GetDriverStationTest());
  m_autonomous.SetValue(HALSIM_GetDriverStationAutonomous());

  double matchTime = HALSIM_GetDriverStationMatchTime();
  if (m_matchTimeEnabled && !IsDSDisabled()) {
    int32_t status = 0;
    double curTime = HAL_GetFPGATime(&status) * 1.0e-6;
    if (m_startMatchTime == 0.0) {
      m_startMatchTime = curTime;
    }
    if (enabled) {
      matchTime = curTime - m_startMatchTime;
      HALSIM_SetDriverStationMatchTime(matchTime);
    } else {
      if (m_prevTime == 0.0) {
        m_prevTime = curTime;
      }
      m_startMatchTime += (curTime - m_prevTime);
    }
    m_prevTime = curTime;
  } else {
    m_startMatchTime = 0.0;
    m_prevTime = 0.0;
  }
  m_matchTime.SetValue(matchTime);
}

bool FMSSimModel::IsReadOnly() {
  return IsDSDisabled();
}

static void DisplaySystemJoystick(SystemJoystick& joy, int i) {
  char label[64];
  std::snprintf(label, sizeof(label), "%d: %s", i, joy.GetName());

  // highlight if any buttons pressed
  bool anyButtonPressed = joy.IsAnyButtonPressed();
  if (anyButtonPressed) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
  }
  ImGui::Selectable(label, false,
                    joy.IsPresent() ? ImGuiSelectableFlags_None
                                    : ImGuiSelectableFlags_Disabled);
  if (anyButtonPressed) {
    ImGui::PopStyleColor();
  }

  // drag and drop sources are the low level joysticks
  if (ImGui::BeginDragDropSource()) {
    SystemJoystick* joyPtr = &joy;
    ImGui::SetDragDropPayload("Joystick", &joyPtr, sizeof(joyPtr));  // NOLINT
    ImGui::Text("%d: %s", i, joy.GetName());
    ImGui::EndDragDropSource();
  }
}

static void DisplaySystemJoysticks() {
  ImGui::Text("(Drag and drop to Joysticks)");
  int numShowJoysticks = gNumGlfwJoysticks < 6 ? 6 : gNumGlfwJoysticks;
  for (int i = 0; i < numShowJoysticks; ++i) {
    DisplaySystemJoystick(*gGlfwJoysticks[i], i);
  }
  for (size_t i = 0; i < gKeyboardJoysticks.size(); ++i) {
    DisplaySystemJoystick(*gKeyboardJoysticks[i], i + GLFW_JOYSTICK_LAST + 1);
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
        ImGui::SetDragDropPayload("Joystick", &joy.sys,
                                  sizeof(joy.sys));  // NOLINT
        ImGui::Text("%d: %s", joy.sys->GetIndex(), joy.sys->GetName());
        ImGui::EndDragDropSource();
      }
    } else {
      ImGui::Selectable(label, false, ImGuiSelectableFlags_Disabled);
    }
    if (!disableDS && ImGui::BeginDragDropTarget()) {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Joystick")) {
        IM_ASSERT(payload->DataSize == sizeof(SystemJoystick*));  // NOLINT
        SystemJoystick* payload_sys =
            *static_cast<SystemJoystick* const*>(payload->Data);
        // clear it from the other joysticks
        for (auto&& joy2 : gRobotJoysticks) {
          if (joy2.sys == payload_sys) {
            joy2.sys = nullptr;
          }
        }
        joy.sys = payload_sys;
        joy.guid.clear();
        wpi::StringRef name{payload_sys->GetName()};
        joy.useGamepad = name.startswith("Xbox") || name.contains("pad");
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

    if (disableDS) {
      joy.GetHAL(i);
    }

    if ((disableDS && joy.data.desc.type != 0) ||
        (joy.sys && joy.sys->IsPresent())) {
      // update GUI display
      ImGui::PushID(i);
      if (disableDS) {
        ImGui::Text("%s", joy.data.desc.name);
        ImGui::Text("Gamepad: %s", joy.data.desc.isXbox ? "Yes" : "No");
      } else {
        ImGui::Text("%d: %s", joy.sys->GetIndex(), joy.sys->GetName());

        if (joy.sys->IsGamepad()) {
          ImGui::Checkbox("Map gamepad", &joy.useGamepad);
        }
      }

      for (int j = 0; j < joy.data.axes.count; ++j) {
        if (source && source->axes[j]) {
          char label[64];
          std::snprintf(label, sizeof(label), "Axis[%d]", j);
          ImGui::Selectable(label);
          source->axes[j]->EmitDrag();
          ImGui::SameLine();
          ImGui::Text(": %.3f", joy.data.axes.axes[j]);
        } else {
          ImGui::Text("Axis[%d]: %.3f", j, joy.data.axes.axes[j]);
        }
      }

      for (int j = 0; j < joy.data.povs.count; ++j) {
        if (source && source->povs[j]) {
          char label[64];
          std::snprintf(label, sizeof(label), "POVs[%d]", j);
          ImGui::Selectable(label);
          source->povs[j]->EmitDrag();
          ImGui::SameLine();
          ImGui::Text(": %d", joy.data.povs.povs[j]);
        } else {
          ImGui::Text("POVs[%d]: %d", j, joy.data.povs.povs[j]);
        }
      }

      // show buttons as multiple lines of LED indicators, 8 per line
      static const ImU32 color = IM_COL32(255, 255, 102, 255);
      wpi::SmallVector<int, 64> buttons;
      buttons.resize(joy.data.buttons.count);
      for (int j = 0; j < joy.data.buttons.count; ++j) {
        buttons[j] = joy.IsButtonPressed(j) ? 1 : -1;
      }
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

void DSManager::DisplayMenu() {
  if (gDSSocketConnected && *gDSSocketConnected) {
    ImGui::MenuItem("Turn off DS (real DS connected)", nullptr, true, false);
  } else {
    ImGui::MenuItem("Turn off DS", nullptr, &gDisableDS);
    ImGui::MenuItem("Zero disconnected joysticks", nullptr,
                    &gZeroDisconnectedJoysticks);
    ImGui::MenuItem("Enable on []\\ combo, Disable on Enter", nullptr,
                    &gUseEnableDisableHotkeys);
    ImGui::MenuItem("Disable on Spacebar", nullptr, &gUseEstopHotkey);
  }
  ImGui::Separator();

  for (auto&& window : m_windows) {
    window->DisplayMenuItem();
  }
}

static void DriverStationInitialize() {
  // hook ini handler to save joystick settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "Joystick";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = JoystickReadOpen;
  iniHandler.ReadLineFn = JoystickReadLine;
  iniHandler.WriteAllFn = JoystickWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  // hook ini handler to save keyboard settings
  iniHandler.TypeName = "KeyboardJoystick";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = KeyboardJoystickReadOpen;
  iniHandler.ReadLineFn = KeyboardJoystickReadLine;
  iniHandler.WriteAllFn = KeyboardJoystickWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  // hook ini handler to save DS settings
  iniHandler.TypeName = "DriverStation";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = DriverStationReadOpen;
  iniHandler.ReadLineFn = DriverStationReadLine;
  iniHandler.WriteAllFn = DriverStationWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);
}

void DriverStationGui::GlobalInit() {
  // set up system joysticks (both GLFW and keyboard)
  for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i) {
    gGlfwJoysticks.emplace_back(std::make_unique<GlfwSystemJoystick>(i));
  }
  for (int i = 0; i < 4; ++i) {
    gKeyboardJoysticks.emplace_back(std::make_unique<GlfwKeyboardJoystick>(i));
  }

  dsManager.GlobalInit();

  wpi::gui::AddInit(DriverStationInitialize);

  gFMSModel = std::make_unique<FMSSimModel>();

  wpi::gui::AddEarlyExecute(DriverStationExecute);
  wpi::gui::AddEarlyExecute([] { gFMSModel->Update(); });
  if (auto win = dsManager.AddWindow("FMS", [] {
        DisplayFMS(gFMSModel.get(), &gFMSModel->m_matchTimeEnabled);
      })) {
    win->DisableRenamePopup();
    win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
    win->SetDefaultPos(5, 540);
  }
  if (auto win =
          dsManager.AddWindow("System Joysticks", DisplaySystemJoysticks)) {
    win->DisableRenamePopup();
    win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
    win->SetDefaultPos(5, 350);
  }
  if (auto win = dsManager.AddWindow("Joysticks", DisplayJoysticks)) {
    win->DisableRenamePopup();
    win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
    win->SetDefaultPos(250, 465);
  }
  int i = 0;
  for (auto&& joy : gKeyboardJoysticks) {
    char label[64];
    std::snprintf(label, sizeof(label), "%s Settings", joy->GetName());
    if (auto win = dsManager.AddWindow(
            label, [j = joy.get()] { j->SettingsDisplay(); })) {
      win->SetVisible(false);
      win->DisableRenamePopup();
      win->SetDefaultPos(10 + 310 * i++, 50);
      if (i > 3) {
        i = 0;
      }
      win->SetDefaultSize(300, 560);
    }
  }
}

void DriverStationGui::SetDSSocketExtension(void* data) {
  gDSSocketConnected = static_cast<std::atomic<bool>*>(data);
}
