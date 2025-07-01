// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Gamepad.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/event/BooleanEvent.hpp"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi;

static double ClampDeadband(double deadband) {
  if (std::isnan(deadband)) {
    return 0.0;
  }
  return std::clamp(deadband, 0.0, std::nextafter(1.0, 0.0));
}

Gamepad::Gamepad(int port) : Gamepad{DriverStation::GetGenericHID(port)} {}

Gamepad::Gamepad(GenericHID& hid) : m_hid{&hid} {
  HAL_ReportUsage("HID", hid.GetPort(), "Gamepad");
}

GenericHID& Gamepad::GetHID() {
  return *m_hid;
}

const GenericHID& Gamepad::GetHID() const {
  return *m_hid;
}

double Gamepad::GetLeftX() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::LEFT_X), m_leftXDeadband);
}

void Gamepad::SetLeftXDeadband(double deadband) {
  m_leftXDeadband = ClampDeadband(deadband);
}

double Gamepad::GetLeftY() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::LEFT_Y), m_leftYDeadband);
}

void Gamepad::SetLeftYDeadband(double deadband) {
  m_leftYDeadband = ClampDeadband(deadband);
}

double Gamepad::GetRightX() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::RIGHT_X), m_rightXDeadband);
}

void Gamepad::SetRightXDeadband(double deadband) {
  m_rightXDeadband = ClampDeadband(deadband);
}

double Gamepad::GetRightY() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::RIGHT_Y), m_rightYDeadband);
}

void Gamepad::SetRightYDeadband(double deadband) {
  m_rightYDeadband = ClampDeadband(deadband);
}

double Gamepad::GetLeftTrigger() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::LEFT_TRIGGER),
                                  m_leftTriggerDeadband);
}

void Gamepad::SetLeftTriggerDeadband(double deadband) {
  m_leftTriggerDeadband = ClampDeadband(deadband);
}

BooleanEvent Gamepad::LeftTrigger(double threshold, EventLoop* loop) const {
  return BooleanEvent(loop, [this, threshold] {
    return this->GetAxis(Axis::LEFT_TRIGGER) > threshold;
  });
}

BooleanEvent Gamepad::LeftTrigger(EventLoop* loop) const {
  return this->LeftTrigger(0.5, loop);
}

double Gamepad::GetRightTrigger() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::RIGHT_TRIGGER),
                                  m_rightTriggerDeadband);
}

void Gamepad::SetRightTriggerDeadband(double deadband) {
  m_rightTriggerDeadband = ClampDeadband(deadband);
}

BooleanEvent Gamepad::RightTrigger(double threshold, EventLoop* loop) const {
  return BooleanEvent(loop, [this, threshold] {
    return this->GetAxis(Axis::RIGHT_TRIGGER) > threshold;
  });
}

BooleanEvent Gamepad::RightTrigger(EventLoop* loop) const {
  return this->RightTrigger(0.5, loop);
}

bool Gamepad::GetFaceDownButton() const {
  return GetButton(Button::FACE_DOWN);
}

bool Gamepad::GetFaceDownButtonPressed() {
  return GetButtonPressed(Button::FACE_DOWN);
}

bool Gamepad::GetFaceDownButtonReleased() {
  return GetButtonReleased(Button::FACE_DOWN);
}

BooleanEvent Gamepad::FaceDown(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetFaceDownButton(); });
}

bool Gamepad::GetFaceRightButton() const {
  return GetButton(Button::FACE_RIGHT);
}

bool Gamepad::GetFaceRightButtonPressed() {
  return GetButtonPressed(Button::FACE_RIGHT);
}

bool Gamepad::GetFaceRightButtonReleased() {
  return GetButtonReleased(Button::FACE_RIGHT);
}

BooleanEvent Gamepad::FaceRight(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetFaceRightButton(); });
}

bool Gamepad::GetFaceLeftButton() const {
  return GetButton(Button::FACE_LEFT);
}

bool Gamepad::GetFaceLeftButtonPressed() {
  return GetButtonPressed(Button::FACE_LEFT);
}

bool Gamepad::GetFaceLeftButtonReleased() {
  return GetButtonReleased(Button::FACE_LEFT);
}

BooleanEvent Gamepad::FaceLeft(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetFaceLeftButton(); });
}

