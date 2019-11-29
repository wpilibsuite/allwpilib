/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

namespace wpi {

// Binary insortion into vector; std::log(n) efficiency.
template <typename T>
typename std::vector<T>::iterator insert_sorted(std::vector<T>& vec,
                                                T const& item) {
  return vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}
}  // namespace wpi
