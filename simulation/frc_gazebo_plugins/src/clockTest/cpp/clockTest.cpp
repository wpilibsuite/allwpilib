/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <simulation/gz_msgs/msgs.h>

#include <gazebo/gazebo.hh>
#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/sensors/sensors.hh>
#include <gazebo/transport/transport.hh>

#include "gtest/gtest.h"

using namespace testing;

static char *library;
static char *world_sdf;
static double latest_time;

void cb(gazebo::msgs::ConstFloat64Ptr &msg)
{
    latest_time = msg->data();
}

TEST(ClockTests, test_clock) {
    gazebo::physics::WorldPtr world;

    ASSERT_TRUE(library);
    ASSERT_TRUE(!access(library, X_OK|R_OK));
    ASSERT_TRUE(world_sdf);
    ASSERT_TRUE(!access(world_sdf, R_OK));

    gazebo::addPlugin(library);
    ASSERT_TRUE(gazebo::setupServer());

    world = gazebo::loadWorld(world_sdf);
    ASSERT_TRUE(world != NULL);

    ASSERT_TRUE(gazebo::client::setup());

    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    gazebo::transport::SubscriberPtr sub = node->Subscribe("/gazebo/frc/time", cb);


    gazebo::sensors::run_once(true);
    gazebo::sensors::run_threads();
    gazebo::common::Time::MSleep(1);

    double start = latest_time;

    for (unsigned int i = 0; i < 1000; ++i)
    {
        gazebo::runWorld(world, 1);
        gazebo::sensors::run_once();
        gazebo::common::Time::MSleep(1);
    }

    double end = latest_time;

    ASSERT_TRUE(end > start);
    ASSERT_TRUE(gazebo::client::shutdown());
    ASSERT_TRUE(gazebo::shutdown());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  if (argc >= 1)
      library = argv[1];

  if (argc >= 2)
      world_sdf = argv[2];

  return RUN_ALL_TESTS();
}
