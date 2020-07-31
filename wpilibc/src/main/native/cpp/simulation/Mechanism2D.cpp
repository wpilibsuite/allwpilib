/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/Mechanism2D.h"

#include <wpi/SmallString.h>
#include <wpi/Twine.h>
#include <wpi/raw_ostream.h>

using namespace frc;

Mechanism2D::Mechanism2D() : m_device{"Mechanism2D"} {}

void Mechanism2D::SetLigamentAngle(const wpi::Twine& ligamentPath,
                                   float angle) {
  if (m_device) {
    wpi::SmallString<64> fullPathBuf;
    wpi::StringRef fullPath =
        (ligamentPath + "/angle").toNullTerminatedStringRef(fullPathBuf);
    if (!createdItems.count(fullPath)) {
      createdItems[fullPath] =
          m_device.CreateDouble(fullPath.data(), false, angle);
    }
    createdItems[fullPath].Set(angle);
  }
}

void Mechanism2D::SetLigamentLength(const wpi::Twine& ligamentPath,
                                    float length) {
  if (m_device) {
    wpi::SmallString<64> fullPathBuf;
    wpi::StringRef fullPath =
        (ligamentPath + "/length").toNullTerminatedStringRef(fullPathBuf);
    if (!createdItems.count(fullPath)) {
      createdItems[fullPath] =
          m_device.CreateDouble(fullPath.data(), false, length);
    }
    createdItems[fullPath].Set(length);
  }
}
