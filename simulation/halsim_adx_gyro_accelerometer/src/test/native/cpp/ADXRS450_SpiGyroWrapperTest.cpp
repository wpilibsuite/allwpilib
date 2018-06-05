
#include "ADXRS450_Gyro.h"
#include "ADXRS450_SpiGyroWrapperData.h"
#include "Talon.h"
#include "gtest/gtest.h"

class ADXRS450_SpiGyroWrapperTest : public ::testing::TestWithParam<frc::SPI::Port> {

};

TEST_P(ADXRS450_SpiGyroWrapperTest, TestAccelerometer) {

	const double EPSILON = .000001;

	frc::SPI::Port port = GetParam();

	hal::ADXRS450_SpiGyroWrapper sim{port};
	frc::ADXRS450_Gyro gyro{port};

	EXPECT_NEAR(0, sim.GetAngle(), EPSILON);
	EXPECT_NEAR(0, gyro.GetAngle(), EPSILON);

	sim.SetAngle(32.56);
	EXPECT_NEAR(32.56, sim.GetAngle(), EPSILON);
	EXPECT_NEAR(32.56, gyro.GetAngle(), EPSILON);
}


INSTANTIATE_TEST_CASE_P(ADXRS450_SpiGyroWrapperTest,
		ADXRS450_SpiGyroWrapperTest,
                        ::testing::Values(
                        		frc::SPI::kOnboardCS0,
                        		frc::SPI::kOnboardCS1,
                        		frc::SPI::kOnboardCS2,
                        		frc::SPI::kOnboardCS3,
								frc::SPI::kMXP));
