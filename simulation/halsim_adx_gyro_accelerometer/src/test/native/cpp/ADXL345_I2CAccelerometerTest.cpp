
#include "ADXL345_I2C.h"
#include "ADXL345_I2CAccelerometerData.h"
#include "Talon.h"
#include "gtest/gtest.h"

class ADXL345_I2CAccelerometerTest : public ::testing::TestWithParam<frc::I2C::Port> {

};

TEST_P(ADXL345_I2CAccelerometerTest, TestAccelerometer) {

	const double EPSILON = 1 / 256.0;

	frc::I2C::Port port = GetParam();

	hal::ADXL345_I2CData sim{port};
	frc::ADXL345_I2C accel{port};

	EXPECT_NEAR(0, sim.GetX(), EPSILON);
	EXPECT_NEAR(0, sim.GetY(), EPSILON);
	EXPECT_NEAR(0, sim.GetZ(), EPSILON);

	EXPECT_NEAR(0, accel.GetX(), EPSILON);
	EXPECT_NEAR(0, accel.GetY(), EPSILON);
	EXPECT_NEAR(0, accel.GetZ(), EPSILON);

	sim.SetX(1.56);
	sim.SetY(-.653);
	sim.SetZ(0.11);

	EXPECT_NEAR(1.56, sim.GetX(), EPSILON);
	EXPECT_NEAR(-.653, sim.GetY(), EPSILON);
	EXPECT_NEAR(0.11, sim.GetZ(), EPSILON);

	EXPECT_NEAR(1.56, accel.GetX(), EPSILON);
	EXPECT_NEAR(-.653, accel.GetY(), EPSILON);
	EXPECT_NEAR(0.11, accel.GetZ(), EPSILON);
}

INSTANTIATE_TEST_CASE_P(ADXL345_I2CAccelerometerTest,
		ADXL345_I2CAccelerometerTest,
                        ::testing::Values(frc::I2C::kOnboard, frc::I2C::kMXP));
