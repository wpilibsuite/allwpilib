/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/MedianFilter.h"

#include <algorithm>

#include <wpi/Algorithm.h>

using namespace frc;

MedianFilter::MedianFilter(size_t size) : m_valueBuffer{size}, m_size{size} {}

double MedianFilter::Calculate(double next) {
  // Insert next value at proper point in sorted array
  wpi::insert_sorted(m_orderedValues, next);

  size_t curSize = m_orderedValues.size();

  // If buffer is at max size, pop element off of end of circular buffer
  // and remove from ordered list
  if (curSize > m_size) {
    m_orderedValues.erase(std::find(m_orderedValues.begin(),
                                    m_orderedValues.end(),
                                    m_valueBuffer.pop_back()));
    curSize = curSize - 1;
  }

  // Add next value to circular buffer
  m_valueBuffer.push_front(next);

  if (curSize % 2 == 1) {
    // If size is odd, return middle element of sorted list
    return m_orderedValues[curSize / 2];
  } else {
    // If size is even, return average of middle elements
    return (m_orderedValues[curSize / 2 - 1] + m_orderedValues[curSize / 2]) /
           2.0;
  }
}
