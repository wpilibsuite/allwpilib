/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "pneumatic_piston.h"

#include <boost/algorithm/string/replace.hpp>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>

#ifdef _WIN32
// Ensure that Winsock2.h is included before Windows.h, which can get
// pulled in by anybody (e.g., Boost).
#include <Winsock2.h>
#endif

GZ_REGISTER_MODEL_PLUGIN(PneumaticPiston)

void PneumaticPiston::Load(gazebo::physics::ModelPtr model,
                           sdf::ElementPtr sdf) {
  this->model = model;
  forward_signal = reverse_signal = false;

  // Parse SDF properties
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/" + sdf->GetAttribute("name")->GetAsString();
  }

  if (sdf->HasElement("reverse-topic")) {
    reverse_topic = sdf->Get<std::string>("reverse-topic");
  } else {
    reverse_topic.clear();
  }

  forward_force = sdf->Get<double>("forward-force");
  if (sdf->HasElement("reverse-force"))
    reverse_force = -1.0 * sdf->Get<double>("reverse-force");

  if (sdf->HasElement("direction") &&
      sdf->Get<std::string>("direction") == "reversed") {
    forward_force = -forward_force;
    reverse_force = -reverse_force;
  }

  gzmsg << "Initializing piston: " << topic << " joint=" << joint->GetName()
        << " forward_force=" << forward_force
        << " reverse_force=" << reverse_force << std::endl;
  if (!reverse_topic.empty())
    gzmsg << "Reversing on topic " << reverse_topic << std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name =
      model->GetWorld()->GetName() + "::" + model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  node->Init(scoped_name);
  sub = node->Subscribe(topic, &PneumaticPiston::ForwardCallback, this);
  if (!reverse_topic.empty())
    sub_reverse =
        node->Subscribe(reverse_topic, &PneumaticPiston::ReverseCallback, this);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = gazebo::event::Events::ConnectWorldUpdateBegin(
      boost::bind(&PneumaticPiston::Update, this, _1));
}

void PneumaticPiston::Update(const gazebo::common::UpdateInfo& info) {
  double force = 0.0;
  if (forward_signal) {
    force = forward_force;
  }
  else {
    /* For DoubleSolenoids, the second signal must be present
       for us to apply the reverse force.  For SingleSolenoids,
       the lack of the forward signal suffices.
       Note that a true simulation would not allow a SingleSolenoid to
       have reverse force, but we put that in the hands of the model builder.*/
    if (reverse_topic.empty() || reverse_signal) force = reverse_force;
  }
  joint->SetForce(0, force);
}

void PneumaticPiston::ForwardCallback(const gazebo::msgs::ConstBoolPtr& msg) {
  forward_signal = msg->data();
}

void PneumaticPiston::ReverseCallback(const gazebo::msgs::ConstBoolPtr& msg) {
  reverse_signal = msg->data();
}
