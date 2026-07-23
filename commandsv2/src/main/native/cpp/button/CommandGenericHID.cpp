// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandGenericHID.hpp"

#include <array>
#include <format>
#include <memory>
#include <unordered_map>

#include "wpi/commands2/Commands.hpp"

using namespace wpi::cmd;

namespace {
std::mutex hidsMutex;
std::array<std::unique_ptr<CommandGenericHID>,
           wpi::internal::DriverStationBackend::JOYSTICK_PORTS>
    hids;
}  // namespace

std::array<std::unique_ptr<SubsystemBase>, 5>& CommandGenericHID::GetSubsystems(
    wpi::GenericHID& hid) {
  if (auto it = m_subsystems.find(&hid); it == m_subsystems.end()) {
    return m_subsystems
        .emplace(&hid,
                 std::array{
                     std::make_unique<SubsystemBase>(std::format(
                         "Controller {} Left Rumble", hid.GetPort())),
                     std::make_unique<SubsystemBase>(std::format(
                         "Controller {} Right Rumble", hid.GetPort())),
                     std::make_unique<SubsystemBase>(std::format(
                         "Controller {} Left Trigger Rumble", hid.GetPort())),
                     std::make_unique<SubsystemBase>(std::format(
                         "Controller {} Right Trigger Rumble", hid.GetPort())),
                     std::make_unique<SubsystemBase>(
                         std::format("Controller {} LEDs", hid.GetPort())),
                 })
        .first->second;
  } else {
    return it->second;
  }
}

std::unordered_map<wpi::GenericHID*,
                   std::array<std::unique_ptr<SubsystemBase>, 5>>
    CommandGenericHID::m_subsystems;

CommandGenericHID::CommandGenericHID(int port)
    : CommandGenericHID{wpi::DriverStation::GetGenericHID(port)} {}

CommandGenericHID::CommandGenericHID(wpi::GenericHID& hid)
    : CommandGenericHID{hid, GetSubsystems(hid)} {}

CommandGenericHID::CommandGenericHID(
    wpi::GenericHID& hid,
    std::array<std::unique_ptr<SubsystemBase>, 5>& subsystems)
    : m_leftRumble{*subsystems[0]},
      m_rightRumble{*subsystems[1]},
      m_leftTriggerRumble{*subsystems[2]},
      m_rightTriggerRumble{*subsystems[3]},
      m_leds{*subsystems[4]},
      m_hid{&hid} {}

CommandGenericHID& CommandGenericHID::GetCommandGenericHID(int port) {
  auto& hid = wpi::DriverStation::GetGenericHID(port);
  std::scoped_lock lock{hidsMutex};

  if (!hids[port]) {
    hids[port] = std::make_unique<CommandGenericHID>(hid);
  }

  return *hids[port];
}

wpi::GenericHID& CommandGenericHID::GetHID() {
  return *m_hid;
}

Trigger CommandGenericHID::Button(int button, wpi::EventLoop* loop) const {
  return Trigger(loop, [this, button] { return m_hid->GetRawButton(button); });
}

Trigger CommandGenericHID::POV(wpi::POVDirection angle,
                               wpi::EventLoop* loop) const {
  return POV(0, angle, loop);
}

Trigger CommandGenericHID::POV(int pov, wpi::POVDirection angle,
                               wpi::EventLoop* loop) const {
  return Trigger(loop,
                 [this, pov, angle] { return m_hid->GetPOV(pov) == angle; });
}

Trigger CommandGenericHID::POVUp(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::UP, loop);
}

Trigger CommandGenericHID::POVUpRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::UP_RIGHT, loop);
}

Trigger CommandGenericHID::POVRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::RIGHT, loop);
}

Trigger CommandGenericHID::POVDownRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::DOWN_RIGHT, loop);
}

Trigger CommandGenericHID::POVDown(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::DOWN, loop);
}

Trigger CommandGenericHID::POVDownLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::DOWN_LEFT, loop);
}

Trigger CommandGenericHID::POVLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::LEFT, loop);
}

Trigger CommandGenericHID::POVUpLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::UP_LEFT, loop);
}

Trigger CommandGenericHID::POVCenter(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::CENTER, loop);
}

Trigger CommandGenericHID::AxisLessThan(int axis, double threshold,
                                        wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid->GetRawAxis(axis) < threshold;
  });
}

Trigger CommandGenericHID::AxisGreaterThan(int axis, double threshold,
                                           wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid->GetRawAxis(axis) > threshold;
  });
}

Trigger CommandGenericHID::AxisMagnitudeGreaterThan(
    int axis, double threshold, wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return std::abs(m_hid->GetRawAxis(axis)) > threshold;
  });
}

void CommandGenericHID::SetRumble(wpi::GenericHID::RumbleType type,
                                  double value) {
  m_hid->SetRumble(type, value);
}

bool CommandGenericHID::IsConnected() const {
  return m_hid->IsConnected();
}

CommandPtr CommandGenericHID::Rumble(SubsystemBase& subsystem,
                                     std::string_view name,
                                     wpi::GenericHID::RumbleType type,
                                     double value) {
  return subsystem
      .StartEnd([this, type, value] { SetRumble(type, value); },
                [this, type] { SetRumble(type, 0); })
      .WithName(name);
}

CommandPtr CommandGenericHID::RumbleLeft(double value) {
  return Rumble(m_leftRumble, "Rumble Left",
                wpi::GenericHID::RumbleType::LEFT_RUMBLE, value);
}

CommandPtr CommandGenericHID::RumbleRight(double value) {
  return Rumble(m_rightRumble, "Rumble Right",
                wpi::GenericHID::RumbleType::RIGHT_RUMBLE, value);
}

CommandPtr CommandGenericHID::RumbleBoth(double value) {
  return Parallel(RumbleLeft(value), RumbleRight(value))
      .WithName("Both Rumble");
}

CommandPtr CommandGenericHID::RumbleLeftTrigger(double value) {
  return Rumble(m_leftTriggerRumble, "Rumble Left Trigger",
                wpi::GenericHID::RumbleType::LEFT_TRIGGER_RUMBLE, value);
}

CommandPtr CommandGenericHID::RumbleRightTrigger(double value) {
  return Rumble(m_rightTriggerRumble, "Rumble Right Trigger",
                wpi::GenericHID::RumbleType::RIGHT_TRIGGER_RUMBLE, value);
}

CommandPtr CommandGenericHID::RumbleTriggers(double value) {
  return Parallel(RumbleLeftTrigger(value), RumbleRightTrigger(value))
      .WithName("Both Trigger Rumble");
}

CommandPtr CommandGenericHID::SetLeds(int r, int g, int b) const {
  return m_leds
      .StartEnd([this, r, g, b] { m_hid->SetLeds(r, g, b); },
                [this] { m_hid->SetLeds(0, 0, 0); })
      .WithName("Set LEDs (" + std::to_string(r) + ", " + std::to_string(g) +
                ", " + std::to_string(b) + ")");
}

CommandPtr CommandGenericHID::SetLeds(const util::Color& color) const {
  return SetLeds(static_cast<int>(color.red * 255),
                 static_cast<int>(color.green * 255),
                 static_cast<int>(color.blue * 255));
}

CommandPtr CommandGenericHID::SetLeds(const util::Color8Bit& color) const {
  return SetLeds(color.red, color.green, color.blue);
}
