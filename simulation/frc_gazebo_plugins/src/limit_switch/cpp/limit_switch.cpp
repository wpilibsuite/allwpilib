/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "limit_switch.h"

#include <boost/algorithm/string/replace.hpp>

GZ_REGISTER_MODEL_PLUGIN(LimitSwitch)

void LimitSwitch::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;

  // Parse SDF properties
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/" + sdf->GetAttribute("name")->GetAsString();
  }
  invert = sdf->HasElement("invert");
  std::string type = sdf->Get<std::string>("type");

  gzmsg << "Initializing limit switch: " << topic << " type=" << type
        << std::endl;
  if (type == "internal") {
    ls = new InternalLimitSwitch(model, sdf);
  } else if (type == "external") {
    ls = new ExternalLimitSwitch(sdf);
  } else {
    gzerr << "WARNING: unsupported limit switch type " << type;
  }

  // Connect to Gazebo transport for messaging
  std::string scoped_name =
      model->GetWorld()->GetName() + "::" + model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  node->Init(scoped_name);
  pub = node->Advertise<gazebo::msgs::Bool>(topic);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = gazebo::event::Events::ConnectWorldUpdateBegin(
      boost::bind(&LimitSwitch::Update, this, _1));
}

void LimitSwitch::Update(const gazebo::common::UpdateInfo& info) {
  gazebo::msgs::Bool msg;
  if (invert)
    msg.set_data(!ls->Get());
  else
    msg.set_data(ls->Get());
  pub->Publish(msg);
}
