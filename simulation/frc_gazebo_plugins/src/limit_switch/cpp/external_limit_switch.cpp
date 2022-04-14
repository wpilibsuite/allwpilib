// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
