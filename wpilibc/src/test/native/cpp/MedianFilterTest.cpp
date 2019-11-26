/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/MedianFilter.h"
#include "gtest/gtest.h"

TEST(MedianFilterTest, MedianFilterNotFullTest) {
  frc::MedianFilter filter{10};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(4);

  EXPECT_EQ(filter.Calculate(1000), 3.5);
}

TEST(MedianFilterTest, MedianFilterFullTest) {
  frc::MedianFilter filter{5};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(5);
  filter.Calculate(4);
  filter.Calculate(1000);

  EXPECT_EQ(filter.Calculate(99), 5);
}
