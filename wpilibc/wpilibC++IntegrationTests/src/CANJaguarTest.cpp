/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

class CANJaguarTest : public testing::Test {
protected:
    CANJaguar *m_jaguar;

    virtual void SetUp() {
        Wait(0.04);
        m_jaguar = new CANJaguar(TestBench::kCANJaguarID);
    }

    virtual void TearDown() {
        delete m_jaguar;
    }

    void Reset() {
        m_jaguar->ChangeControlMode(CANJaguar::kPercentVbus);
        m_jaguar->Set(0.0f);
    }
};

#if 0
TEST_F(CANJaguarTest, Speed) {
    Reset();

    for(;;) {
        m_jaguar->Set(1.0f);

        std::cout << "Temperature = " << m_jaguar->GetTemperature() << std::endl;
        std::cout << "Current = " << m_jaguar->GetOutputCurrent() << std::endl;

        Wait(0.1);
    }
}
#endif
