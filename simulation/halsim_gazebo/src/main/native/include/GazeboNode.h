/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
