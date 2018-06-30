/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "internal_limit_switch.h"

#include <string>

InternalLimitSwitch::InternalLimitSwitch(gazebo::physics::ModelPtr model,
                                         sdf::ElementPtr sdf) {
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  min = sdf->Get<double>("min");
  max = sdf->Get<double>("max");

  if (sdf->HasElement("units")) {
    radians = sdf->Get<std::string>("units") != "degrees";
  } else {
    radians = true;
  }

  gzmsg << "\tinternal limit switch: "
        << " type=" << joint->GetName() << " range=[" << min << "," << max
        << "] radians=" << radians << std::endl;
}

bool InternalLimitSwitch::Get() {
  double value;
  joint->GetAngle(0).Normalize();
  if (radians) {
    value = joint->GetAngle(0).Radian();
  } else {
    value = joint->GetAngle(0).Degree();
  }
  return value >= min && value <= max;
}
