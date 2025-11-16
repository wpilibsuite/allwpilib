// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DriverStationGui.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/glass/other/FMS.hpp"
#include "wpi/glass/support/ExtraGuiWidgets.hpp"
#include "wpi/glass/support/NameSetting.hpp"
#include "wpi/gui/wpigui.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/halsim/gui/HALDataSource.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace halsimgui;

namespace {

struct HALJoystickData {
  HALJoystickData() {
    std::memset(&desc, 0, sizeof(desc));
    desc.gamepadType = 0;
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
  KeyboardJoystick(wpi::glass::Storage& storage, int index);

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

 protected:
  void EditKey(const char* label, int* key);

  int m_index;
  char m_name[20];
  char m_guid[20];

  static int* s_keyEdit;

  HALJoystickData m_data;

  int& m_axisCount;
  int& m_buttonCount;
  int& m_povCount;

  struct AxisConfig {
    explicit AxisConfig(wpi::glass::Storage& storage);

    int& incKey;
    int& decKey;
    float& keyRate;
    float& decayRate;
    float& maxAbsValue;
  };

  std::vector<std::unique_ptr<wpi::glass::Storage>>& m_axisStorage;
  std::vector<AxisConfig> m_axisConfig;

  static constexpr int kMaxButtonCount = 32;
  std::vector<int>& m_buttonKey;

  struct PovConfig {
    explicit PovConfig(wpi::glass::Storage& storage);

    int& keyUp;
    int& keyUpRight;
    int& keyRight;
    int& keyDownRight;
    int& keyDown;
    int& keyDownLeft;
    int& keyLeft;
    int& keyUpLeft;
  };

  std::vector<std::unique_ptr<wpi::glass::Storage>>& m_povStorage;
  std::vector<PovConfig> m_povConfig;
};

class GlfwKeyboardJoystick : public KeyboardJoystick {
 public:
  GlfwKeyboardJoystick(wpi::glass::Storage& storage, int index);

  const char* GetKeyName(int key) const override;
};

struct RobotJoystick {
  explicit RobotJoystick(wpi::glass::Storage& storage);

  wpi::glass::NameSetting name;
  std::string& guid;
  const SystemJoystick* sys = nullptr;
  bool& useGamepad;  // = false;

  HALJoystickData data;

  void Clear() { data = HALJoystickData{}; }
  void Update();
  void SetHAL(int i);
  void GetHAL(int i);
  bool IsButtonPressed(int i) {
    return (data.buttons.buttons & (1llu << i)) != 0;
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
  std::unique_ptr<wpi::glass::DoubleSource> axes[HAL_kMaxJoystickAxes];
  // use pointer instead of unique_ptr to allow it to be passed directly
  // to DrawLEDSources()
  wpi::glass::BooleanSource* buttons[32];
  std::unique_ptr<wpi::glass::IntegerSource> povs[HAL_kMaxJoystickPOVs];

 private:
  static void CallbackFunc(const char*, void* param, const HAL_Value*);

  int m_index;
  int32_t m_callback;
};

class FMSSimModel : public wpi::glass::FMSModel {
 public:
  FMSSimModel();

  wpi::glass::BooleanSource* GetFmsAttachedData() override {
    return &m_fmsAttached;
  }
  wpi::glass::BooleanSource* GetDsAttachedData() override {
    return &m_dsAttached;
  }
  wpi::glass::IntegerSource* GetAllianceStationIdData() override {
    return &m_allianceStationId;
  }
  wpi::glass::DoubleSource* GetMatchTimeData() override { return &m_matchTime; }
  wpi::glass::BooleanSource* GetEStopData() override { return &m_estop; }
  wpi::glass::BooleanSource* GetEnabledData() override { return &m_enabled; }
  wpi::glass::BooleanSource* GetTestData() override { return &m_test; }
  wpi::glass::BooleanSource* GetAutonomousData() override {
    return &m_autonomous;
  }
  wpi::glass::StringSource* GetGameSpecificMessageData() override {
    return &m_gameMessage;
  }

  void SetFmsAttached(bool val) override { m_fmsAttached.SetValue(val); }
  void SetDsAttached(bool val) override { m_dsAttached.SetValue(val); }
  void SetAllianceStationId(int val) override {
    m_allianceStationId.SetValue(val);
  }
  void SetMatchTime(double val) override { m_matchTime.SetValue(val); }
  void SetEStop(bool val) override { m_estop.SetValue(val); }
  void SetEnabled(bool val) override { m_enabled.SetValue(val); }
  void SetTest(bool val) override { m_test.SetValue(val); }
  void SetAutonomous(bool val) override { m_autonomous.SetValue(val); }
  void SetGameSpecificMessage(std::string_view val) override {
    m_gameMessage.SetValue(val);
  }

  void UpdateHAL();

  void Update() override;

  bool Exists() override { return true; }

  bool IsReadOnly() override;

