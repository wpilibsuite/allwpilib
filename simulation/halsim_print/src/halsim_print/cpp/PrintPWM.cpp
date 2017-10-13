/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PrintPWM.h"

#include <iostream>

#include "MockData/HAL_Value.h"
#include "MockData/NotifyListener.h"
#include "MockData/PWMData.h"

static void pwm_callback(const char* name, void* param,
                         const struct HAL_Value* value) {
  PrintPWM* pwm = static_cast<PrintPWM*>(param);
  pwm->Publish(value->data.v_double);
}

PrintPWM::PrintPWM(int port) {
  m_port = port;
  HALSIM_RegisterPWMSpeedCallback(port, pwm_callback, this, false);
}

void PrintPWM::Publish(double value) {
  std::cout << "PWM " << m_port << ": " << value << std::endl;
}
