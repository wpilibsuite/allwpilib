// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