 private:
  wpi::glass::BooleanSource m_fmsAttached{"FMS:FMSAttached"};
  wpi::glass::BooleanSource m_dsAttached{"FMS:DSAttached"};
  wpi::glass::IntegerSource m_allianceStationId{"FMS:AllianceStationID"};
  wpi::glass::DoubleSource m_matchTime{"FMS:MatchTime"};
  wpi::glass::BooleanSource m_estop{"FMS:EStop"};
  wpi::glass::BooleanSource m_enabled{"FMS:RobotEnabled"};
  wpi::glass::BooleanSource m_test{"FMS:TestMode"};
  wpi::glass::BooleanSource m_autonomous{"FMS:AutonomousMode"};
  double m_startMatchTime = -1.0;
  wpi::glass::StringSource m_gameMessage{"FMS:GameSpecificMessage"};
};

}  // namespace

// system joysticks
static std::vector<std::unique_ptr<SystemJoystick>> gGlfwJoysticks;
static int gNumGlfwJoysticks = 0;
static std::vector<std::unique_ptr<GlfwKeyboardJoystick>> gKeyboardJoysticks;

// robot joysticks
static std::vector<RobotJoystick> gRobotJoysticks;
static std::unique_ptr<JoystickModel> gJoystickSources[HAL_kMaxJoysticks];

// FMS
static std::unique_ptr<FMSSimModel> gFMSModel;

// Window management
std::unique_ptr<DSManager> DriverStationGui::dsManager;

static bool* gpDisableDS = nullptr;
static bool* gpZeroDisconnectedJoysticks = nullptr;
static bool* gpUseEnableDisableHotkeys = nullptr;
static bool* gpUseEstopHotkey = nullptr;
static std::atomic<bool>* gpDSSocketConnected = nullptr;

static inline bool IsDSDisabled() {
  return (gpDisableDS != nullptr && *gpDisableDS) ||
         (gpDSSocketConnected && *gpDSSocketConnected);
}

JoystickModel::JoystickModel(int index) : m_index{index} {
  HAL_JoystickAxes halAxes;
  HALSIM_GetJoystickAxes(index, &halAxes);
  axisCount = static_cast<uint8_t>(16 - std::countl_zero(halAxes.available));
  for (int i = 0; i < axisCount; ++i) {
    axes[i] = std::make_unique<wpi::glass::DoubleSource>(
        fmt::format("Joystick[{}] Axis[{}]", index, i));
  }

  HAL_JoystickButtons halButtons;
  HALSIM_GetJoystickButtons(index, &halButtons);
  buttonCount =
      static_cast<uint8_t>(64 - std::countl_zero(halButtons.available));
  for (int i = 0; i < buttonCount; ++i) {
    buttons[i] = new wpi::glass::BooleanSource(
        fmt::format("Joystick[{}] Button[{}]", index, i + 1));
  }
  for (int i = buttonCount; i < 64; ++i) {
    buttons[i] = nullptr;
  }

  HAL_JoystickPOVs halPOVs;
  HALSIM_GetJoystickPOVs(index, &halPOVs);
  povCount = static_cast<uint8_t>(8 - std::countl_zero(halPOVs.available));
  for (int i = 0; i < povCount; ++i) {
    povs[i] = std::make_unique<wpi::glass::IntegerSource>(
        fmt::format("Joystick[{}] POV [{}]", index, i));
  }

  m_callback =
      HALSIM_RegisterDriverStationNewDataCallback(CallbackFunc, this, true);
}

void JoystickModel::CallbackFunc(const char*, void* param, const HAL_Value*) {
  auto self = static_cast<JoystickModel*>(param);

  HAL_JoystickAxes halAxes;
  HALSIM_GetJoystickAxes(self->m_index, &halAxes);
  int halAxesCount = 16 - std::countl_zero(halAxes.available);
  for (int i = 0; i < halAxesCount; ++i) {
    if (auto axis = self->axes[i].get()) {
      axis->SetValue(halAxes.axes[i]);
    }
  }

  HAL_JoystickButtons halButtons;
  HALSIM_GetJoystickButtons(self->m_index, &halButtons);
  int halButtonCount = 64 - std::countl_zero(halButtons.available);
  for (int i = 0; i < halButtonCount; ++i) {
    if (auto button = self->buttons[i]) {
      button->SetValue((halButtons.buttons & (1llu << i)) != 0 ? 1 : 0);
    }
  }

  HAL_JoystickPOVs halPOVs;
  HALSIM_GetJoystickPOVs(self->m_index, &halPOVs);
  int halPovCount = 8 - std::countl_zero(halPOVs.available);
  for (int i = 0; i < halPovCount; ++i) {
    if (auto pov = self->povs[i].get()) {
      pov->SetValue(halPOVs.povs[i]);
    }
  }
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
        break;
      }
    }
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
  data->desc.isGamepad = m_isGamepad ? 1 : 0;
  data->desc.gamepadType = m_isGamepad ? 21 : 20;
  std::strncpy(data->desc.name, m_name, sizeof(data->desc.name) - 1);
  data->desc.name[sizeof(data->desc.name) - 1] = '\0';
  int axesCount = (std::min)(m_axisCount, HAL_kMaxJoystickAxes);
  int buttonCount = (std::min)(m_buttonCount, 64);
  int povsCount = (std::min)(m_hatCount, HAL_kMaxJoystickPOVs);

  if (buttonCount < 64) {
    data->buttons.available = (1ULL << buttonCount) - 1;
  } else {
    data->buttons.available = (std::numeric_limits<uint64_t>::max)();
  }
  data->axes.available = (1 << axesCount) - 1;
  data->povs.available = (1 << povsCount) - 1;

  for (int j = 0; j < buttonCount; ++j) {
    data->buttons.buttons |= (sysButtons[j] ? 1u : 0u) << j;
  }

