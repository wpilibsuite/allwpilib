/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "raw_istream.h"

#include <cstring>

using namespace nt;

bool raw_mem_istream::read(void* data, std::size_t len) {
  if (len > m_left) return false;
  std::memcpy(data, m_cur, len);
  m_cur += len;
  m_left -= len;
  return true;
}
