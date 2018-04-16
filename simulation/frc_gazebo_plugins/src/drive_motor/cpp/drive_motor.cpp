/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "drive_motor.h"

#include <boost/algorithm/string/replace.hpp>

GZ_REGISTER_MODEL_PLUGIN(DriveMotor)

void DriveMotor::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;
  signal = 0;

  // Parse SDF properties
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  if (!joint) {
    gzerr << "Error initializing drive motor: could not get joint";
    return;
  }

  parent = joint->GetParent();
  if (!parent) {
    gzerr << "Error initializing drive motor: could not get parent";
    return;
  }

  child = joint->GetChild();
  if (!child) {
    gzerr << "Error initializing drive motor: could not get child";
    return;
  }

  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/" + sdf->GetAttribute("name")->GetAsString();
  }

  if (sdf->HasElement("max_speed")) {
    maxSpeed = sdf->Get<double>("max_speed");
  } else {
    maxSpeed = 0;
  }

  if (sdf->HasElement("multiplier")) {
    multiplier = sdf->Get<double>("multiplier");
  } else {
    multiplier = 1;
  }

  if (sdf->HasElement("dx")) {
    dx = sdf->Get<double>("dx");
  } else {
    dx = 0;
  }

  if (sdf->HasElement("dy")) {
    dy = sdf->Get<double>("dy");
  } else {
    dy = 0;
  }

  if (sdf->HasElement("dz")) {
    dz = sdf->Get<double>("dz");
  } else {
    dz = 0;
  }

  gzmsg << "Initializing drive motor: " << topic << " parent=" << parent->GetName()
        << " directions=" << dx << " " << dy << " " << dz
        << " multiplier=" << multiplier <<  std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name =
      model->GetWorld()->GetName() + "::" + model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  node->Init(scoped_name);
  sub = node->Subscribe(topic, &DriveMotor::Callback, this);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = gazebo::event::Events::ConnectWorldUpdateBegin(
      boost::bind(&DriveMotor::Update, this, _1));
}

static double computeForce(double input, double velocity, double max) {
  double output = input;
  if (max == 0.0) return output;
  if (std::fabs(velocity) >= max)
    output = 0;
  else {
    double reduce = (max - std::fabs(velocity)) / max;
    output *= reduce;
  }
  return output;
}

void DriveMotor::Update(const gazebo::common::UpdateInfo& info) {
#if GAZEBO_MAJOR_VERSION >= 8
  ignition::math::Vector3d velocity = parent->RelativeLinearVel();
#else
  ignition::math::Vector3d velocity = parent->GetRelativeLinearVel().Ign();
#endif

  if (signal == 0)
    return;

  double x = computeForce(signal * dx * multiplier, velocity.X(),
                          std::fabs(maxSpeed * dx));
  double y = computeForce(signal * dy * multiplier, velocity.Y(),
                          std::fabs(maxSpeed * dy));
  double z = computeForce(signal * dz * multiplier, velocity.Z(),
                          std::fabs(maxSpeed * dz));

  ignition::math::Vector3d force(x, y, z);
#if GAZEBO_MAJOR_VERSION >= 8
  parent->AddLinkForce(force, child->RelativePose().Pos());
#else
  parent->AddLinkForce(force, child->GetRelativePose().Ign().Pos());
#endif
}

void DriveMotor::Callback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  signal = msg->data();
  if (signal < -1) {
    signal = -1;
  } else if (signal > 1) {
    signal = 1;
  }
}