  if (m_isGamepad && mapGamepad) {
    // the FRC DriverStation maps gamepad (XInput) trigger values to 0-1 range
    // on axis 2 and 3.
    data->axes.axes[0] = sysAxes[0];
    data->axes.axes[1] = sysAxes[1];
    data->axes.axes[2] = 0.5 + sysAxes[4] / 2.0;
    data->axes.axes[3] = 0.5 + sysAxes[5] / 2.0;
    data->axes.axes[4] = sysAxes[2];
    data->axes.axes[5] = sysAxes[3];

  } else {
    std::memcpy(data->axes.axes, sysAxes,
                axesCount * sizeof(data->axes.axes[0]));
  }

  for (int j = 0; j < povsCount; ++j) {
#if __GNUC__ >= 12
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow="
#endif  // __GNUC__ >= 12
    // From https://www.glfw.org/docs/latest/group__hat__state.html, GLFW hat
    // states use the same format
    data->povs.povs[j] = static_cast<HAL_JoystickPOV>(m_hats[j]);
#if __GNUC__ >= 12
#pragma GCC diagnostic pop
#endif  // __GNUC__ >= 12
  }
}

KeyboardJoystick::AxisConfig::AxisConfig(wpi::glass::Storage& storage)
    : incKey{storage.GetInt("incKey", -1)},
      decKey{storage.GetInt("decKey", -1)},
      keyRate{storage.GetFloat("keyRate", 0.05f)},
      decayRate{storage.GetFloat("decayRate", 0.05f)},
      maxAbsValue{storage.GetFloat("maxAbsValue", 1.0f)} {
  // sanity check the key ranges
  if (incKey < -1 || incKey >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    incKey = -1;
  }
  if (decKey < -1 || decKey >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    decKey = -1;
  }
}

KeyboardJoystick::PovConfig::PovConfig(wpi::glass::Storage& storage)
    : keyUp{storage.GetInt("keyUp", -1)},
      keyUpRight{storage.GetInt("keyUpRight", -1)},
      keyRight{storage.GetInt("keyRight", -1)},
      keyDownRight{storage.GetInt("keyDownRight", -1)},
      keyDown{storage.GetInt("keyDown", -1)},
      keyDownLeft{storage.GetInt("keyDownLeft", -1)},
      keyLeft{storage.GetInt("keyLeft", -1)},
      keyUpLeft{storage.GetInt("keyUpLeft", -1)} {
  // sanity check the key ranges
  if (keyUp < -1 || keyUp >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyUp = -1;
  }
  if (keyUpRight < -1 || keyUpRight >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyUpRight = -1;
  }
  if (keyRight < -1 || keyRight >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyRight = -1;
  }
  if (keyDownRight < -1 || keyDownRight >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyDownRight = -1;
  }
  if (keyDown < -1 || keyDown >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyDown = -1;
  }
  if (keyDownLeft < -1 || keyDownLeft >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyDownLeft = -1;
  }
  if (keyLeft < -1 || keyLeft >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyLeft = -1;
  }
  if (keyUpLeft < -1 || keyUpLeft >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
    keyUpLeft = -1;
  }
}

KeyboardJoystick::KeyboardJoystick(wpi::glass::Storage& storage, int index)
    : m_index{index},
      m_axisCount{storage.GetInt("axisCount", -1)},
      m_buttonCount{storage.GetInt("buttonCount", -1)},
      m_povCount{storage.GetInt("povCount", -1)},
      m_axisStorage{storage.GetChildArray("axisConfig")},
      m_buttonKey{storage.GetIntArray("buttonKeys")},
      m_povStorage{storage.GetChildArray("povConfig")} {
  wpi::util::format_to_n_c_str(m_name, sizeof(m_name), "Keyboard {}", index);
  wpi::util::format_to_n_c_str(m_guid, sizeof(m_guid), "Keyboard{}", index);

  // init axes
  for (auto&& axisConfig : m_axisStorage) {
    m_axisConfig.emplace_back(*axisConfig);
  }

  // sanity check the button key ranges
  for (auto&& key : m_buttonKey) {
    if (key < -1 || key >= IM_ARRAYSIZE(ImGuiIO::KeysDown)) {
      key = -1;
    }
  }

  // init POVs
  for (auto&& povConfig : m_povStorage) {
    m_povConfig.emplace_back(*povConfig);
  }

  // init desc structure
  m_data.desc.isGamepad = 0;
  m_data.desc.gamepadType = 20;
  std::strncpy(m_data.desc.name, m_name, 256);
}

int* KeyboardJoystick::s_keyEdit = nullptr;

