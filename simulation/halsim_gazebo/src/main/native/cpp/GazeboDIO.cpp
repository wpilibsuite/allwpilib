/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GazeboDIO.h"

#include <string>

#include <mockdata/DIOData.h>
#include <mockdata/HAL_Value.h>
#include <mockdata/NotifyListener.h>
#pragma warning(disable: 4244 4267 4146)

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
        "~/simulator/dio/" + std::to_string(m_index), &GazeboDIO::Callback,
        this);
}

void GazeboDIO::Callback(const gazebo::msgs::ConstBoolPtr& msg) {
  HALSIM_SetDIOValue(m_index, msg->data());
}
