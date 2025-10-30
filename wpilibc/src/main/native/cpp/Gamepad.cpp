// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Gamepad.h"

#include <hal/UsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/event/BooleanEvent.h"

using namespace frc;

Gamepad::Gamepad(int port) : GenericHID(port) {
  HAL_ReportUsage("HID", port, "Gamepad");
}

double Gamepad::GetLeftX() const {
  return GetRawAxis(Axis::kLeftX);
}

double Gamepad::GetLeftY() const {
  return GetRawAxis(Axis::kLeftY);
}

double Gamepad::GetRightX() const {
  return GetRawAxis(Axis::kRightX);
}

double Gamepad::GetRightY() const {
  return GetRawAxis(Axis::kRightY);
}

double Gamepad::GetLeftTriggerAxis() const {
  return GetRawAxis(Axis::kLeftTrigger);
}

BooleanEvent Gamepad::LeftTrigger(double threshold, EventLoop* loop) const {
  return BooleanEvent(loop, [this, threshold] {
    return this->GetLeftTriggerAxis() > threshold;
  });
}

BooleanEvent Gamepad::LeftTrigger(EventLoop* loop) const {
  return this->LeftTrigger(0.5, loop);
}

double Gamepad::GetRightTriggerAxis() const {
  return GetRawAxis(Axis::kRightTrigger);
}

BooleanEvent Gamepad::RightTrigger(double threshold, EventLoop* loop) const {
  return BooleanEvent(loop, [this, threshold] {
    return this->GetRightTriggerAxis() > threshold;
  });
}

BooleanEvent Gamepad::RightTrigger(EventLoop* loop) const {
  return this->RightTrigger(0.5, loop);
}

bool Gamepad::GetSouthFaceButton() const {
  return GetRawButton(Button::kSouthFace);
}

bool Gamepad::GetSouthFaceButtonPressed() {
  return GetRawButtonPressed(Button::kSouthFace);
}

bool Gamepad::GetSouthFaceButtonReleased() {
  return GetRawButtonReleased(Button::kSouthFace);
}

BooleanEvent Gamepad::SouthFace(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetSouthFaceButton(); });
}

bool Gamepad::GetEastFaceButton() const {
  return GetRawButton(Button::kEastFace);
}

bool Gamepad::GetEastFaceButtonPressed() {
  return GetRawButtonPressed(Button::kEastFace);
}

bool Gamepad::GetEastFaceButtonReleased() {
  return GetRawButtonReleased(Button::kEastFace);
}

BooleanEvent Gamepad::EastFace(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetEastFaceButton(); });
}

bool Gamepad::GetWestFaceButton() const {
  return GetRawButton(Button::kWestFace);
}

bool Gamepad::GetWestFaceButtonPressed() {
  return GetRawButtonPressed(Button::kWestFace);
}

bool Gamepad::GetWestFaceButtonReleased() {
  return GetRawButtonReleased(Button::kWestFace);
}

BooleanEvent Gamepad::WestFace(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetWestFaceButton(); });
}

bool Gamepad::GetNorthFaceButton() const {
  return GetRawButton(Button::kNorthFace);
}

bool Gamepad::GetNorthFaceButtonPressed() {
  return GetRawButtonPressed(Button::kNorthFace);
}

bool Gamepad::GetNorthFaceButtonReleased() {
  return GetRawButtonReleased(Button::kNorthFace);
}

BooleanEvent Gamepad::NorthFace(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetNorthFaceButton(); });
}

bool Gamepad::GetBackButton() const {
  return GetRawButton(Button::kBack);
}

bool Gamepad::GetBackButtonPressed() {
  return GetRawButtonPressed(Button::kBack);
}

bool Gamepad::GetBackButtonReleased() {
  return GetRawButtonReleased(Button::kBack);
}

BooleanEvent Gamepad::Back(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetBackButton(); });
}

bool Gamepad::GetGuideButton() const {
  return GetRawButton(Button::kGuide);
}

bool Gamepad::GetGuideButtonPressed() {
  return GetRawButtonPressed(Button::kGuide);
}

bool Gamepad::GetGuideButtonReleased() {
  return GetRawButtonReleased(Button::kGuide);
}

BooleanEvent Gamepad::Guide(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetGuideButton(); });
}

bool Gamepad::GetStartButton() const {
  return GetRawButton(Button::kStart);
}

bool Gamepad::GetStartButtonPressed() {
  return GetRawButtonPressed(Button::kStart);
}

