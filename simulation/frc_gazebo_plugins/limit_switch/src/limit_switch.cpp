/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "limit_switch.h"

GZ_REGISTER_MODEL_PLUGIN(LimitSwitch)

LimitSwitch::LimitSwitch() {}

LimitSwitch::~LimitSwitch() {}

void LimitSwitch::Load(physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;

  // Parse SDF properties
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/"+sdf->GetAttribute("name")->GetAsString();
  }
  std::string type = sdf->Get<std::string>("type");

  gzmsg << "Initializing limit switch: " << topic << " type=" << type << std::endl;
  if (type == "internal") {
    ls = new InternalLimitSwitch(model, sdf);
  } else if (type == "external") {
    ls = new ExternalLimitSwitch(sdf);
  } else {
    gzerr << "WARNING: unsupported limit switch type " << type;
  }

  // Connect to Gazebo transport for messaging
  std::string scoped_name = model->GetWorld()->GetName()+"::"+model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = transport::NodePtr(new transport::Node());
  node->Init(scoped_name);
  pub = node->Advertise<msgs::Bool>(topic);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = event::Events::ConnectWorldUpdateBegin(boost::bind(&LimitSwitch::Update, this, _1));
}

void LimitSwitch::Update(const common::UpdateInfo &info) {
  msgs::Bool msg;
  msg.set_data(ls->Get());
  pub->Publish(msg);
}
