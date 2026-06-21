// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/filter/MedianFilter.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("MedianFilterTest MedianFilterNotFullTestEven", "[wpimath]") {
  wpi::math::MedianFilter<double> filter{10};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(4);

  CHECK(filter.Calculate(1000) == 3.5);
}

TEST_CASE("MedianFilterTest MedianFilterNotFullTestOdd", "[wpimath]") {
  wpi::math::MedianFilter<double> filter{10};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(4);
  filter.Calculate(7);

  CHECK(filter.Calculate(1000) == 4);
}

TEST_CASE("MedianFilterTest MedianFilterFullTestEven", "[wpimath]") {
  wpi::math::MedianFilter<double> filter{6};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(0);
  filter.Calculate(5);
  filter.Calculate(4);
  filter.Calculate(1000);

  CHECK(filter.Calculate(99) == 4.5);
}

TEST_CASE("MedianFilterTest MedianFilterFullTestOdd", "[wpimath]") {
  wpi::math::MedianFilter<double> filter{5};

  filter.Calculate(3);
  filter.Calculate(0);
  filter.Calculate(5);
  filter.Calculate(4);
  filter.Calculate(1000);

  CHECK(filter.Calculate(99) == 5);
}
