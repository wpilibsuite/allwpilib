#include <tagpose.h>

namespace tag {
    pose::pose(double xpos, double ypos, double zpos, double w, double x, double y, double z) {
        xPos = xpos;
        yPos = ypos;
        zPos = zpos;
        quaternion = Eigen::Quaterniond(w, x, y, z);
        rotationMatrix = quaternion.toRotationMatrix();
        transformMatrixFmap.setZero();
        transformMatrixFmap.block<3, 3>(0, 0) = rotationMatrix;
        transformMatrixFmap(0, 3) = xpos - (16.541 / 2.0);
        transformMatrixFmap(1, 3) = ypos - (8.211 / 2.0);
        transformMatrixFmap(2, 3) = zpos;
        transformMatrixFmap(3, 3) = 1;
        transformMatrixFmap(3, 0) = 0;
        transformMatrixFmap(3, 1) = 0;
        transformMatrixFmap(3, 2) = 0;
        Eigen::Vector3d eulerAngles = rotationMatrix.eulerAngles(0, 1, 2);
        rollRot = eulerAngles[0];
        pitchRot = eulerAngles[1];
        yawRot = eulerAngles[2];
    }
}
