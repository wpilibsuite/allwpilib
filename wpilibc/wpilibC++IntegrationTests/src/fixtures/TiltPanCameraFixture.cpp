/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "fixtures/TiltPanCameraFixture.h"

TiltPanCameraFixture::TiltPanCameraFixture(Servo *tilt, Servo *pan, Gyro *gyro):
	m_tilt(tilt),
	m_pan(pan),
	m_gyro(gyro) {
}

bool TiltPanCameraFixture::SetUp() {
	return Reset();
}

bool TiltPanCameraFixture::Reset() {
	m_tilt->Set(0.0);
	m_pan->Set(0.0);
	
	Wait(RESET_TIME);
	
	m_gyro->Reset();
	
	return m_pan->Get() == 0.0 and m_tilt->Get() == 0.0 and m_gyro->GetAngle() == 0.0;
}

bool TiltPanCameraFixture::TearDown() {
	return Reset();
}

Gyro *TiltPanCameraFixture::GetGyro() {
	return m_gyro;
}

Servo *TiltPanCameraFixture::GetTilt() {
	return m_tilt;
}

Servo *TiltPanCameraFixture::GetPan() {
	return m_pan;
}

