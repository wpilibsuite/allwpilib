// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/livewindow/LiveWindow.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/simulation/DriverStationSim.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include <networktables/BooleanTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

#include "gtest/gtest.h"

using namespace frc;

class LiveWindowTest : public ::testing::Test {
 public:
  LiveWindowTest()
      : m_smartDashboardTable{nt::NetworkTableInstance::GetDefault().GetTable(
            "SmartDashboard")},
        m_liveWindowTable{
            nt::NetworkTableInstance::GetDefault().GetTable("LiveWindow")} {}
  std::shared_ptr<nt::NetworkTable> m_smartDashboardTable;
  std::shared_ptr<nt::NetworkTable> m_liveWindowTable;
};

TEST_F(LiveWindowTest, EnableDisableCallbacks) {
  int counter = 0;
  nt::BooleanSubscriber subscriber = m_liveWindowTable->GetSubTable(".status")
                                         ->GetBooleanTopic("LW Enabled")
                                         .Subscribe(false);
  frc::LiveWindow::SetEnabled(false);
  frc::LiveWindow::SetEnabledCallback([&counter] { counter++; });
  frc::LiveWindow::SetDisabledCallback([&counter] { counter--; });

  EXPECT_EQ(0, counter);
  EXPECT_FALSE(subscriber.Get());

  frc::LiveWindow::SetEnabled(true);
  EXPECT_EQ(1, counter);
  EXPECT_TRUE(subscriber.Get());

  frc::LiveWindow::SetEnabled(false);
  EXPECT_EQ(0, counter);
  EXPECT_FALSE(subscriber.Get());
}

class LiveWindowPwmControlTest : public LiveWindowTest,
                                 public ::testing::WithParamInterface<bool> {};

TEST_P(LiveWindowPwmControlTest, PwmControl) {
  frc::LiveWindow::SetEnabled(false);
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  std::shared_ptr<nt::NetworkTable> pwmTable =
      m_smartDashboardTable->GetSubTable("MyPWM");

  frc::PWMSparkMax pwm{0};
  nt::BooleanSubscriber controllable =
      pwmTable->GetBooleanTopic(".controllable").Subscribe(false);
  nt::DoublePublisher pwmValue = pwmTable->GetDoubleTopic("Value").Publish();
  nt::BooleanSubscriber lwControllable =
      m_liveWindowTable->GetSubTable("Ungrouped")
          ->GetSubTable("PWMSparkMax[0]")
          ->GetBooleanTopic(".controllable")
          .Subscribe(false);

  // Shouldn't be published yet
  EXPECT_EQ(0, controllable.GetAtomic().time);
  EXPECT_EQ(0, lwControllable.GetAtomic().time);

  frc::SmartDashboard::PutData("MyPWM", &pwm);
  frc::SmartDashboard::UpdateValues();
  frc::LiveWindow::UpdateValues();

  // Should be published now
  auto publishedValue = controllable.GetAtomic();
  EXPECT_NE(0, publishedValue.time);
  EXPECT_FALSE(publishedValue.value);

  EXPECT_EQ(0, lwControllable.GetAtomic().time);

  frc::LiveWindow::SetEnabled(GetParam());

  pwmValue.Set(0.5);

  frc::SmartDashboard::UpdateValues();
  frc::LiveWindow::UpdateValues();

  // LiveWindow doesn't change `.controllable` in place!
  EXPECT_FALSE(controllable.Get());

  // Instead, LiveWindow publishes everything to the LiveWindow table
  auto lwControllableTimestamped = lwControllable.GetAtomic();
  if (GetParam()) {
    EXPECT_NE(0, lwControllableTimestamped.time);
    EXPECT_TRUE(lwControllableTimestamped.value);
  } else {
    EXPECT_EQ(0, lwControllableTimestamped.time);
  }
  EXPECT_DOUBLE_EQ(GetParam() ? 0.5 : 0, pwm.Get());
}

INSTANTIATE_TEST_SUITE_P(LiveWindowTests, LiveWindowPwmControlTest,
                         ::testing::Bool());
