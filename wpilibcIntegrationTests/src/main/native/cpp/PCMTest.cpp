// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestBench.h"
#include "frc/AnalogInput.h"
#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"
#include "frc/DoubleSolenoid.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/Solenoid.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

/* The PCM switches the compressor up to a couple seconds after the pressure
        switch changes. */
static constexpr auto kCompressorDelayTime = 3_s;

/* Solenoids should change much more quickly */
static constexpr auto kSolenoidDelayTime = 0.5_s;

/* The voltage divider on the test bench should bring the compressor output
        to around these values. */
static const double kCompressorOnVoltage = 5.00;
static const double kCompressorOffVoltage = 1.68;

class PCMTest : public testing::Test {
 protected:
  frc::PneumaticsControlModule m_pneumaticsModule;

  frc::DigitalOutput m_fakePressureSwitch{
      TestBench::kFakePressureSwitchChannel};
  frc::AnalogInput m_fakeCompressor{TestBench::kFakeCompressorChannel};
  frc::DigitalInput m_fakeSolenoid1{TestBench::kFakeSolenoid1Channel};
  frc::DigitalInput m_fakeSolenoid2{TestBench::kFakeSolenoid2Channel};

  void Reset() {
    m_pneumaticsModule.DisableCompressor();
    m_fakePressureSwitch.Set(false);
  }
};

/**
 * Test if the compressor turns on and off when the pressure switch is toggled
 */
TEST_F(PCMTest, PressureSwitch) {
  Reset();

  m_pneumaticsModule.EnableCompressorDigital();

  // Turn on the compressor
  m_fakePressureSwitch.Set(true);
  frc::Wait(kCompressorDelayTime);
  EXPECT_NEAR(kCompressorOnVoltage, m_fakeCompressor.GetVoltage(), 0.5)
      << "Compressor did not turn on when the pressure switch turned on.";

  // Turn off the compressor
  m_fakePressureSwitch.Set(false);
  frc::Wait(kCompressorDelayTime);
  EXPECT_NEAR(kCompressorOffVoltage, m_fakeCompressor.GetVoltage(), 0.5)
      << "Compressor did not turn off when the pressure switch turned off.";
}

/**
 * Test if the correct solenoids turn on and off when they should
 */
TEST_F(PCMTest, Solenoid) {
  Reset();
  frc::Solenoid solenoid1{frc::PneumaticsModuleType::CTREPCM,
                          TestBench::kSolenoidChannel1};
  frc::Solenoid solenoid2{frc::PneumaticsModuleType::CTREPCM,
                          TestBench::kSolenoidChannel2};

  // Turn both solenoids off
  solenoid1.Set(false);
  solenoid2.Set(false);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Turn one solenoid on and one off
  solenoid1.Set(true);
  solenoid2.Set(false);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn on";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_TRUE(solenoid1.Get()) << "Solenoid #1 did not read on";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Turn one solenoid on and one off
  solenoid1.Set(false);
  solenoid2.Set(true);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_FALSE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn on";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";

  // Turn both on
  solenoid1.Set(true);
  solenoid2.Set(true);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn on";
  EXPECT_FALSE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn on";
  EXPECT_TRUE(solenoid1.Get()) << "Solenoid #1 did not read on";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";
}

/**
 * Test if the correct solenoids turn on and off when they should when used
 * with the DoubleSolenoid class.
 */
TEST_F(PCMTest, DoubleSolenoid) {
  frc::DoubleSolenoid solenoid{frc::PneumaticsModuleType::CTREPCM,
                               TestBench::kSolenoidChannel1,
                               TestBench::kSolenoidChannel2};

  solenoid.Set(frc::DoubleSolenoid::kOff);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_TRUE(solenoid.Get() == frc::DoubleSolenoid::kOff)
      << "Solenoid does not read off";

  solenoid.Set(frc::DoubleSolenoid::kForward);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn on";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_TRUE(solenoid.Get() == frc::DoubleSolenoid::kForward)
      << "Solenoid does not read forward";

  solenoid.Set(frc::DoubleSolenoid::kReverse);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_FALSE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn on";
  EXPECT_TRUE(solenoid.Get() == frc::DoubleSolenoid::kReverse)
      << "Solenoid does not read reverse";
}

TEST_F(PCMTest, OneShot) {
  Reset();
  frc::Solenoid solenoid1{frc::PneumaticsModuleType::CTREPCM,
                          TestBench::kSolenoidChannel1};
  frc::Solenoid solenoid2{frc::PneumaticsModuleType::CTREPCM,
                          TestBench::kSolenoidChannel2};

  // Turn both solenoids off
  solenoid1.Set(false);
  solenoid2.Set(false);
  frc::Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Pulse Solenoid #1 on, and turn Solenoid #2 off
  solenoid1.SetPulseDuration(2 * kSolenoidDelayTime);
  solenoid2.Set(false);
  solenoid1.StartPulse();
  frc::Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn on";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_TRUE(solenoid1.Get()) << "Solenoid #1 did not read on";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";
  frc::Wait(2 * kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Turn Solenoid #1 off, and pulse Solenoid #2 on
  solenoid1.Set(false);
  solenoid2.SetPulseDuration(2 * kSolenoidDelayTime);
  solenoid2.StartPulse();
  frc::Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_FALSE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn on";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";
  frc::Wait(2 * kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Pulse both Solenoids on
  solenoid1.SetPulseDuration(2 * kSolenoidDelayTime);
  solenoid2.SetPulseDuration(2 * kSolenoidDelayTime);
  solenoid1.StartPulse();
  solenoid2.StartPulse();
  frc::Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn on";
  EXPECT_FALSE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn on";
  EXPECT_TRUE(solenoid1.Get()) << "Solenoid #1 did not read on";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";
  frc::Wait(2 * kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Pulse both Solenoids on with different durations
  solenoid1.SetPulseDuration(1.5 * kSolenoidDelayTime);
  solenoid2.SetPulseDuration(2.5 * kSolenoidDelayTime);
  solenoid1.StartPulse();
  solenoid2.StartPulse();
  frc::Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn on";
  EXPECT_FALSE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn on";
  EXPECT_TRUE(solenoid1.Get()) << "Solenoid #1 did not read on";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";
  frc::Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_FALSE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn on";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";
  frc::Wait(2 * kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1.Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2.Get()) << "Solenoid #2 did not turn off";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";
}
