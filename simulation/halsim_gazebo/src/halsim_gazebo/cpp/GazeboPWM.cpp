/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GazeboPWM.h"

#include <iostream>
#include <string>

#include "MockData/HAL_Value.h"
#include "MockData/NotifyListener.h"
#include "MockData/PWMData.h"
#include "simulation/gz_msgs/msgs.h"

static void pwm_callback(const char* name, void* param,
                         const struct HAL_Value* value) {
  GazeboPWM* pwm = static_cast<GazeboPWM*>(param);
  pwm->Publish(value->data.v_double);
}

GazeboPWM::GazeboPWM(int port, HALSimGazebo* halsim) {
  m_port = port;
  m_halsim = halsim;
  m_pub = halsim->node.Advertise<gazebo::msgs::Float64>("~/simulator/pwm/" +
                                                        std::to_string(port));
  HALSIM_RegisterPWMSpeedCallback(port, pwm_callback, this, false);
}

void GazeboPWM::Publish(double value) {
  if (!m_pub)
    m_pub = m_halsim->node.Advertise<gazebo::msgs::Bool>(
        "~/simulator/pwm/" + std::to_string(m_port));
  gazebo::msgs::Float64 msg;
  msg.set_data(value);
  if (m_pub) m_pub->Publish(msg);
}
