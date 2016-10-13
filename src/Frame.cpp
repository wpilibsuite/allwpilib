/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Frame.h"

#include "SourceImpl.h"

using namespace cs;

void Frame::ReleaseFrame() {
  m_source->ReleaseFrame(std::unique_ptr<Data>(m_data));
  m_data = nullptr;
}
