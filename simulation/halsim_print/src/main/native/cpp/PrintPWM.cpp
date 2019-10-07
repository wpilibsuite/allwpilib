/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PrintPWM.h"

#include <iostream>

#include <hal/Value.h>
#include <mockdata/NotifyListener.h>
#include <mockdata/PWMData.h>

static void PWMCallback(const char* name, void* param,
                        const struct HAL_Value* value) {
  auto pwm = static_cast<PrintPWM*>(param);
  pwm->Publish(value->data.v_double);
}

PrintPWM::PrintPWM(int port) {
  m_port = port;
  HALSIM_RegisterPWMSpeedCallback(port, PWMCallback, this, false);
}

void PrintPWM::Publish(double value) {
  std::cout << "PWM " << m_port << ": " << value << std::endl;
}
