// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/FastQueue.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

TEST_CASE("FastQueueTest Basic", "[wpiutil]") {
  wpi::util::FastQueue<int> q;
  q.enqueue(25);

  int item;
  bool found = q.try_dequeue(item);
  CHECK(found);
  CHECK((item) == (25));
}
