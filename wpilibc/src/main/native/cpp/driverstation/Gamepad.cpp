// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Gamepad.hpp"

#include "wpi/event/BooleanEvent.hpp"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"

using namespace wpi;

Gamepad::Gamepad(int port) : GenericHID(port) {
  HAL_ReportUsage("HID", port, "Gamepad");
}

double Gamepad::GetLeftX() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::LEFT_X), m_leftXDeadband);
}

void Gamepad::SetLeftXDeadband(double deadband) {
  m_leftXDeadband = deadband;
}

double Gamepad::GetLeftY() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::LEFT_Y), m_leftYDeadband);
}

void Gamepad::SetLeftYDeadband(double deadband) {
  m_leftYDeadband = deadband;
}

double Gamepad::GetRightX() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::RIGHT_X), m_rightXDeadband);
}

void Gamepad::SetRightXDeadband(double deadband) {
  m_rightXDeadband = deadband;
}

double Gamepad::GetRightY() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::RIGHT_Y), m_rightYDeadband);
}

void Gamepad::SetRightYDeadband(double deadband) {
  m_rightYDeadband = deadband;
}

double Gamepad::GetLeftTriggerAxis() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::LEFT_TRIGGER),
                                  m_leftTriggerDeadband);
}

void Gamepad::SetLeftTriggerDeadband(double deadband) {
  m_leftTriggerDeadband = deadband;
}

BooleanEvent Gamepad::LeftTrigger(double threshold, EventLoop* loop) const {
  return BooleanEvent(loop, [this, threshold] {
    return this->GetAxis(Axis::LEFT_TRIGGER) > threshold;
  });
}

BooleanEvent Gamepad::LeftTrigger(EventLoop* loop) const {
  return this->LeftTrigger(0.5, loop);
}

double Gamepad::GetRightTriggerAxis() const {
  return wpi::math::ApplyDeadband(GetAxis(Axis::RIGHT_TRIGGER),
                                  m_rightTriggerDeadband);
}

void Gamepad::SetRightTriggerDeadband(double deadband) {
  m_rightTriggerDeadband = deadband;
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
  return GetRawButton(static_cast<int>(button));
}

bool Gamepad::GetButtonPressed(Button button) {
  return GetRawButtonPressed(static_cast<int>(button));
}

bool Gamepad::GetButtonReleased(Button button) {
  return GetRawButtonReleased(static_cast<int>(button));
}

BooleanEvent Gamepad::ButtonEvent(Button button, EventLoop* loop) const {
  return GenericHID::Button(static_cast<int>(button), loop);
}

double Gamepad::GetAxis(Axis axis) const {
  return GetRawAxis(static_cast<int>(axis));
}

BooleanEvent Gamepad::AxisLessThan(Axis axis, double threshold,
                                   EventLoop* loop) const {
  return GenericHID::AxisLessThan(static_cast<int>(axis), threshold, loop);
}

BooleanEvent Gamepad::AxisGreaterThan(Axis axis, double threshold,
                                      EventLoop* loop) const {
  return GenericHID::AxisGreaterThan(static_cast<int>(axis), threshold, loop);
}

double Gamepad::GetAxisForSendable(Axis axis) const {
  return wpi::internal::DriverStationBackend::GetStickAxisIfAvailable(
             GetPort(), static_cast<int>(axis))
      .value_or(0.0);
}

bool Gamepad::GetButtonForSendable(Button button) const {
  return wpi::internal::DriverStationBackend::GetStickButtonIfAvailable(
             GetPort(), static_cast<int>(button))
      .value_or(false);
}

