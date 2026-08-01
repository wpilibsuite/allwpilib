// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>
#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "wpi/util/priority_queue.hpp"

TEST_CASE(
    "PriorityQueueTest RvalueContainerConstructorHeapifiesStoredContainer",
    "[wpiutil]") {
  wpi::util::priority_queue<int> queue{std::less<int>{},
                                       std::vector<int>{1, 3, 2}};

  CHECK((3) == (queue.top()));
}

TEST_CASE(
    "PriorityQueueTest IteratorRvalueContainerConstructorUsesStoredContainer",
    "[wpiutil]") {
  std::vector<int> values{4, 5};
  wpi::util::priority_queue<int> queue{values.begin(), values.end(),
                                       std::less<int>{},
                                       std::vector<int>{1, 3, 2}};

  CHECK((5) == (queue.top()));
  CHECK((5u) == (queue.size()));
}

TEST_CASE("PriorityQueueTest ConstContainerConstructorsUseStoredContainer",
          "[wpiutil]") {
  const std::vector<int> base{1, 3, 2};
  wpi::util::priority_queue<int> queue{std::less<int>{}, base};

  CHECK((3) == (queue.top()));

  std::vector<int> values{4, 5};
  wpi::util::priority_queue<int> rangeQueue{values.begin(), values.end(),
                                            std::less<int>{}, base};

  CHECK((5) == (rangeQueue.top()));
  CHECK((5u) == (rangeQueue.size()));
}
