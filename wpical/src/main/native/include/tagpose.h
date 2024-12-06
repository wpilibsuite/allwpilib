#pragma once

#ifndef TAGPOSE_H
#define TAGPOSE_H

#include <Eigen/Dense>
#include <Eigen/Geometry>
namespace tag {
    class pose {
        public:
            pose(double xpos, double ypos, double zpos, double w, double x, double y, double z);
            double xPos, yPos, zPos, yawRot, rollRot, pitchRot;
            Eigen::Quaterniond quaternion;
            Eigen::Matrix3d rotationMatrix;
            Eigen::Matrix4d transformMatrixFmap;
    };
}

#endif