void KeyboardJoystick::EditKey(const char* label, int* key) {
  ImGui::PushID(label);
  ImGui::Text("%s", label);
  ImGui::SameLine();

  char editLabel[32];
  if (s_keyEdit == key) {
    wpi::util::format_to_n_c_str(editLabel, sizeof(editLabel),
                                 "(press key)###edit");
  } else {
    wpi::util::format_to_n_c_str(editLabel, sizeof(editLabel), "{}###edit",
                                 GetKeyName(*key));
  }

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
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
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
    if (ImGui::InputInt("Count", &m_axisCount)) {
      if (m_axisCount < 0) {
        m_axisCount = 0;
      } else if (m_axisCount > HAL_kMaxJoystickAxes) {
        m_axisCount = HAL_kMaxJoystickAxes;
      }
    }
    while (m_axisCount > static_cast<int>(m_axisConfig.size())) {
      m_axisStorage.emplace_back(std::make_unique<wpi::glass::Storage>());
      m_axisConfig.emplace_back(*m_axisStorage.back());
    }
    for (int i = 0; i < m_axisCount; ++i) {
      wpi::util::format_to_n_c_str(label, sizeof(label), "Axis {}", i);

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
    if (ImGui::InputInt("Count", &m_buttonCount)) {
      if (m_buttonCount < 0) {
        m_buttonCount = 0;
      }
      if (m_buttonCount > kMaxButtonCount) {
        m_buttonCount = kMaxButtonCount;
      }
    }
    while (m_buttonCount > static_cast<int>(m_buttonKey.size())) {
      m_buttonKey.emplace_back(-1);
    }
    for (int i = 0; i < m_buttonCount; ++i) {
      wpi::util::format_to_n_c_str(label, sizeof(label), "Button {}", i + 1);

      EditKey(label, &m_buttonKey[i]);
    }
    ImGui::PopID();
  }

  // povs
  if (ImGui::CollapsingHeader("POVs", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::PushID("POVs");
    if (ImGui::InputInt("Count", &m_povCount)) {
      if (m_povCount < 0) {
        m_povCount = 0;
      }
      if (m_povCount > HAL_kMaxJoystickPOVs) {
        m_povCount = HAL_kMaxJoystickPOVs;
      }
    }
    while (m_povCount > static_cast<int>(m_povConfig.size())) {
      m_povStorage.emplace_back(std::make_unique<wpi::glass::Storage>());
      m_povConfig.emplace_back(*m_povStorage.back());
    }
    for (int i = 0; i < m_povCount; ++i) {
      wpi::util::format_to_n_c_str(label, sizeof(label), "POV {}", i);

      if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
        EditKey("        Up", &m_povConfig[i].keyUp);
        EditKey("  Up Right", &m_povConfig[i].keyUpRight);
        EditKey("     Right", &m_povConfig[i].keyRight);
        EditKey("Down Right", &m_povConfig[i].keyDownRight);
        EditKey("      Down", &m_povConfig[i].keyDown);
        EditKey(" Down Left", &m_povConfig[i].keyDownLeft);
        EditKey("      Left", &m_povConfig[i].keyLeft);
        EditKey("   Up Left", &m_povConfig[i].keyUpLeft);
        ImGui::TreePop();
      }
    }
    ImGui::PopID();
  }

  ImGui::PopItemWidth();
}

static inline bool IsKeyDown(ImGuiIO& io, int key) {
  return key >= 0 && key < IM_ARRAYSIZE(ImGuiIO::KeysDown) && io.KeysDown[key];
}