bool Gamepad::GetStartButtonReleased() {
  return GetRawButtonReleased(Button::kStart);
}

BooleanEvent Gamepad::Start(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetStartButton(); });
}

bool Gamepad::GetLeftStickButton() const {
  return GetRawButton(Button::kLeftStick);
}

bool Gamepad::GetLeftStickButtonPressed() {
  return GetRawButtonPressed(Button::kLeftStick);
}

bool Gamepad::GetLeftStickButtonReleased() {
  return GetRawButtonReleased(Button::kLeftStick);
}

BooleanEvent Gamepad::LeftStick(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftStickButton(); });
}

bool Gamepad::GetRightStickButton() const {
  return GetRawButton(Button::kRightStick);
}

bool Gamepad::GetRightStickButtonPressed() {
  return GetRawButtonPressed(Button::kRightStick);
}

bool Gamepad::GetRightStickButtonReleased() {
  return GetRawButtonReleased(Button::kRightStick);
}

BooleanEvent Gamepad::RightStick(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightStickButton(); });
}

bool Gamepad::GetLeftShoulderButton() const {
  return GetRawButton(Button::kLeftShoulder);
}

bool Gamepad::GetLeftShoulderButtonPressed() {
  return GetRawButtonPressed(Button::kLeftShoulder);
}

bool Gamepad::GetLeftShoulderButtonReleased() {
  return GetRawButtonReleased(Button::kLeftShoulder);
}

BooleanEvent Gamepad::LeftShoulder(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftShoulderButton(); });
}

bool Gamepad::GetRightShoulderButton() const {
  return GetRawButton(Button::kRightShoulder);
}

bool Gamepad::GetRightShoulderButtonPressed() {
  return GetRawButtonPressed(Button::kRightShoulder);
}

bool Gamepad::GetRightShoulderButtonReleased() {
  return GetRawButtonReleased(Button::kRightShoulder);
}

BooleanEvent Gamepad::RightShoulder(EventLoop* loop) const {
  return BooleanEvent(loop,
                      [this]() { return this->GetRightShoulderButton(); });
}

bool Gamepad::GetDpadUpButton() const {
  return GetRawButton(Button::kDpadUp);
}

bool Gamepad::GetDpadUpButtonPressed() {
  return GetRawButtonPressed(Button::kDpadUp);
}

bool Gamepad::GetDpadUpButtonReleased() {
  return GetRawButtonReleased(Button::kDpadUp);
}

BooleanEvent Gamepad::DpadUp(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadUpButton(); });
}

bool Gamepad::GetDpadDownButton() const {
  return GetRawButton(Button::kDpadDown);
}

bool Gamepad::GetDpadDownButtonPressed() {
  return GetRawButtonPressed(Button::kDpadDown);
}

bool Gamepad::GetDpadDownButtonReleased() {
  return GetRawButtonReleased(Button::kDpadDown);
}

BooleanEvent Gamepad::DpadDown(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadDownButton(); });
}

bool Gamepad::GetDpadLeftButton() const {
  return GetRawButton(Button::kDpadLeft);
}

bool Gamepad::GetDpadLeftButtonPressed() {
  return GetRawButtonPressed(Button::kDpadLeft);
}

bool Gamepad::GetDpadLeftButtonReleased() {
  return GetRawButtonReleased(Button::kDpadLeft);
}

BooleanEvent Gamepad::DpadLeft(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadLeftButton(); });
}

bool Gamepad::GetDpadRightButton() const {
  return GetRawButton(Button::kDpadRight);
}

bool Gamepad::GetDpadRightButtonPressed() {
  return GetRawButtonPressed(Button::kDpadRight);
}

bool Gamepad::GetDpadRightButtonReleased() {
  return GetRawButtonReleased(Button::kDpadRight);
}

BooleanEvent Gamepad::DpadRight(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetDpadRightButton(); });
}

bool Gamepad::GetMisc1Button() const {
  return GetRawButton(Button::kMisc1);
}

bool Gamepad::GetMisc1ButtonPressed() {
  return GetRawButtonPressed(Button::kMisc1);
}

bool Gamepad::GetMisc1ButtonReleased() {
  return GetRawButtonReleased(Button::kMisc1);
}

BooleanEvent Gamepad::Misc1(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc1Button(); });
}

bool Gamepad::GetRightPaddle1Button() const {
  return GetRawButton(Button::kRightPaddle1);
}

bool Gamepad::GetRightPaddle1ButtonPressed() {
  return GetRawButtonPressed(Button::kRightPaddle1);
}