bool Gamepad::GetFaceUpButton() const {
  return GetButton(Button::FACE_UP);
}

bool Gamepad::GetFaceUpButtonPressed() {
  return GetButtonPressed(Button::FACE_UP);
}

bool Gamepad::GetFaceUpButtonReleased() {
  return GetButtonReleased(Button::FACE_UP);
}

BooleanEvent Gamepad::FaceUp(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetFaceUpButton(); });
}

bool Gamepad::GetBackButton() const {
  return GetButton(Button::BACK);
}

bool Gamepad::GetBackButtonPressed() {
  return GetButtonPressed(Button::BACK);
}

bool Gamepad::GetBackButtonReleased() {
  return GetButtonReleased(Button::BACK);
}

BooleanEvent Gamepad::Back(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetBackButton(); });
}

bool Gamepad::GetGuideButton() const {
  return GetButton(Button::GUIDE);
}

bool Gamepad::GetGuideButtonPressed() {
  return GetButtonPressed(Button::GUIDE);
}

bool Gamepad::GetGuideButtonReleased() {
  return GetButtonReleased(Button::GUIDE);
}

BooleanEvent Gamepad::Guide(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetGuideButton(); });
}

bool Gamepad::GetStartButton() const {
  return GetButton(Button::START);
}

bool Gamepad::GetStartButtonPressed() {
  return GetButtonPressed(Button::START);
}

bool Gamepad::GetStartButtonReleased() {
  return GetButtonReleased(Button::START);
}

BooleanEvent Gamepad::Start(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetStartButton(); });
}

bool Gamepad::GetLeftStickButton() const {
  return GetButton(Button::LEFT_STICK);
}

bool Gamepad::GetLeftStickButtonPressed() {
  return GetButtonPressed(Button::LEFT_STICK);
}

bool Gamepad::GetLeftStickButtonReleased() {
  return GetButtonReleased(Button::LEFT_STICK);
}

BooleanEvent Gamepad::LeftStick(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftStickButton(); });
}

bool Gamepad::GetRightStickButton() const {
  return GetButton(Button::RIGHT_STICK);
}

bool Gamepad::GetRightStickButtonPressed() {
  return GetButtonPressed(Button::RIGHT_STICK);
}

bool Gamepad::GetRightStickButtonReleased() {
  return GetButtonReleased(Button::RIGHT_STICK);
}

BooleanEvent Gamepad::RightStick(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightStickButton(); });
}

bool Gamepad::GetLeftBumperButton() const {
  return GetButton(Button::LEFT_BUMPER);
}

bool Gamepad::GetLeftBumperButtonPressed() {
  return GetButtonPressed(Button::LEFT_BUMPER);
}

bool Gamepad::GetLeftBumperButtonReleased() {
  return GetButtonReleased(Button::LEFT_BUMPER);
}

BooleanEvent Gamepad::LeftBumper(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftBumperButton(); });
}

bool Gamepad::GetRightBumperButton() const {
  return GetButton(Button::RIGHT_BUMPER);
}

bool Gamepad::GetRightBumperButtonPressed() {
  return GetButtonPressed(Button::RIGHT_BUMPER);
}

bool Gamepad::GetRightBumperButtonReleased() {
  return GetButtonReleased(Button::RIGHT_BUMPER);
}

BooleanEvent Gamepad::RightBumper(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightBumperButton(); });
}

bool Gamepad::GetDpadUpButton() const {
  return GetButton(Button::DPAD_UP);
}

bool Gamepad::GetDpadUpButtonPressed() {
  return GetButtonPressed(Button::DPAD_UP);
}

bool Gamepad::GetDpadUpButtonReleased() {
  return GetButtonReleased(Button::DPAD_UP);
}

BooleanEvent Gamepad::DpadUp(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadUpButton(); });
}

bool Gamepad::GetDpadDownButton() const {
  return GetButton(Button::DPAD_DOWN);
}

bool Gamepad::GetDpadDownButtonPressed() {
  return GetButtonPressed(Button::DPAD_DOWN);
}

bool Gamepad::GetDpadDownButtonReleased() {
  return GetButtonReleased(Button::DPAD_DOWN);
}

