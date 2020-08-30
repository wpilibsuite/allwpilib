/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GazeboAnalogIn.h"

#include <string>

#include <hal/Power.h>
#include <hal/Value.h>
#include <hal/simulation/AnalogInData.h>
#include <hal/simulation/NotifyListener.h>

static void init_callback(const char* name, void* param,
                          const struct HAL_Value* value) {
  GazeboAnalogIn* ain = static_cast<GazeboAnalogIn*>(param);
  ain->SetInitialized(value->data.v_boolean);
  if (ain->IsInitialized()) {
    ain->Listen();
  }
}

GazeboAnalogIn::GazeboAnalogIn(int index, HALSimGazebo* halsim) {
  m_index = index;
  m_halsim = halsim;
  m_sub = NULL;
  HALSIM_RegisterAnalogInInitializedCallback(index, init_callback, this, true);
}

void GazeboAnalogIn::Listen() {
  if (!m_sub)
    m_sub = m_halsim->node.Subscribe<gazebo::msgs::Float64>(
        "~/simulator/analog/" + std::to_string(m_index),
        &GazeboAnalogIn::Callback, this);
}

void GazeboAnalogIn::Callback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  /* This value is going to be divided by the 5V rail in the HAL, so
     we multiply by that value to make the change neutral */
  int32_t status;
  HALSIM_SetAnalogInVoltage(m_index,
                            msg->data() * HAL_GetUserVoltage5V(&status));
}
