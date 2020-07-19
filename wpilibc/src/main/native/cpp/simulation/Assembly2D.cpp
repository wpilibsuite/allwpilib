/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/



#include "frc/simulation/Assembly2D.h"

using namespace frc;


Assembly2D::Assembly2D() : m_device{"Assembly2D"} {}

void Assembly2D::SetLigamentPose(std::string ligamentPath, float angle) {
    if (m_device) {
        if (!createdItems.count(ligamentPath)) {
            createdItems[ligamentPath] = m_device.CreateDouble(ligamentPath.c_str(), false, angle);
        }
    } else {
        createdItems[ligamentPath].Set(angle);
    }
}