void KeyboardJoystick::Update() {
  ImGuiIO& io = ImGui::GetIO();

  if (m_axisCount < 0) {
    m_axisCount = 0;
  }
  if (m_buttonCount < 0) {
    m_buttonCount = 0;
  }
  if (m_povCount < 0) {
    m_povCount = 0;
  }

  int axesCount =
      (std::min)(m_axisCount, static_cast<int>(m_axisConfig.size()));
  int buttonsCount =
      (std::min)(m_buttonCount, static_cast<int>(m_buttonKey.size()));
  int povsCount = (std::min)(m_povCount, static_cast<int>(m_povConfig.size()));

  m_data.axes.available = (1 << axesCount) - 1;
  m_data.povs.available = (1 << povsCount) - 1;
  if (buttonsCount >= 64) {
    m_data.buttons.available = (std::numeric_limits<uint64_t>::max)();
  } else {
    m_data.buttons.available = (1u << buttonsCount) - 1;
  }

  if (m_data.axes.available > 0 || m_data.buttons.available > 0 ||
      m_data.povs.available > 0) {
    m_present = true;
  }

  // axes
  for (int i = 0; i < axesCount; ++i) {
    auto& config = m_axisConfig[i];
    float& axisValue = m_data.axes.axes[i];
    // increase/decrease while key held down (to saturation); decay back to 0
    if (IsKeyDown(io, config.incKey)) {
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

    if (IsKeyDown(io, config.decKey)) {
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
  for (int i = 0; i < buttonsCount; ++i) {
    if (IsKeyDown(io, m_buttonKey[i])) {
      m_data.buttons.buttons |= 1llu << i;
      m_anyButtonPressed = true;
    }
  }

  // povs
  for (int i = 0; i < povsCount; ++i) {
    auto& config = m_povConfig[i];
    auto& povValue = m_data.povs.povs[i];
    povValue = HAL_JoystickPOV_kCentered;
    if (IsKeyDown(io, config.keyUp)) {
      povValue = HAL_JoystickPOV_kUp;
    } else if (IsKeyDown(io, config.keyUpRight)) {
      povValue = HAL_JoystickPOV_kRightUp;
    } else if (IsKeyDown(io, config.keyRight)) {
      povValue = HAL_JoystickPOV_kRight;
    } else if (IsKeyDown(io, config.keyDownRight)) {
      povValue = HAL_JoystickPOV_kRightDown;
    } else if (IsKeyDown(io, config.keyDown)) {
      povValue = HAL_JoystickPOV_kDown;
    } else if (IsKeyDown(io, config.keyDownLeft)) {
      povValue = HAL_JoystickPOV_kLeftDown;
    } else if (IsKeyDown(io, config.keyLeft)) {
      povValue = HAL_JoystickPOV_kLeft;
    } else if (IsKeyDown(io, config.keyUpLeft)) {
      povValue = HAL_JoystickPOV_kLeftUp;
    }
  }

  // try to find matching GUID
  for (auto&& joy : gRobotJoysticks) {
    if (m_guid == joy.guid) {
      joy.sys = this;
      break;
    }
  }
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
    if (config.keyUp == key) {
      config.keyUp = -1;
    }
    if (config.keyUpRight == key) {
      config.keyUpRight = -1;
    }
    if (config.keyRight == key) {
      config.keyRight = -1;
    }
    if (config.keyDownRight == key) {
      config.keyDownRight = -1;
    }
    if (config.keyDown == key) {
      config.keyDown = -1;
    }
    if (config.keyDownLeft == key) {
      config.keyDownLeft = -1;
    }
    if (config.keyLeft == key) {
      config.keyLeft = -1;
    }
    if (config.keyUpLeft == key) {
      config.keyUpLeft = -1;
    }
  }
}

GlfwKeyboardJoystick::GlfwKeyboardJoystick(wpi::glass::Storage& storage,
                                           int index)
    : KeyboardJoystick{storage, index} {
  // set up a default keyboard config for 0, 1, and 2
  if (index == 0) {
    if (m_axisCount == -1 && m_axisStorage.empty()) {
      m_axisCount = 3;
      for (int i = 0; i < 3; ++i) {
        m_axisStorage.emplace_back(std::make_unique<wpi::glass::Storage>());
        m_axisConfig.emplace_back(*m_axisStorage.back());
      }
      m_axisConfig[0].incKey = GLFW_KEY_D;
      m_axisConfig[0].decKey = GLFW_KEY_A;
      m_axisConfig[1].incKey = GLFW_KEY_S;
      m_axisConfig[1].decKey = GLFW_KEY_W;
      m_axisConfig[2].incKey = GLFW_KEY_R;
      m_axisConfig[2].decKey = GLFW_KEY_E;
      m_axisConfig[2].keyRate = 0.01f;
      m_axisConfig[2].decayRate = 0;  // works like a throttle
    }
    if (m_buttonCount == -1 && m_buttonKey.empty()) {
      m_buttonCount = 4;
      m_buttonKey.resize(4);
      m_buttonKey[0] = GLFW_KEY_Z;
      m_buttonKey[1] = GLFW_KEY_X;
      m_buttonKey[2] = GLFW_KEY_C;
      m_buttonKey[3] = GLFW_KEY_V;
    }
    if (m_povCount == -1 && m_povStorage.empty()) {
      m_povCount = 1;
      m_povStorage.emplace_back(std::make_unique<wpi::glass::Storage>());
      m_povConfig.emplace_back(*m_povStorage.back());
      m_povConfig[0].keyUp = GLFW_KEY_KP_8;
      m_povConfig[0].keyUpRight = GLFW_KEY_KP_9;
      m_povConfig[0].keyRight = GLFW_KEY_KP_6;
      m_povConfig[0].keyDownRight = GLFW_KEY_KP_3;
      m_povConfig[0].keyDown = GLFW_KEY_KP_2;
      m_povConfig[0].keyDownLeft = GLFW_KEY_KP_1;
      m_povConfig[0].keyLeft = GLFW_KEY_KP_4;
      m_povConfig[0].keyUpLeft = GLFW_KEY_KP_7;
    }
  } else if (index == 1) {
    if (m_axisCount == -1 && m_axisStorage.empty()) {
      m_axisCount = 2;
      for (int i = 0; i < 2; ++i) {
        m_axisStorage.emplace_back(std::make_unique<wpi::glass::Storage>());
        m_axisConfig.emplace_back(*m_axisStorage.back());
      }
      m_axisConfig[0].incKey = GLFW_KEY_L;
      m_axisConfig[0].decKey = GLFW_KEY_J;
      m_axisConfig[1].incKey = GLFW_KEY_K;
      m_axisConfig[1].decKey = GLFW_KEY_I;
    }
    if (m_buttonCount == -1 && m_buttonKey.empty()) {
      m_buttonCount = 4;
      m_buttonKey.resize(4);
      m_buttonKey[0] = GLFW_KEY_M;
      m_buttonKey[1] = GLFW_KEY_COMMA;
      m_buttonKey[2] = GLFW_KEY_PERIOD;
      m_buttonKey[3] = GLFW_KEY_SLASH;
    }
  } else if (index == 2) {
    if (m_axisCount == -1 && m_axisStorage.empty()) {
      m_axisCount = 2;
      for (int i = 0; i < 2; ++i) {
        m_axisStorage.emplace_back(std::make_unique<wpi::glass::Storage>());
        m_axisConfig.emplace_back(*m_axisStorage.back());
      }
      m_axisConfig[0].incKey = GLFW_KEY_RIGHT;
      m_axisConfig[0].decKey = GLFW_KEY_LEFT;
      m_axisConfig[1].incKey = GLFW_KEY_DOWN;
      m_axisConfig[1].decKey = GLFW_KEY_UP;
    }
    if (m_buttonCount == -1 && m_buttonKey.empty()) {
      m_buttonCount = 6;
      m_buttonKey.resize(6);
      m_buttonKey[0] = GLFW_KEY_INSERT;
      m_buttonKey[1] = GLFW_KEY_HOME;
      m_buttonKey[2] = GLFW_KEY_PAGE_UP;
      m_buttonKey[3] = GLFW_KEY_DELETE;
      m_buttonKey[4] = GLFW_KEY_END;
      m_buttonKey[5] = GLFW_KEY_PAGE_DOWN;
    }
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

RobotJoystick::RobotJoystick(wpi::glass::Storage& storage)
    : name{storage.GetString("name")},
      guid{storage.GetString("guid")},
      useGamepad{storage.GetBool("useGamepad")} {}

void RobotJoystick::Update() {
  Clear();
  if (sys) {
    sys->GetData(&data, useGamepad);
  }
}

void RobotJoystick::SetHAL(int i) {
  if ((gpZeroDisconnectedJoysticks != nullptr &&
       !gpZeroDisconnectedJoysticks) &&
      (!sys || !sys->IsPresent())) {
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

static void DriverStationConnect(bool enabled, bool autonomous, bool test) {
  if (!HALSIM_GetDriverStationDsAttached()) {
    // initialize FMS bits too
    gFMSModel->SetDsAttached(true);
    gFMSModel->SetEnabled(enabled);
    gFMSModel->SetAutonomous(autonomous);
    gFMSModel->SetTest(test);
    gFMSModel->UpdateHAL();
  } else {
    HALSIM_SetDriverStationEnabled(enabled);
    HALSIM_SetDriverStationAutonomous(autonomous);
    HALSIM_SetDriverStationTest(test);
  }
}

static void DriverStationExecute() {
  // update sources
  for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
    auto& source = gJoystickSources[i];
    uint16_t axesAvailable;
    uint8_t povsAvailable;
    uint64_t buttonsAvailable;
    HALSIM_GetJoystickAvailables(i, &axesAvailable, &buttonsAvailable,
                                 &povsAvailable);
    if (axesAvailable != 0 || buttonsAvailable != 0 || povsAvailable != 0) {
      uint8_t axesCount =
          static_cast<uint8_t>(16 - std::countl_zero(axesAvailable));
      uint8_t buttonCount =
          static_cast<uint8_t>(64 - std::countl_zero(buttonsAvailable));
      uint8_t povCount =
          static_cast<uint8_t>(8 - std::countl_zero(povsAvailable));

      if (!source || source->axisCount != axesCount ||
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
    if (auto win = DriverStationGui::dsManager->GetWindow("System Joysticks")) {
      win->SetVisibility(wpi::glass::Window::kDisabled);
    }
  } else if (!disableDS && prevDisableDS) {
    if (auto win = DriverStationGui::dsManager->GetWindow("System Joysticks")) {
      win->SetVisibility(wpi::glass::Window::kShow);
    }
  }
  prevDisableDS = disableDS;
  if (disableDS) {
    gFMSModel->Update();
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

  bool isAttached = HALSIM_GetDriverStationDsAttached();
  bool isEnabled = HALSIM_GetDriverStationEnabled();
  bool isAuto = HALSIM_GetDriverStationAutonomous();
  bool isTest = HALSIM_GetDriverStationTest();

  // Robot state
  {
    // DS hotkeys
    bool enableHotkey = false;
    bool disableHotkey = false;
    if (gpUseEnableDisableHotkeys != nullptr && *gpUseEnableDisableHotkeys) {
      ImGuiIO& io = ImGui::GetIO();
      if (io.KeysDown[GLFW_KEY_ENTER] || io.KeysDown[GLFW_KEY_KP_ENTER]) {
        disableHotkey = true;
      } else if (io.KeysDown[GLFW_KEY_LEFT_BRACKET] &&
                 io.KeysDown[GLFW_KEY_RIGHT_BRACKET] &&
                 io.KeysDown[GLFW_KEY_BACKSLASH]) {
        enableHotkey = true;
      }
    }
    if (gpUseEstopHotkey != nullptr && *gpUseEstopHotkey) {
      ImGuiIO& io = ImGui::GetIO();
      if (io.KeysDown[GLFW_KEY_SPACE]) {
        HALSIM_SetDriverStationEnabled(false);
      }
    }

    ImGui::SetNextWindowPos(ImVec2{5, 20}, ImGuiCond_FirstUseEver);
    const char* title = "Robot State";
    // Accounts for size of title and collapse button
    float minWidth = ImGui::CalcTextSize(title).x + ImGui::GetFontSize() +
                     ImGui::GetStyle().ItemInnerSpacing.x * 2 +
                     ImGui::GetStyle().FramePadding.x * 2 +
                     ImGui::GetStyle().WindowBorderSize;
    ImGui::SetNextWindowSizeConstraints(ImVec2{minWidth, 0},
                                        ImVec2{FLT_MAX, FLT_MAX});
    ImGui::Begin(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Selectable("Disconnected", !isAttached)) {
      HALSIM_SetDriverStationEnabled(false);
      HALSIM_SetDriverStationDsAttached(false);
      isAttached = false;
      gFMSModel->Update();
    }
    if (ImGui::Selectable("Disabled", isAttached && !isEnabled) ||
        disableHotkey) {
      DriverStationConnect(false, false, false);
    }
    if (ImGui::Selectable("Autonomous",
                          isAttached && isEnabled && isAuto && !isTest)) {
      DriverStationConnect(true, true, false);
    }
    if (ImGui::Selectable("Teleoperated",
                          isAttached && isEnabled && !isAuto && !isTest) ||
        enableHotkey) {
      DriverStationConnect(true, false, false);
    }
    if (ImGui::Selectable("Test", isEnabled && isTest)) {
      DriverStationConnect(true, false, true);
    }
    ImGui::End();
  }

  // Update HAL
  if (isAttached && !isAuto) {
    for (int i = 0, end = gRobotJoysticks.size();
         i < end && i < HAL_kMaxJoysticks; ++i) {
      gRobotJoysticks[i].SetHAL(i);
    }
  }

  // Send new data every 20 ms (may be slower depending on GUI refresh rate)
  static double lastNewDataTime = 0.0;
  if ((curTime - lastNewDataTime) > 0.02 && !HALSIM_IsTimingPaused() &&
      isAttached) {
    lastNewDataTime = curTime;
    gFMSModel->Update();
    HALSIM_NotifyDriverStationNewData();
  }
}

FMSSimModel::FMSSimModel() {
  m_matchTime.SetValue(-1.0);
  m_allianceStationId.SetValue(HAL_AllianceStationID_kRed1);
}

void FMSSimModel::UpdateHAL() {
  HALSIM_SetDriverStationFmsAttached(m_fmsAttached.GetValue());
  HALSIM_SetDriverStationAllianceStationId(
      static_cast<HAL_AllianceStationID>(m_allianceStationId.GetValue()));
  HALSIM_SetDriverStationEStop(m_estop.GetValue());
  HALSIM_SetDriverStationEnabled(m_enabled.GetValue());
  HALSIM_SetDriverStationTest(m_test.GetValue());
  HALSIM_SetDriverStationAutonomous(m_autonomous.GetValue());
  HALSIM_SetDriverStationMatchTime(m_matchTime.GetValue());
  auto str = wpi::util::make_string(m_gameMessage.GetValue());
  HALSIM_SetGameSpecificMessage(&str);
  HALSIM_SetDriverStationDsAttached(m_dsAttached.GetValue());
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
  if (!IsDSDisabled() && enabled) {
    int32_t status = 0;
    double curTime = HAL_GetFPGATime(&status) * 1.0e-6;
    if (m_startMatchTime == -1.0) {
      m_startMatchTime = matchTime + curTime;
    }
    matchTime = m_startMatchTime - curTime;
    if (matchTime < 0) {
      matchTime = -1.0;
    }
    HALSIM_SetDriverStationMatchTime(matchTime);
  } else {
    if (m_startMatchTime != -1.0) {
      matchTime = -1.0;
      HALSIM_SetDriverStationMatchTime(matchTime);
    }
    m_startMatchTime = -1.0;
  }
  m_matchTime.SetValue(matchTime);

  HAL_MatchInfo info;
  HALSIM_GetMatchInfo(&info);
  m_gameMessage.SetValue(
      std::string_view{reinterpret_cast<const char*>(info.gameSpecificMessage),
                       reinterpret_cast<const char*>(info.gameSpecificMessage) +
                           info.gameSpecificMessageSize});
}

bool FMSSimModel::IsReadOnly() {
  return IsDSDisabled();
}

static void DisplaySystemJoystick(SystemJoystick& joy, int i) {
  char label[64];
  wpi::util::format_to_n_c_str(label, sizeof(label), "{}: {}", i,
                               joy.GetName());

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
    auto joy = gKeyboardJoysticks[i].get();
    DisplaySystemJoystick(*joy, i + GLFW_JOYSTICK_LAST + 1);
    if (ImGui::BeginPopupContextItem()) {
      char buf[64];
      wpi::util::format_to_n_c_str(buf, sizeof(buf), "{} Settings",
                                   joy->GetName());

      if (ImGui::MenuItem(buf)) {
        if (auto win = DriverStationGui::dsManager->GetWindow(buf)) {
          win->SetVisible(true);
        }
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
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
            joy2.guid.clear();
          }
        }
        joy.sys = payload_sys;
        joy.guid = payload_sys->GetGUID();
        std::string_view name{payload_sys->GetName()};
        joy.useGamepad = wpi::util::starts_with(name, "Xbox") ||
                         wpi::util::contains(name, "pad");
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

    if ((disableDS && joy.data.desc.gamepadType > 0) ||
        (joy.sys && joy.sys->IsPresent())) {
      // update GUI display
      ImGui::PushID(i);
      if (disableDS) {
        ImGui::Text("%s", joy.data.desc.name);
        ImGui::Text("Gamepad: %s", joy.data.desc.isGamepad ? "Yes" : "No");
      } else {
        ImGui::Text("%d: %s", joy.sys->GetIndex(), joy.sys->GetName());

        if (joy.sys->IsGamepad()) {
          ImGui::Checkbox("Map gamepad", &joy.useGamepad);
        }
      }

      uint8_t axesCount =
          static_cast<uint8_t>(16 - std::countl_zero(joy.data.axes.available));

      for (int j = 0; j < axesCount; ++j) {
        if (source && source->axes[j]) {
          char label[64];
          wpi::util::format_to_n_c_str(label, sizeof(label), "Axis[{}]", j);

          ImGui::Selectable(label);
          source->axes[j]->EmitDrag();
          ImGui::SameLine();
          ImGui::Text(": %.3f", joy.data.axes.axes[j]);
        } else {
          ImGui::Text("Axis[%d]: %.3f", j, joy.data.axes.axes[j]);
        }
      }

      uint8_t povCount =
          static_cast<uint8_t>(16 - std::countl_zero(joy.data.povs.available));

      for (int j = 0; j < povCount; ++j) {
        if (source && source->povs[j]) {
          char label[64];
          wpi::util::format_to_n_c_str(label, sizeof(label), "POVs[{}]", j);

          ImGui::Selectable(label);
          source->povs[j]->EmitDrag();
          ImGui::SameLine();
          ImGui::Text(": %d", joy.data.povs.povs[j]);
        } else {
          ImGui::Text("POVs[%d]: %d", j, joy.data.povs.povs[j]);
        }
      }

      uint8_t buttonCount = static_cast<uint8_t>(
          64 - std::countl_zero(joy.data.buttons.available));

      // show buttons as multiple lines of LED indicators, 8 per line
      static const ImU32 color = IM_COL32(255, 255, 102, 255);
      wpi::util::SmallVector<int, 64> buttons;
      buttons.resize(buttonCount);
      for (int j = 0; j < buttonCount; ++j) {
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
  if (gpDSSocketConnected && *gpDSSocketConnected) {
    ImGui::MenuItem("Turn off DS (real DS connected)", nullptr, true, false);
  } else {
    if (gpDisableDS != nullptr) {
      ImGui::MenuItem("Turn off DS", nullptr, gpDisableDS);
    }
    if (gpZeroDisconnectedJoysticks != nullptr) {
      ImGui::MenuItem("Zero disconnected joysticks", nullptr,
                      gpZeroDisconnectedJoysticks);
    }
    if (gpUseEnableDisableHotkeys != nullptr) {
      ImGui::MenuItem("Enable on []\\ combo, Disable on Enter", nullptr,
                      gpUseEnableDisableHotkeys);
    }
    if (gpUseEstopHotkey != nullptr) {
      ImGui::MenuItem("Disable on Spacebar", nullptr, gpUseEstopHotkey);
    }
  }
  ImGui::Separator();

  for (auto&& window : m_windows) {
    window->DisplayMenuItem();
  }
}

void DriverStationGui::GlobalInit() {
  auto& storageRoot = wpi::glass::GetStorageRoot("ds");
  dsManager = std::make_unique<DSManager>(storageRoot);

  // set up system joysticks (both GLFW and keyboard)
  for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i) {
    gGlfwJoysticks.emplace_back(std::make_unique<GlfwSystemJoystick>(i));
  }

  dsManager->GlobalInit();

  gFMSModel = std::make_unique<FMSSimModel>();

  wpi::gui::AddEarlyExecute(DriverStationExecute);

  storageRoot.SetCustomApply([&storageRoot] {
    gpDisableDS = &storageRoot.GetBool("disable", false);
    gpZeroDisconnectedJoysticks =
        &storageRoot.GetBool("zeroDisconnectedJoysticks", true);
    gpUseEnableDisableHotkeys =
        &storageRoot.GetBool("useEnableDisableHotkeys", false);
    gpUseEstopHotkey = &storageRoot.GetBool("useEstopHotkey", false);

    auto& keyboardStorage = storageRoot.GetChildArray("keyboardJoysticks");
    keyboardStorage.resize(4);
    for (int i = 0; i < 4; ++i) {
      if (!keyboardStorage[i]) {
        keyboardStorage[i] = std::make_unique<wpi::glass::Storage>();
      }
      gKeyboardJoysticks.emplace_back(
          std::make_unique<GlfwKeyboardJoystick>(*keyboardStorage[i], i));
    }

    auto& robotJoystickStorage = storageRoot.GetChildArray("robotJoysticks");
    robotJoystickStorage.resize(HAL_kMaxJoysticks);
    for (int i = 0; i < HAL_kMaxJoysticks; ++i) {
      if (!robotJoystickStorage[i]) {
        robotJoystickStorage[i] = std::make_unique<wpi::glass::Storage>();
      }
      gRobotJoysticks.emplace_back(*robotJoystickStorage[i]);
    }

    int i = 0;
    for (auto&& joy : gKeyboardJoysticks) {
      char label[64];
      wpi::util::format_to_n_c_str(label, sizeof(label), "{} Settings",
                                   joy->GetName());

      if (auto win = dsManager->AddWindow(
              label, [j = joy.get()] { j->SettingsDisplay(); },
              wpi::glass::Window::kHide)) {
        win->DisableRenamePopup();
        win->SetDefaultPos(10 + 310 * i++, 50);
        if (i > 3) {
          i = 0;
        }
        win->SetDefaultSize(300, 560);
      }
    }
    if (auto win = dsManager->AddWindow("FMS", [] {
          if (HALSIM_GetDriverStationDsAttached()) {
            DisplayFMSReadOnly(gFMSModel.get());
          } else {
            DisplayFMS(gFMSModel.get(), false);
          }
        })) {
      win->DisableRenamePopup();
      win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
      win->SetDefaultPos(5, 540);
    }
    if (auto win =
            dsManager->AddWindow("System Joysticks", DisplaySystemJoysticks)) {
      win->DisableRenamePopup();
      win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
      win->SetDefaultPos(5, 350);
    }
    if (auto win = dsManager->AddWindow("Joysticks", DisplayJoysticks)) {
      win->DisableRenamePopup();
      win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
      win->SetDefaultPos(250, 465);
    }
  });

  storageRoot.SetCustomClear([&storageRoot] {
    dsManager->EraseWindows();
    gKeyboardJoysticks.clear();
    gRobotJoysticks.clear();
    storageRoot.GetChildArray("keyboardJoysticks").clear();
    storageRoot.GetChildArray("robotJoysticks").clear();
    storageRoot.ClearValues();
  });
}

void DriverStationGui::SetDSSocketExtension(void* data) {
  gpDSSocketConnected = static_cast<std::atomic<bool>*>(data);
}
