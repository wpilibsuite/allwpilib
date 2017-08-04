/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInput.h"
#include "Compressor.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "DoubleSolenoid.h"
#include "Solenoid.h"
#include "TestBench.h"
#include "Timer.h"
#include "gtest/gtest.h"

using namespace frc;

/* The PCM switches the compressor up to a couple seconds after the pressure
        switch changes. */
static const double kCompressorDelayTime = 3.0;

/* Solenoids should change much more quickly */
static const double kSolenoidDelayTime = 0.5;

/* The voltage divider on the test bench should bring the compressor output
        to around these values. */
static const double kCompressorOnVoltage = 5.00;
static const double kCompressorOffVoltage = 1.68;

class PCMTest : public testing::Test {
 protected:
  Compressor* m_compressor;

  DigitalOutput* m_fakePressureSwitch;
  AnalogInput* m_fakeCompressor;
  DoubleSolenoid* m_doubleSolenoid;
  DigitalInput *m_fakeSolenoid1, *m_fakeSolenoid2;

  void SetUp() override {
    m_compressor = new Compressor();

    m_fakePressureSwitch =
        new DigitalOutput(TestBench::kFakePressureSwitchChannel);
    m_fakeCompressor = new AnalogInput(TestBench::kFakeCompressorChannel);
    m_fakeSolenoid1 = new DigitalInput(TestBench::kFakeSolenoid1Channel);
    m_fakeSolenoid2 = new DigitalInput(TestBench::kFakeSolenoid2Channel);
  }

  void TearDown() override {
    delete m_compressor;
    delete m_fakePressureSwitch;
    delete m_fakeCompressor;
    delete m_fakeSolenoid1;
    delete m_fakeSolenoid2;
  }

  void Reset() {
    m_compressor->Stop();
    m_fakePressureSwitch->Set(false);
  }
};

/**
 * Test if the compressor turns on and off when the pressure switch is toggled
 */
TEST_F(PCMTest, PressureSwitch) {
  Reset();

  m_compressor->SetClosedLoopControl(true);

  // Turn on the compressor
  m_fakePressureSwitch->Set(true);
  Wait(kCompressorDelayTime);
  EXPECT_NEAR(kCompressorOnVoltage, m_fakeCompressor->GetVoltage(), 0.5)
      << "Compressor did not turn on when the pressure switch turned on.";

  // Turn off the compressor
  m_fakePressureSwitch->Set(false);
  Wait(kCompressorDelayTime);
  EXPECT_NEAR(kCompressorOffVoltage, m_fakeCompressor->GetVoltage(), 0.5)
      << "Compressor did not turn off when the pressure switch turned off.";
}

/**
 * Test if the correct solenoids turn on and off when they should
 */
TEST_F(PCMTest, Solenoid) {
  Reset();
  Solenoid solenoid1(TestBench::kSolenoidChannel1);
  Solenoid solenoid2(TestBench::kSolenoidChannel2);

  // Turn both solenoids off
  solenoid1.Set(false);
  solenoid2.Set(false);
  Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn off";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Turn one solenoid on and one off
  solenoid1.Set(true);
  solenoid2.Set(false);
  Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn on";
  EXPECT_TRUE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn off";
  EXPECT_TRUE(solenoid1.Get()) << "Solenoid #1 did not read on";
  EXPECT_FALSE(solenoid2.Get()) << "Solenoid #2 did not read off";

  // Turn one solenoid on and one off
  solenoid1.Set(false);
  solenoid2.Set(true);
  Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn off";
  EXPECT_FALSE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn on";
  EXPECT_FALSE(solenoid1.Get()) << "Solenoid #1 did not read off";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";

  // Turn both on
  solenoid1.Set(true);
  solenoid2.Set(true);
  Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn on";
  EXPECT_FALSE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn on";
  EXPECT_TRUE(solenoid1.Get()) << "Solenoid #1 did not read on";
  EXPECT_TRUE(solenoid2.Get()) << "Solenoid #2 did not read on";
}

/**
 * Test if the correct solenoids turn on and off when they should when used
 * with the DoubleSolenoid class.
 */
TEST_F(PCMTest, DoubleSolenoid) {
  DoubleSolenoid solenoid(TestBench::kSolenoidChannel1,
                          TestBench::kSolenoidChannel2);

  solenoid.Set(DoubleSolenoid::kOff);
  Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn off";
  EXPECT_TRUE(solenoid.Get() == DoubleSolenoid::kOff)
      << "Solenoid does not read off";

  solenoid.Set(DoubleSolenoid::kForward);
  Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn on";
  EXPECT_TRUE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn off";
  EXPECT_TRUE(solenoid.Get() == DoubleSolenoid::kForward)
      << "Solenoid does not read forward";

  solenoid.Set(DoubleSolenoid::kReverse);
  Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn off";
  EXPECT_FALSE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn on";
  EXPECT_TRUE(solenoid.Get() == DoubleSolenoid::kReverse)
      << "Solenoid does not read reverse";
}