BooleanEvent Gamepad::DpadDown(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadDownButton(); });
}

bool Gamepad::GetDpadLeftButton() const {
  return GetButton(Button::DPAD_LEFT);
}

bool Gamepad::GetDpadLeftButtonPressed() {
  return GetButtonPressed(Button::DPAD_LEFT);
}

bool Gamepad::GetDpadLeftButtonReleased() {
  return GetButtonReleased(Button::DPAD_LEFT);
}

BooleanEvent Gamepad::DpadLeft(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadLeftButton(); });
}

bool Gamepad::GetDpadRightButton() const {
  return GetButton(Button::DPAD_RIGHT);
}

bool Gamepad::GetDpadRightButtonPressed() {
  return GetButtonPressed(Button::DPAD_RIGHT);
}

bool Gamepad::GetDpadRightButtonReleased() {
  return GetButtonReleased(Button::DPAD_RIGHT);
}

BooleanEvent Gamepad::DpadRight(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadRightButton(); });
}

bool Gamepad::GetMisc1Button() const {
  return GetButton(Button::MISC_1);
}

bool Gamepad::GetMisc1ButtonPressed() {
  return GetButtonPressed(Button::MISC_1);
}

bool Gamepad::GetMisc1ButtonReleased() {
  return GetButtonReleased(Button::MISC_1);
}

BooleanEvent Gamepad::Misc1(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc1Button(); });
}

bool Gamepad::GetRightPaddle1Button() const {
  return GetButton(Button::RIGHT_PADDLE_1);
}

bool Gamepad::GetRightPaddle1ButtonPressed() {
  return GetButtonPressed(Button::RIGHT_PADDLE_1);
}

bool Gamepad::GetRightPaddle1ButtonReleased() {
  return GetButtonReleased(Button::RIGHT_PADDLE_1);
}

BooleanEvent Gamepad::RightPaddle1(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightPaddle1Button(); });
}

bool Gamepad::GetLeftPaddle1Button() const {
  return GetButton(Button::LEFT_PADDLE_1);
}

bool Gamepad::GetLeftPaddle1ButtonPressed() {
  return GetButtonPressed(Button::LEFT_PADDLE_1);
}

bool Gamepad::GetLeftPaddle1ButtonReleased() {
  return GetButtonReleased(Button::LEFT_PADDLE_1);
}

BooleanEvent Gamepad::LeftPaddle1(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftPaddle1Button(); });
}

bool Gamepad::GetRightPaddle2Button() const {
  return GetButton(Button::RIGHT_PADDLE_2);
}

bool Gamepad::GetRightPaddle2ButtonPressed() {
  return GetButtonPressed(Button::RIGHT_PADDLE_2);
}

bool Gamepad::GetRightPaddle2ButtonReleased() {
  return GetButtonReleased(Button::RIGHT_PADDLE_2);
}

BooleanEvent Gamepad::RightPaddle2(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightPaddle2Button(); });
}

bool Gamepad::GetLeftPaddle2Button() const {
  return GetButton(Button::LEFT_PADDLE_2);
}

bool Gamepad::GetLeftPaddle2ButtonPressed() {
  return GetButtonPressed(Button::LEFT_PADDLE_2);
}

bool Gamepad::GetLeftPaddle2ButtonReleased() {
  return GetButtonReleased(Button::LEFT_PADDLE_2);
}

BooleanEvent Gamepad::LeftPaddle2(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftPaddle2Button(); });
}

bool Gamepad::GetTouchpadButton() const {
  return GetButton(Button::TOUCHPAD);
}

bool Gamepad::GetTouchpadButtonPressed() {
  return GetButtonPressed(Button::TOUCHPAD);
}

bool Gamepad::GetTouchpadButtonReleased() {
  return GetButtonReleased(Button::TOUCHPAD);
}

BooleanEvent Gamepad::Touchpad(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetTouchpadButton(); });
}

bool Gamepad::GetMisc2Button() const {
  return GetButton(Button::MISC_2);
}

bool Gamepad::GetMisc2ButtonPressed() {
  return GetButtonPressed(Button::MISC_2);
}

bool Gamepad::GetMisc2ButtonReleased() {
  return GetButtonReleased(Button::MISC_2);
}

