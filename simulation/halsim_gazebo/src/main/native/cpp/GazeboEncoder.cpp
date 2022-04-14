// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "GazeboEncoder.h"

#include <fmt/format.h>
#include <hal/Value.h>
#include <hal/simulation/EncoderData.h>
#include <hal/simulation/NotifyListener.h>

static void encoder_init_callback(const char* name, void* param,
                                  const struct HAL_Value* value) {
  GazeboEncoder* encoder = static_cast<GazeboEncoder*>(param);
  encoder->SetInitialized(value->data.v_boolean);
  if (encoder->IsInitialized()) {
    encoder->Control("start");
    encoder->Listen();
  }
}

static void encoder_reset_callback(const char* name, void* param,
                                   const struct HAL_Value* value) {
  GazeboEncoder* encoder = static_cast<GazeboEncoder*>(param);
  if (encoder->IsInitialized() && value->data.v_boolean)
    encoder->Control("reset");
}

static void encoder_reverse_callback(const char* name, void* param,
                                     const struct HAL_Value* value) {
  GazeboEncoder* encoder = static_cast<GazeboEncoder*>(param);
  if (encoder->IsInitialized())
    encoder->SetReverse(value->data.v_boolean);
}

GazeboEncoder::GazeboEncoder(int index, HALSimGazebo* halsim) {
  m_index = index;
  m_halsim = halsim;
  m_reverse = false;
  m_pub = NULL;
  m_sub = NULL;
  HALSIM_RegisterEncoderInitializedCallback(index, encoder_init_callback, this,
                                            true);
  HALSIM_RegisterEncoderResetCallback(index, encoder_reset_callback, this,
                                      true);
  HALSIM_RegisterEncoderReverseDirectionCallback(
      index, encoder_reverse_callback, this, true);
}

void GazeboEncoder::Control(const char* command) {
  if (!m_pub) {
    m_pub = m_halsim->node.Advertise<gazebo::msgs::String>(
        fmt::format("~/simulator/encoder/dio/{}/control",
                    HALSIM_GetEncoderDigitalChannelA(m_index)));
    m_pub->WaitForConnection(gazebo::common::Time(1, 0));
  }
  gazebo::msgs::String msg;
  msg.set_data(command);
  if (m_pub)
    m_pub->Publish(msg);
}

void GazeboEncoder::Listen() {
  if (!m_sub)
    m_sub = m_halsim->node.Subscribe<gazebo::msgs::Float64>(
        fmt::format("~/simulator/encoder/dio/{}/position",
                    HALSIM_GetEncoderDigitalChannelA(m_index)),
        &GazeboEncoder::Callback, this);
}

void GazeboEncoder::Callback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  HALSIM_SetEncoderCount(m_index, msg->data() * (m_reverse ? -1 : 1));
}
