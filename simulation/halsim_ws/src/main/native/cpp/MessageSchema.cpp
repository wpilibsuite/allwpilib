#include "MessageSchema.h"

// to/from_json definitions

namespace wpilibws {
  namespace digitalio {
    namespace config {
      void to_json(wpi::json& j, const Message& m) {
        j = wpi::json{
          {"channel", m.channel},
          {"isInput", m.isInput}
        };
      }

      void from_json(const wpi::json& j, Message& m) {
        m.channel = j.at("channel").get<int>();
        m.isInput = j.at("isInput").get<bool>();
      }
    } // END NAMESPACE wpilibws::digitalio::config

    namespace value {
      void to_json(wpi::json& j, const Message& m) {
        j = wpi::json{
          {"channel", m.channel},
          {"value", m.value}
        };
      }

      void from_json(const wpi::json& j, Message& m) {
        m.channel = j.at("channel").get<int>();
        m.value = j.at("value").get<bool>();
      }
    } // END NAMESPACE wpilibws::digitalio::value
  } // END NAMESPACE wpilibws::digitalio

  namespace analogio {
    namespace value {
      void to_json(wpi::json& j, const Message& m) {
        j = wpi::json{
          {"channel", m.channel},
          {"voltage", m.voltage}
        };
      }

      void from_json(const wpi::json& j, Message& m) {
        m.channel = j.at("channel").get<int>();
        m.voltage = j.at("voltage").get<double>();
      }
    } // END NAMESPACE wpilibws::analogio::value
  } // END NAMESPACE wpilibws::analogio

  namespace pwm {
    namespace value {
      void to_json(wpi::json& j, const Message& m) {
        j = wpi::json{
          {"channel", m.channel},
          {"value", m.value}
        };
      }

      void from_json(const wpi::json& j, Message& m) {
        m.channel = j.at("channel").get<int>();
        m.value = j.at("value").get<double>();
      }
    } // END NAMESPACE wpilibws::pwm::value
  } // END NAMESPACE wpilibws::pwm
}