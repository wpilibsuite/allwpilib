/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "external_limit_switch.h"

#include <string>

ExternalLimitSwitch::ExternalLimitSwitch(sdf::ElementPtr sdf) {
  sensor = std::dynamic_pointer_cast<gazebo::sensors::ContactSensor>(
      gazebo::sensors::get_sensor(sdf->Get<std::string>("sensor")));

  gzmsg << "\texternal limit switch: "
        << " sensor=" << sensor->Name() << std::endl;
}

bool ExternalLimitSwitch::Get() {
  return sensor->Contacts().contact().size() > 0;
}
