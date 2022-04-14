// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <gazebo/gazebo_client.hh>
#include <gazebo/transport/transport.hh>

class GazeboNode {
 public:
  bool Connect();

  template <typename M>
  gazebo::transport::PublisherPtr Advertise(const std::string& topic,
                                            int queueLimit = 10,
                                            bool latch = false) {
    return main->Advertise<M>(topic, queueLimit, latch);
  }

  template <typename M, typename T>
  gazebo::transport::SubscriberPtr Subscribe(
      const std::string& topic,
      void (T::*fp)(const boost::shared_ptr<M const>&), T* obj,
      bool latching = false) {
    return main->Subscribe(topic, fp, obj, latching);
  }

  template <typename M>
  gazebo::transport::SubscriberPtr Subscribe(
      const std::string& topic, void (*fp)(const boost::shared_ptr<M const>&),
      bool latching = false) {
    return main->Subscribe(topic, fp, latching);
  }

 private:
  gazebo::transport::NodePtr main;
};
