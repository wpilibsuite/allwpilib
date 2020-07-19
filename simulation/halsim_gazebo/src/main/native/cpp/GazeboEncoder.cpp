/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GazeboEncoder.h"

#include <string>

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
  if (encoder->IsInitialized()) encoder->SetReverse(value->data.v_boolean);
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
        "~/simulator/encoder/dio/" +
        std::to_string(HALSIM_GetEncoderDigitalChannelA(m_index)) + "/control");
    m_pub->WaitForConnection(gazebo::common::Time(1, 0));
  }
  gazebo::msgs::String msg;
  msg.set_data(command);
  if (m_pub) m_pub->Publish(msg);
}

void GazeboEncoder::Listen() {
  if (!m_sub)
    m_sub = m_halsim->node.Subscribe<gazebo::msgs::Float64>(
        "~/simulator/encoder/dio/" +
            std::to_string(HALSIM_GetEncoderDigitalChannelA(m_index)) +
            "/position",
        &GazeboEncoder::Callback, this);
}

void GazeboEncoder::Callback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  HALSIM_SetEncoderCount(m_index, msg->data() * (m_reverse ? -1 : 1));
}
