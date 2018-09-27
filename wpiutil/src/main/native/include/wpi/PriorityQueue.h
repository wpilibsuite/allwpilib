/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_PRIORITYQUEUE_H_
#define WPIUTIL_WPI_PRIORITYQUEUE_H_

#include <algorithm>
#include <functional>
#include <queue>
#include <vector>

namespace wpi {

/**
 * This class adds a method for removing all elements from the priority queue
 * matching the given value.
 */
template <class T, class Container = std::vector<T>,
          class Compare = std::less<typename Container::value_type>>
class PriorityQueue : public std::priority_queue<T, Container, Compare> {
 public:
  bool remove(const T& value) {
    auto it = std::find(this->c.begin(), this->c.end(), value);
    if (it != this->c.end()) {
      this->c.erase(it);
      std::make_heap(this->c.begin(), this->c.end(), this->comp);
      return true;
    } else {
      return false;
    }
  }
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_PRIORITYQUEUE_H_