bool Gamepad::GetRightPaddle1ButtonReleased() {
  return GetRawButtonReleased(Button::kRightPaddle1);
}

BooleanEvent Gamepad::RightPaddle1(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightPaddle1Button(); });
}

bool Gamepad::GetLeftPaddle1Button() const {
  return GetRawButton(Button::kLeftPaddle1);
}

bool Gamepad::GetLeftPaddle1ButtonPressed() {
  return GetRawButtonPressed(Button::kLeftPaddle1);
}

bool Gamepad::GetLeftPaddle1ButtonReleased() {
  return GetRawButtonReleased(Button::kLeftPaddle1);
}

BooleanEvent Gamepad::LeftPaddle1(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftPaddle1Button(); });
}

bool Gamepad::GetRightPaddle2Button() const {
  return GetRawButton(Button::kRightPaddle2);
}

bool Gamepad::GetRightPaddle2ButtonPressed() {
  return GetRawButtonPressed(Button::kRightPaddle2);
}

bool Gamepad::GetRightPaddle2ButtonReleased() {
  return GetRawButtonReleased(Button::kRightPaddle2);
}

BooleanEvent Gamepad::RightPaddle2(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightPaddle2Button(); });
}

bool Gamepad::GetLeftPaddle2Button() const {
  return GetRawButton(Button::kLeftPaddle2);
}

bool Gamepad::GetLeftPaddle2ButtonPressed() {
  return GetRawButtonPressed(Button::kLeftPaddle2);
}

bool Gamepad::GetLeftPaddle2ButtonReleased() {
  return GetRawButtonReleased(Button::kLeftPaddle2);
}

BooleanEvent Gamepad::LeftPaddle2(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftPaddle2Button(); });
}

bool Gamepad::GetTouchpadButton() const {
  return GetRawButton(Button::kTouchpad);
}

bool Gamepad::GetTouchpadButtonPressed() {
  return GetRawButtonPressed(Button::kTouchpad);
}

bool Gamepad::GetTouchpadButtonReleased() {
  return GetRawButtonReleased(Button::kTouchpad);
}

BooleanEvent Gamepad::Touchpad(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetTouchpadButton(); });
}

bool Gamepad::GetMisc2Button() const {
  return GetRawButton(Button::kMisc2);
}

bool Gamepad::GetMisc2ButtonPressed() {
  return GetRawButtonPressed(Button::kMisc2);
}

bool Gamepad::GetMisc2ButtonReleased() {
  return GetRawButtonReleased(Button::kMisc2);
}

BooleanEvent Gamepad::Misc2(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc2Button(); });
}

bool Gamepad::GetMisc3Button() const {
  return GetRawButton(Button::kMisc3);
}

bool Gamepad::GetMisc3ButtonPressed() {
  return GetRawButtonPressed(Button::kMisc3);
}

bool Gamepad::GetMisc3ButtonReleased() {
  return GetRawButtonReleased(Button::kMisc3);
}

BooleanEvent Gamepad::Misc3(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc3Button(); });
}

bool Gamepad::GetMisc4Button() const {
  return GetRawButton(Button::kMisc4);
}

bool Gamepad::GetMisc4ButtonPressed() {
  return GetRawButtonPressed(Button::kMisc4);
}

bool Gamepad::GetMisc4ButtonReleased() {
  return GetRawButtonReleased(Button::kMisc4);
}

BooleanEvent Gamepad::Misc4(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc4Button(); });
}

bool Gamepad::GetMisc5Button() const {
  return GetRawButton(Button::kMisc5);
}

bool Gamepad::GetMisc5ButtonPressed() {
  return GetRawButtonPressed(Button::kMisc5);
}

bool Gamepad::GetMisc5ButtonReleased() {
  return GetRawButtonReleased(Button::kMisc5);
}

BooleanEvent Gamepad::Misc5(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc5Button(); });
}

bool Gamepad::GetMisc6Button() const {
  return GetRawButton(Button::kMisc6);
}

bool Gamepad::GetMisc6ButtonPressed() {
  return GetRawButtonPressed(Button::kMisc6);
}

bool Gamepad::GetMisc6ButtonReleased() {
  return GetRawButtonReleased(Button::kMisc6);
}

BooleanEvent Gamepad::Misc6(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetMisc6Button(); });
}

double Gamepad::GetAxisForSendable(int axis) const {
  return DriverStation::GetStickAxisIfAvailable(GetPort(), axis).value_or(0.0);
}

