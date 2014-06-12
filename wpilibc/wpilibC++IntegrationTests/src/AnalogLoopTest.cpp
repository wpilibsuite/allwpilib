/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

static const double kDelayTime = 0.05;

/**
 * A fixture with an analog input and an analog output wired together
 */
class AnalogLoopTest : public testing::Test {
protected:
    AnalogInput *m_input;
    AnalogOutput *m_output;

    virtual void SetUp() {
        m_input = new AnalogInput(TestBench::kFakeAnalogOutputChannel);
        m_output = new AnalogOutput(TestBench::kAnalogOutputChannel);
    }

    virtual void TearDown() {
        delete m_input;
        delete m_output;
    }

    void Reset() {
        m_output->SetVoltage(0.0f);
    }
};

/**
 * Test analog inputs and outputs by setting one and making sure the other
 * matches.
 */
TEST_F(AnalogLoopTest, Loop) {
    Reset();

    // Set the output voltage and check if the input measures the same voltage
    for(int i = 0; i < 50; i++) {
        m_output->SetVoltage(i / 10.0f);

        Wait(kDelayTime);

        EXPECT_NEAR(m_output->GetVoltage(), m_input->GetVoltage(), 0.01f);
    }
}
