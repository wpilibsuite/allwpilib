#pragma once

enum AccelerometerRange {
	kRange_2G = 0,
	kRange_4G = 1,
	kRange_8G = 2,
};

extern "C" {
	void setAccelerometerActive(bool);
	void setAccelerometerRange(AccelerometerRange);
	double getAccelerometerX();
	double getAccelerometerY();
	double getAccelerometerZ();
}
