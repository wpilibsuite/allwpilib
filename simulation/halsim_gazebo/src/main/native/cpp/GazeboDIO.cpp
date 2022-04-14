// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "GazeboDIO.h"

#include <fmt/format.h>
#include <hal/Value.h>
#include <hal/simulation/DIOData.h>
#include <hal/simulation/NotifyListener.h>

static void init_callback(const char* name, void* param,
                          const struct HAL_Value* value) {
  GazeboDIO* dio = static_cast<GazeboDIO*>(param);
  dio->SetInitialized(value->data.v_boolean);
  if (dio->IsInitialized()) {
    dio->Listen();
  }
}

GazeboDIO::GazeboDIO(int index, HALSimGazebo* halsim) {
  m_index = index;
  m_halsim = halsim;
  m_sub = NULL;
  HALSIM_RegisterDIOInitializedCallback(index, init_callback, this, true);
}

void GazeboDIO::Listen() {
  if (!m_sub)
    m_sub = m_halsim->node.Subscribe<gazebo::msgs::Bool>(
        fmt::format("~/simulator/dio/{}", m_index), &GazeboDIO::Callback, this);
}

void GazeboDIO::Callback(const gazebo::msgs::ConstBoolPtr& msg) {
  HALSIM_SetDIOValue(m_index, msg->data());
}
