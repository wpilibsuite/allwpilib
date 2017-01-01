/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "servo.h"

#include <boost/algorithm/string/replace.hpp>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>

#ifdef _WIN32
// Ensure that Winsock2.h is included before Windows.h, which can get
// pulled in by anybody (e.g., Boost).
#include <Winsock2.h>
#endif

GZ_REGISTER_MODEL_PLUGIN(Servo)

void Servo::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;
  signal = 0;

  // parse SDF Properries
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/" + sdf->GetAttribute("name")->GetAsString();
  }

  if (sdf->HasElement("torque")) {
    torque = sdf->Get<double>("torque");
  } else {
    torque = 5;
  }

  gzmsg << "initializing servo: " << topic << " joint=" << joint->GetName()
        << " torque=" << torque << std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name =
      model->GetWorld()->GetName() + "::" + model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  node->Init(scoped_name);
  sub = node->Subscribe(topic, &Servo::Callback, this);

  // connect to the world update event
  // this will call update every iteration
  updateConn = gazebo::event::Events::ConnectWorldUpdateBegin(
      boost::bind(&Servo::Update, this, _1));
}

void Servo::Update(const gazebo::common::UpdateInfo& info) {
  // torque is in kg*cm
  // joint->SetAngle(0,signal*180);
  if (joint->GetAngle(0) < signal) {
    joint->SetForce(0, torque);
  } else if (joint->GetAngle(0) > signal) {
    joint->SetForce(0, torque);
  }
  joint->SetForce(0, 0);
}

void Servo::Callback(const gazebo::msgs::ConstFloat64Ptr& msg) {
  signal = msg->data();
  if (signal < -1) {
    signal = -1;
  } else if (signal > 1) {
    signal = 1;
  }
}
