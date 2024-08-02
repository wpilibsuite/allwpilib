#include "fieldmap.h"
#include <cmath>
#include <numbers>
#include <Eigen/Dense>
#include <Eigen/Geometry>

fieldmap::fieldmap() {}

fieldmap::fieldmap(nlohmann::json json) {
    for (auto& tag : json["tags"].items()) {
        double tagXPos = tag.value()["pose"]["translation"]["x"];
        double tagYPos = tag.value()["pose"]["translation"]["y"];
        double tagZPos = tag.value()["pose"]["translation"]["z"];
        double tagWQuat = tag.value()["pose"]["rotation"]["quaternion"]["W"];
        double tagXQuat = tag.value()["pose"]["rotation"]["quaternion"]["X"];
        double tagYQuat = tag.value()["pose"]["rotation"]["quaternion"]["Y"];
        double tagZQuat = tag.value()["pose"]["rotation"]["quaternion"]["Z"];
        
        tagVec.push_back(tag::pose(tagXPos, tagYPos, tagZPos, tagWQuat, tagXQuat, tagYQuat, tagZQuat));
    }
}

tag::pose fieldmap::getTag(int tag) {
    return tagVec[tag - 1];
}

int fieldmap::getNumTags() {
    return tagVec.size();
}

double fieldmap::minimizeAngle(double angle) {
    if (angle > 180) {
        return angle - 360;
    } else if (angle < -180) {
        return angle + 360;
    }
    return angle;
}
