/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <gazebo/gazebo_client.hh>
#include <gazebo/transport/transport.hh>

#include "simulation/gz_msgs/msgs.h"

using namespace gazebo;

namespace frc {

class MainNode {
 public:
  static MainNode* GetInstance() {
    static MainNode instance;
    return &instance;
  }

  template <typename M>
  static transport::PublisherPtr Advertise(const std::string& topic,
                                           int queueLimit = 10,
                                           bool latch = false) {
    return GetInstance()->main->Advertise<M>(topic, queueLimit, latch);
  }

  template <typename M, typename T>
  static transport::SubscriberPtr Subscribe(
      const std::string& topic,
      void (T::*fp)(const boost::shared_ptr<M const>&), T* obj,
      bool latching = false) {
    return GetInstance()->main->Subscribe(topic, fp, obj, latching);
  }

  template <typename M>
  static transport::SubscriberPtr Subscribe(
      const std::string& topic, void (*fp)(const boost::shared_ptr<M const>&),
      bool latching = false) {
    return GetInstance()->main->Subscribe(topic, fp, latching);
  }

  transport::NodePtr main;

 private:
  MainNode() {
    bool success = gazebo::client::setup();

    if (success) {
      main = transport::NodePtr(new transport::Node());
      main->Init("frc");
      gazebo::transport::run();
    } else {
      std::cout << "An error has occured setting up gazebo_client!"
                << std::endl;
    }
  }
};

}  // namespace frc
