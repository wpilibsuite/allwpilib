#include <fmap.h>
#include <iostream>
#include <numbers>

std::string fmap::singleTag(int tag, tag::pose tagpose, bool endTag) {
    std::string transform = "";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            transform += std::to_string(tagpose.transformMatrixFmap(i, j));
            transform += ",";
        }
    }

    // remove end comma
    transform = transform.substr(0, transform.length() - 2);

    std::string fmapTag = "{\"family\":\"apriltag3_36h11_classic\",\"id\":" + std::to_string(tag) + ",\"size\":165.1,\"transform\":[" + transform + "],\"unique\":true}";

    if (!endTag) {
        fmapTag += ",";
    }
    return fmapTag;
}

std::string fmap::convertfmap(nlohmann::json json) {
    std::string fmapstart = "{\"fiducials\":[";
    
    std::string fmapend = "],\"type\":\"frc\"}";

    fieldmap fieldmap(json);

    for (int i = 0; i < fieldmap.getNumTags(); i++) {
        fmapstart += singleTag(i + 1, fieldmap.getTag(i + 1), i == fieldmap.getNumTags() - 1 ? true : false);
    }

    return fmapstart.append(fmapend);
}