bool Gamepad::GetButtonForSendable(int button) const {
  return DriverStation::GetStickButtonIfAvailable(GetPort(), button)
      .value_or(false);
}

void Gamepad::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("HID");
  builder.PublishConstString("ControllerType", "Gamepad");
  builder.AddDoubleProperty(
      "LeftTrigger Axis",
      [this] { return GetAxisForSendable(Axis::kLeftTrigger); }, nullptr);
  builder.AddDoubleProperty(
      "RightTrigger Axis",
      [this] { return GetAxisForSendable(Axis::kRightTrigger); }, nullptr);
  builder.AddDoubleProperty(
      "LeftX", [this] { return GetAxisForSendable(Axis::kLeftX); }, nullptr);
  builder.AddDoubleProperty(
      "LeftY", [this] { return GetAxisForSendable(Axis::kLeftY); }, nullptr);
  builder.AddDoubleProperty(
      "RightX", [this] { return GetAxisForSendable(Axis::kRightX); }, nullptr);
  builder.AddDoubleProperty(
      "RightY", [this] { return GetAxisForSendable(Axis::kRightY); }, nullptr);
  builder.AddBooleanProperty(
      "SouthFace", [this] { return GetButtonForSendable(Button::kSouthFace); },
      nullptr);
  builder.AddBooleanProperty(
      "EastFace", [this] { return GetButtonForSendable(Button::kEastFace); },
      nullptr);
  builder.AddBooleanProperty(
      "WestFace", [this] { return GetButtonForSendable(Button::kWestFace); },
      nullptr);
  builder.AddBooleanProperty(
      "NorthFace",
      [this] { return GetButtonForSendable(Button::kNorthFace); }, nullptr);
  builder.AddBooleanProperty(
      "Back", [this] { return GetButtonForSendable(Button::kBack); }, nullptr);
  builder.AddBooleanProperty(
      "Guide", [this] { return GetButtonForSendable(Button::kGuide); },
      nullptr);
  builder.AddBooleanProperty(
      "Start", [this] { return GetButtonForSendable(Button::kStart); },
      nullptr);
  builder.AddBooleanProperty(
      "LeftStick", [this] { return GetButtonForSendable(Button::kLeftStick); },
      nullptr);
  builder.AddBooleanProperty(
      "RightStick",
      [this] { return GetButtonForSendable(Button::kRightStick); }, nullptr);
  builder.AddBooleanProperty(
      "LeftShoulder",
      [this] { return GetButtonForSendable(Button::kLeftShoulder); }, nullptr);
  builder.AddBooleanProperty(
      "RightShoulder",
      [this] { return GetButtonForSendable(Button::kRightShoulder); }, nullptr);
  builder.AddBooleanProperty(
      "DpadUp", [this] { return GetButtonForSendable(Button::kDpadUp); },
      nullptr);
  builder.AddBooleanProperty(
      "DpadDown", [this] { return GetButtonForSendable(Button::kDpadDown); },
      nullptr);
  builder.AddBooleanProperty(
      "DpadLeft", [this] { return GetButtonForSendable(Button::kDpadLeft); },
      nullptr);
  builder.AddBooleanProperty(
      "DpadRight", [this] { return GetButtonForSendable(Button::kDpadRight); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc1", [this] { return GetButtonForSendable(Button::kMisc1); },
      nullptr);
  builder.AddBooleanProperty(
      "RightPaddle1",
      [this] { return GetButtonForSendable(Button::kRightPaddle1); }, nullptr);
  builder.AddBooleanProperty(
      "LeftPaddle1",
      [this] { return GetButtonForSendable(Button::kLeftPaddle1); }, nullptr);
  builder.AddBooleanProperty(
      "RightPaddle2",
      [this] { return GetButtonForSendable(Button::kRightPaddle2); }, nullptr);
  builder.AddBooleanProperty(
      "LeftPaddle2",
      [this] { return GetButtonForSendable(Button::kLeftPaddle2); }, nullptr);
  builder.AddBooleanProperty(
      "Touchpad", [this] { return GetButtonForSendable(Button::kTouchpad); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc2", [this] { return GetButtonForSendable(Button::kMisc2); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc3", [this] { return GetButtonForSendable(Button::kMisc3); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc4", [this] { return GetButtonForSendable(Button::kMisc4); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc5", [this] { return GetButtonForSendable(Button::kMisc5); },
      nullptr);
  builder.AddBooleanProperty(
      "Misc6", [this] { return GetButtonForSendable(Button::kMisc6); },
      nullptr);
}
