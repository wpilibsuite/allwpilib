#pragma once

#ifndef FIELDMAP_H
#define FIELDMAP_H

#include <nlohmann/json.hpp>
#include <vector>
#include <tagpose.h>
#include <numbers>

class fieldmap {
public:
    fieldmap();
    fieldmap(nlohmann::json map);
    tag::pose getTag(int tag);
    int getNumTags();
    double minimizeAngle(double angle);

private:
    std::vector<tag::pose> tagVec;
};

#endif // FIELDMAP_H