BooleanEvent Gamepad::Misc2(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc2Button(); });
}

bool Gamepad::GetMisc3Button() const {
  return GetButton(Button::MISC_3);
}

bool Gamepad::GetMisc3ButtonPressed() {
  return GetButtonPressed(Button::MISC_3);
}

bool Gamepad::GetMisc3ButtonReleased() {
  return GetButtonReleased(Button::MISC_3);
}

BooleanEvent Gamepad::Misc3(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc3Button(); });
}

bool Gamepad::GetMisc4Button() const {
  return GetButton(Button::MISC_4);
}

bool Gamepad::GetMisc4ButtonPressed() {
  return GetButtonPressed(Button::MISC_4);
}

bool Gamepad::GetMisc4ButtonReleased() {
  return GetButtonReleased(Button::MISC_4);
}

BooleanEvent Gamepad::Misc4(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc4Button(); });
}

bool Gamepad::GetMisc5Button() const {
  return GetButton(Button::MISC_5);
}

bool Gamepad::GetMisc5ButtonPressed() {
  return GetButtonPressed(Button::MISC_5);
}

bool Gamepad::GetMisc5ButtonReleased() {
  return GetButtonReleased(Button::MISC_5);
}

BooleanEvent Gamepad::Misc5(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc5Button(); });
}

bool Gamepad::GetMisc6Button() const {
  return GetButton(Button::MISC_6);
}

bool Gamepad::GetMisc6ButtonPressed() {
  return GetButtonPressed(Button::MISC_6);
}

bool Gamepad::GetMisc6ButtonReleased() {
  return GetButtonReleased(Button::MISC_6);
}

BooleanEvent Gamepad::Misc6(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc6Button(); });
}

bool Gamepad::GetButton(Button button) const {
  return m_hid->GetRawButton(static_cast<int>(button));
}

bool Gamepad::GetButtonPressed(Button button) {
  return m_hid->GetRawButtonPressed(static_cast<int>(button));
}

bool Gamepad::GetButtonReleased(Button button) {
  return m_hid->GetRawButtonReleased(static_cast<int>(button));
}

BooleanEvent Gamepad::ButtonEvent(Button button, EventLoop* loop) const {
  return m_hid->Button(static_cast<int>(button), loop);
}

double Gamepad::GetAxis(Axis axis) const {
  return m_hid->GetRawAxis(static_cast<int>(axis));
}

BooleanEvent Gamepad::AxisLessThan(Axis axis, double threshold,
                                   EventLoop* loop) const {
  return m_hid->AxisLessThan(static_cast<int>(axis), threshold, loop);
}

BooleanEvent Gamepad::AxisGreaterThan(Axis axis, double threshold,
                                      EventLoop* loop) const {
  return m_hid->AxisGreaterThan(static_cast<int>(axis), threshold, loop);
}

int Gamepad::GetAxesAvailable() const {
  return m_hid->GetAxesAvailable();
}

uint64_t Gamepad::GetButtonsAvailable() const {
  return m_hid->GetButtonsAvailable();
}

bool Gamepad::IsConnected() const {
  return m_hid->IsConnected();
}

GenericHID::HIDType Gamepad::GetGamepadType() const {
  return m_hid->GetGamepadType();
}

GenericHID::SupportedOutputs Gamepad::GetSupportedOutputs() const {
  return m_hid->GetSupportedOutputs();
}

std::string Gamepad::GetName() const {
  return m_hid->GetName();
}

int Gamepad::GetPort() const {
  return m_hid->GetPort();
}

void Gamepad::SetLeds(int r, int g, int b) {
  m_hid->SetLeds(r, g, b);
}

void Gamepad::SetRumble(GenericHID::RumbleType type, double value) {
  m_hid->SetRumble(type, value);
}

bool Gamepad::GetTouchpadFingerAvailable(int touchpad, int finger) const {
  return m_hid->GetTouchpadFingerAvailable(touchpad, finger);
}

TouchpadFinger Gamepad::GetTouchpadFinger(int touchpad, int finger) const {
  return m_hid->GetTouchpadFinger(touchpad, finger);
}

double Gamepad::GetAxisForTelemetry(Axis axis) const {
  return wpi::internal::DriverStationBackend::GetStickAxisIfAvailable(
             m_hid->GetPort(), static_cast<int>(axis))
      .value_or(0.0);
}