void Gamepad::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("HID");
  builder.PublishConstString("ControllerType", "Gamepad");
  builder.AddDoubleProperty(
      "LeftTrigger Axis",
      [this] { return GetAxisForSendable(Axis::LEFT_TRIGGER); }, nullptr);
  builder.AddDoubleProperty(
      "RightTrigger Axis",
      [this] { return GetAxisForSendable(Axis::RIGHT_TRIGGER); }, nullptr);
  builder.AddDoubleProperty(
      "LeftX", [this] { return GetAxisForSendable(Axis::LEFT_X); }, nullptr);
  builder.AddDoubleProperty(
      "LeftY", [this] { return GetAxisForSendable(Axis::LEFT_Y); }, nullptr);
  builder.AddDoubleProperty(
      "RightX", [this] { return GetAxisForSendable(Axis::RIGHT_X); }, nullptr);
  builder.AddDoubleProperty(
      "RightY", [this] { return GetAxisForSendable(Axis::RIGHT_Y); }, nullptr);
  builder.AddBooleanProperty(
      "FaceDown", [this] { return GetButtonForSendable(Button::FACE_DOWN); },
      nullptr);
  builder.AddBooleanProperty(
      "FaceRight", [this] { return GetButtonForSendable(Button::FACE_RIGHT); },
      nullptr);
  builder.AddBooleanProperty(
      "FaceLeft", [this] { return GetButtonForSendable(Button::FACE_LEFT); },
      nullptr);
  builder.AddBooleanProperty(
      "FaceUp", [this] { return GetButtonForSendable(Button::FACE_UP); },
      nullptr);
  builder.AddBooleanProperty(
      "Back", [this] { return GetButtonForSendable(Button::BACK); }, nullptr);
  builder.AddBooleanProperty(
      "Guide", [this] { return GetButtonForSendable(Button::GUIDE); }, nullptr);
  builder.AddBooleanProperty(
      "Start", [this] { return GetButtonForSendable(Button::START); }, nullptr);
  builder.AddBooleanProperty(
      "LeftStick", [this] { return GetButtonForSendable(Button::LEFT_STICK); },
      nullptr);
  builder.AddBooleanProperty(
      "RightStick",
      [this] { return GetButtonForSendable(Button::RIGHT_STICK); }, nullptr);
  builder.AddBooleanProperty(
      "LeftBumper",
      [this] { return GetButtonForSendable(Button::LEFT_BUMPER); }, nullptr);
  builder.AddBooleanProperty(
      "RightBumper",
      [this] { return GetButtonForSendable(Button::RIGHT_BUMPER); }, nullptr);
  builder.AddBooleanProperty(
      "DpadUp", [this] { return GetButtonForSendable(Button::DPAD_UP); },
      nullptr);
  builder.AddBooleanProperty(
      "DpadDown", [this] { return GetButtonForSendable(Button::DPAD_DOWN); },
      nullptr);
  builder.AddBooleanProperty(
      "DpadLeft", [this] { return GetButtonForSendable(Button::DPAD_LEFT); },
      nullptr);
  builder.AddBooleanProperty(
      "DpadRight", [this] { return GetButtonForSendable(Button::DPAD_RIGHT); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc1", [this] { return GetButtonForSendable(Button::MISC_1); },
      nullptr);
  builder.AddBooleanProperty(
      "RightPaddle1",
      [this] { return GetButtonForSendable(Button::RIGHT_PADDLE_1); }, nullptr);
  builder.AddBooleanProperty(
      "LeftPaddle1",
      [this] { return GetButtonForSendable(Button::LEFT_PADDLE_1); }, nullptr);
  builder.AddBooleanProperty(
      "RightPaddle2",
      [this] { return GetButtonForSendable(Button::RIGHT_PADDLE_2); }, nullptr);
  builder.AddBooleanProperty(
      "LeftPaddle2",
      [this] { return GetButtonForSendable(Button::LEFT_PADDLE_2); }, nullptr);
  builder.AddBooleanProperty(
      "Touchpad", [this] { return GetButtonForSendable(Button::TOUCHPAD); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc2", [this] { return GetButtonForSendable(Button::MISC_2); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc3", [this] { return GetButtonForSendable(Button::MISC_3); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc4", [this] { return GetButtonForSendable(Button::MISC_4); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc5", [this] { return GetButtonForSendable(Button::MISC_5); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc6", [this] { return GetButtonForSendable(Button::MISC_6); },
      nullptr);
}
