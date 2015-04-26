#ifdef _WIN32
  // Ensure that Winsock2.h is included before Windows.h, which can get
  // pulled in by anybody (e.g., Boost).
  #include <Winsock2.h>
#endif

#include "pneumatic_piston.h"

#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>

GZ_REGISTER_MODEL_PLUGIN(PneumaticPiston)

PneumaticPiston::PneumaticPiston() {}

PneumaticPiston::~PneumaticPiston() {}

void PneumaticPiston::Load(physics::ModelPtr model, sdf::ElementPtr sdf) {
  this->model = model;
  signal = 0;

  // Parse SDF properties
  joint = model->GetJoint(sdf->Get<std::string>("joint"));
  if (sdf->HasElement("topic")) {
    topic = sdf->Get<std::string>("topic");
  } else {
    topic = "~/"+sdf->GetAttribute("name")->GetAsString();
  }

  forward_force = sdf->Get<double>("forward-force");
  reverse_force = sdf->Get<double>("reverse-force");

  if (sdf->HasElement("direction") && sdf->Get<std::string>("direction") == "reversed") {
    forward_force = -forward_force;
    reverse_force = -reverse_force;
  }

  gzmsg << "Initializing piston: " << topic << " joint=" << joint->GetName()
        << " forward_force=" << forward_force
        << " reverse_force=" << reverse_force<< std::endl;

  // Connect to Gazebo transport for messaging
  std::string scoped_name = model->GetWorld()->GetName()+"::"+model->GetScopedName();
  boost::replace_all(scoped_name, "::", "/");
  node = transport::NodePtr(new transport::Node());
  node->Init(scoped_name);
  sub = node->Subscribe(topic, &PneumaticPiston::Callback, this);

  // Connect to the world update event.
  // This will trigger the Update function every Gazebo iteration
  updateConn = event::Events::ConnectWorldUpdateBegin(boost::bind(&PneumaticPiston::Update, this, _1));
}

void PneumaticPiston::Update(const common::UpdateInfo &info) {
  joint->SetForce(0, signal);
}

void PneumaticPiston::Callback(const msgs::ConstFloat64Ptr &msg) {
  if (msg->data() < -0.001) { signal = -reverse_force; }
  else if (msg->data() > 0.001) { signal = forward_force; }
}
