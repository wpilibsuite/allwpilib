/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

/* The PCM switches the compressor up to 2 seconds after the pressure switch
  changes. */
static const double kCompressorDelayTime = 2.0;

/* Solenoids should change much more quickly */
static const double kSolenoidDelayTime = 0.1;

/* The voltage divider on the test bench should bring the compressor output
  to around these values. */
static const double kCompressorOnVoltage = 5.00;
static const double kCompressorOffVoltage = 1.68;

class PCMTest : public testing::Test {
protected:
  Compressor *m_compressor;

  DigitalOutput *m_fakePressureSwitch;
  AnalogInput *m_fakeCompressor;

  Solenoid *m_solenoid1, *m_solenoid2;
  DigitalInput *m_fakeSolenoid1, *m_fakeSolenoid2;

  virtual void SetUp() {
    m_compressor = new Compressor();

    m_fakePressureSwitch = new DigitalOutput(TestBench::kFakePressureSwitchChannel);
    m_fakeCompressor = new AnalogInput(TestBench::kFakeCompressorChannel);

    m_solenoid1 = new Solenoid(7);
    m_solenoid2 = new Solenoid(6);

    m_fakeSolenoid1 = new DigitalInput(TestBench::kFakeSolenoid1Channel);
    m_fakeSolenoid2 = new DigitalInput(TestBench::kFakeSolenoid2Channel);
  }

  virtual void TearDown() {
    delete m_compressor;
    delete m_fakePressureSwitch;
    delete m_fakeCompressor;
    delete m_solenoid1;
    delete m_solenoid2;
    delete m_fakeSolenoid1;
    delete m_fakeSolenoid2;
  }

  void Reset() {
    m_compressor->Stop();
    m_fakePressureSwitch->Set(false);
    m_solenoid1->Set(false);
    m_solenoid2->Set(false);
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
  EXPECT_NEAR(kCompressorOnVoltage, m_fakeCompressor->GetVoltage(), 0.1)
    << "Compressor did not turn on when the pressure switch turned on.";

  // Turn off the compressor
  m_fakePressureSwitch->Set(false);
  Wait(kCompressorDelayTime);
  EXPECT_NEAR(kCompressorOffVoltage, m_fakeCompressor->GetVoltage(), 0.1)
    << "Compressor did not turn off when the pressure switch turned off.";
}

/**
 * Test if the correct solenoids turn on and off when they should
 */
TEST_F(PCMTest, Solenoid) {
  Reset();

  // Turn both solenoids off
  m_solenoid1->Set(false);
  m_solenoid2->Set(false);
  Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn off";
  EXPECT_TRUE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn off";

  // Turn one solenoid on and one off
  m_solenoid1->Set(true);
  m_solenoid2->Set(false);
  Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn on";
  EXPECT_TRUE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn off";

  // Turn one solenoid on and one off
  m_solenoid1->Set(false);
  m_solenoid2->Set(true);
  Wait(kSolenoidDelayTime);
  EXPECT_TRUE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn off";
  EXPECT_FALSE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn on";

  // Turn both on
  m_solenoid1->Set(true);
  m_solenoid2->Set(true);
  Wait(kSolenoidDelayTime);
  EXPECT_FALSE(m_fakeSolenoid1->Get()) << "Solenoid #1 did not turn on";
  EXPECT_FALSE(m_fakeSolenoid2->Get()) << "Solenoid #2 did not turn on";
}
