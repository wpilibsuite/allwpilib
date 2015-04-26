#include "internal_limit_switch.h"

InternalLimitSwitch::InternalLimitSwitch(physics::ModelPtr model, sdf::ElementPtr sdf) {
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  min = sdf->Get<double>("min");
  max = sdf->Get<double>("max");

  if (sdf->HasElement("units")) {
    radians = sdf->Get<std::string>("units") != "degrees";
  } else {
    radians = true;
  }

  gzmsg << "\tinternal limit switch: " << " type=" << joint->GetName()
        << " range=[" << min << "," << max << "] radians=" << radians << std::endl;
}

InternalLimitSwitch::~InternalLimitSwitch() {}

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
