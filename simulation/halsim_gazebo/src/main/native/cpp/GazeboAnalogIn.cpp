// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
