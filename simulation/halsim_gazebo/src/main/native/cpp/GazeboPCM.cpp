// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "GazeboPCM.h"

#include <fmt/format.h>
#include <hal/Value.h>
#include <hal/simulation/CTREPCMData.h>
#include <hal/simulation/NotifyListener.h>

#include "simulation/gz_msgs/msgs.h"

static void init_callback(const char* name, void* param,
                          const struct HAL_Value* value) {
  GazeboPCM* pcm = static_cast<GazeboPCM*>(param);
  pcm->SetInitialized(value->data.v_boolean);
}

static void output_callback(const char* name, void* param,
                            const struct HAL_Value* value) {
  GazeboPCM* pcm = static_cast<GazeboPCM*>(param);
  if (pcm->IsInitialized())
    pcm->Publish(value->data.v_boolean);
}

GazeboPCM::GazeboPCM(int index, int channel, HALSimGazebo* halsim) {
  m_index = index;
  m_channel = channel;
  m_halsim = halsim;
  m_pub = NULL;
  HALSIM_RegisterCTREPCMInitializedCallback(index, init_callback, this, true);
  HALSIM_RegisterCTREPCMSolenoidOutputCallback(index, channel, output_callback,
                                               this, true);
}

void GazeboPCM::Publish(bool value) {
  if (!m_pub) {
    m_pub = m_halsim->node.Advertise<gazebo::msgs::Bool>(
        fmt::format("~/simulator/pneumatic/{}/{}", m_index + 1, m_channel));
    m_pub->WaitForConnection(gazebo::common::Time(1, 0));
  }
  gazebo::msgs::Bool msg;
  msg.set_data(value);
  if (m_pub)
    m_pub->Publish(msg);
}

void GazeboPCM_SetPressureSwitch(int index, bool value) {
  HALSIM_SetCTREPCMPressureSwitch(index, value);
}
