/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/Mechanism2D.h"

#include <wpi/raw_ostream.h>

using namespace frc;

Mechanism2D::Mechanism2D() : m_device{"Mechanism2D"} {}

void Mechanism2D::SetLigamentAngle(std::string ligamentPath, float angle) {
  if (m_device) {
    ligamentPath = ligamentPath + "angle/";
    wpi::outs() << ligamentPath;
    if (!createdItems.count(ligamentPath)) {
      createdItems[ligamentPath] =
          m_device.CreateDouble(ligamentPath.c_str(), false, angle);
    }
  } else {
    createdItems[ligamentPath].Set(angle);
  }
}

void Mechanism2D::SetLigamentALength(std::string ligamentPath, float length) {
  if (m_device) {
    ligamentPath = ligamentPath + "length/";
    if (!createdItems.count(ligamentPath)) {
      createdItems[ligamentPath] =
          m_device.CreateDouble(ligamentPath.c_str(), false, length);
    }
  } else {
    createdItems[ligamentPath].Set(length);
  }
}
