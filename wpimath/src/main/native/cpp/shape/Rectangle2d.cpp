#include "wpi/math/shape/Rectangle2d.hpp"

#include "wpi/util/json.hpp"

using namespace wpi::math;

void wpi::math::to_json(wpi::util::json& json, const Rectangle2d& rectangle) {
  json.set_object();
  json["center"] = rectangle.Center();
  json["xWidth"] = rectangle.XWidth().value();
  json["yWidth"] = rectangle.YWidth().value();
}

Rectangle2d wpi::util::json_serializer<Rectangle2d>::from(const json& json) {
  return Rectangle2d{json.at("center").get<Pose2d>(),
                     wpi::units::meter_t{json.at("xWidth").get_number()},
                     wpi::units::meter_t{json.at("yWidth").get_number()}};
}