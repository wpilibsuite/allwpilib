// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>
#include <string_view>
#include <utility>
#include <vector>

#include <frc/smartdashboard/SmartDashboard.h>
#include <networktables/BooleanTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringArrayTopic.h>
#include <wpi/sendable/Sendable.h>

#include "CommandTestBase.h"
#include "frc2/command/CommandPtr.h"
#include "frc2/command/Commands.h"
#include "make_vector.h"

using namespace frc2;

using SendableChooserTestPublisherFunc = std::function<void(wpi::Sendable*)>;
using SendableChooserTestArgs =
    std::pair<std::function<frc2::CommandPtr(SendablePublisherFunc)>,
              std::vector<std::string_view>>;

class SendableChooserCommandTest
    : public CommandTestBaseWithParam<SendableChooserTestArgs> {
 protected:
  nt::BooleanPublisher m_publish;
  void SetUp() override {
    m_publish = nt::NetworkTableInstance::GetDefault()
                    .GetBooleanTopic("/SmartDashboard/chooser")
                    .Publish();
    frc::SmartDashboard::UpdateValues();
  }
};

TEST_P(SendableChooserCommandTest, OptionsAreCorrect) {
  auto&& [commandFactory, names] = GetParam();
  nt::StringArraySubscriber optionsSubscriber =
      nt::NetworkTableInstance::GetDefault()
          .GetStringArrayTopic("/SmartDashboard/chooser/options")
          .Subscribe({});
  auto cmd = commandFactory(
      [&](wpi::Sendable* c) { frc::SmartDashboard::PutData("chooser", c); });
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ(names, optionsSubscriber.Get());
}

namespace utils {
static const frc2::CommandPtr CommandNamed(std::string_view name) {
  return frc2::cmd::Print(name).WithName(name);
}

static const auto OptionsAreCorrectParams() {
  SendableChooserTestArgs empty{[](SendableChooserTestPublisherFunc func) {
                                  return frc2::cmd::Choose(func);
                                },
                                std::vector<std::string_view>()};

  SendableChooserTestArgs duplicateName{
      [](SendableChooserTestPublisherFunc(func) {
        return frc2::cmd::Choose(func, CommandNamed("a"), CommandNamed("b"), CommandNamed("a")),
      }, make_vector<std::string_view>("a", "b")};

  SendableChooserTestArgs happyPath{
      [](SendableChooserTestPublisherFunc(func) {
        return frc2::cmd::Choose(func, CommandNamed("a"), CommandNamed("b"), CommandNamed("c")),
      }, make_vector<std::string_view>("a", "b", "c")};

  return testing::Values(empty, duplicateName, happyPath);
}

}  // namespace utils

INSTANTIATE_TEST_SUITE_P(SendableChooserCommandTests,
                         SendableChooserCommandTest,
                         utils::OptionsAreCorrectParams());
