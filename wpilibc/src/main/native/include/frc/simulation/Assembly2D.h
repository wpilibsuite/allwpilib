/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/SimDevice.h>
#include <units/units.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include <string>
#include <map>

namespace frc {
    class Assembly2D {
    public:
        Assembly2D();

        /**
         * Set/Create the angle of a ligament
         *
         * @param ligamentPath json path to the ligament
        * @param angle to set the ligament
         */
        void SetLigamentPose(std::string ligamentPath, float angle);

    private:
        std::map<std::string, hal::SimDouble> createdItems;
        hal::SimDevice m_device;
    };

}  // namespace frc