void Gamepad::LogTo(wpi::TelemetryTable& table) const {
  table.Log("LeftTriggerAxis", GetAxisForTelemetry(Axis::LEFT_TRIGGER));
  table.Log("RightTriggerAxis", GetAxisForTelemetry(Axis::RIGHT_TRIGGER));
  table.Log("LeftXAxis", GetAxisForTelemetry(Axis::LEFT_X));
  table.Log("LeftYAxis", GetAxisForTelemetry(Axis::LEFT_Y));
  table.Log("RightXAxis", GetAxisForTelemetry(Axis::RIGHT_X));
  table.Log("RightYAxis", GetAxisForTelemetry(Axis::RIGHT_Y));

  uint64_t buttons =
      wpi::internal::DriverStationBackend::GetStickButtons(m_hid->GetPort());
  table.Log("FaceDown",
            (buttons & (1UL << static_cast<int>(Button::FACE_DOWN))) != 0);
  table.Log("FaceRight",
            (buttons & (1UL << static_cast<int>(Button::FACE_RIGHT))) != 0);
  table.Log("FaceLeft",
            (buttons & (1UL << static_cast<int>(Button::FACE_LEFT))) != 0);
  table.Log("FaceUp",
            (buttons & (1UL << static_cast<int>(Button::FACE_UP))) != 0);
  table.Log("Back", (buttons & (1UL << static_cast<int>(Button::BACK))) != 0);
  table.Log("Guide", (buttons & (1UL << static_cast<int>(Button::GUIDE))) != 0);
  table.Log("Start", (buttons & (1UL << static_cast<int>(Button::START))) != 0);
  table.Log("LeftStick",
            (buttons & (1UL << static_cast<int>(Button::LEFT_STICK))) != 0);
  table.Log("RightStick",
            (buttons & (1UL << static_cast<int>(Button::RIGHT_STICK))) != 0);
  table.Log("LeftBumper",
            (buttons & (1UL << static_cast<int>(Button::LEFT_BUMPER))) != 0);
  table.Log("RightBumper",
            (buttons & (1UL << static_cast<int>(Button::RIGHT_BUMPER))) != 0);
  table.Log("DpadUp",
            (buttons & (1UL << static_cast<int>(Button::DPAD_UP))) != 0);
  table.Log("DpadDown",
            (buttons & (1UL << static_cast<int>(Button::DPAD_DOWN))) != 0);
  table.Log("DpadLeft",
            (buttons & (1UL << static_cast<int>(Button::DPAD_LEFT))) != 0);
  table.Log("DpadRight",
            (buttons & (1UL << static_cast<int>(Button::DPAD_RIGHT))) != 0);
  table.Log("Misc1",
            (buttons & (1UL << static_cast<int>(Button::MISC_1))) != 0);
  table.Log("RightPaddle1",
            (buttons & (1UL << static_cast<int>(Button::RIGHT_PADDLE_1))) != 0);
  table.Log("LeftPaddle1",
            (buttons & (1UL << static_cast<int>(Button::LEFT_PADDLE_1))) != 0);
  table.Log("RightPaddle2",
            (buttons & (1UL << static_cast<int>(Button::RIGHT_PADDLE_2))) != 0);
  table.Log("LeftPaddle2",
            (buttons & (1UL << static_cast<int>(Button::LEFT_PADDLE_2))) != 0);
  table.Log("Touchpad",
            (buttons & (1UL << static_cast<int>(Button::TOUCHPAD))) != 0);
  table.Log("Misc2",
            (buttons & (1UL << static_cast<int>(Button::MISC_2))) != 0);
  table.Log("Misc3",
            (buttons & (1UL << static_cast<int>(Button::MISC_3))) != 0);
  table.Log("Misc4",
            (buttons & (1UL << static_cast<int>(Button::MISC_4))) != 0);
  table.Log("Misc5",
            (buttons & (1UL << static_cast<int>(Button::MISC_5))) != 0);
  table.Log("Misc6",
            (buttons & (1UL << static_cast<int>(Button::MISC_6))) != 0);
}

std::string_view Gamepad::GetTelemetryType() const {
  return "HID:Gamepad";
}
