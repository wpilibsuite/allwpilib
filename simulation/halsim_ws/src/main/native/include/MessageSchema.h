#pragma once

#include <wpi/json.h>

namespace wpilibws {
  // Digital IO messages
  namespace digitalio {
    // Configuration messages
    namespace config {
      struct Message {
        int channel;
        bool isInput;
      };

      void to_json(wpi::json& j, const Message& m);
      void from_json(const wpi::json&, Message& m);
    }

    // Value messages
    namespace value {
      struct Message {
        int channel;
        bool value;
      };

      void to_json(wpi::json& j, const Message& m);
      void from_json(const wpi::json&, Message& m);
    }
  }

  // Anlog IO messages
  namespace analogio {
    namespace value {
      struct Message {
        int channel;
        double voltage;
      };

      void to_json(wpi::json& j, const Message& m);
      void from_json(const wpi::json&, Message& m);
    }
  }

  // PWM messages
  namespace pwm {
    namespace value {
      struct Message {
        int channel;
        double value;
      };

      void to_json(wpi::json& j, const Message& m);
      void from_json(const wpi::json&, Message& m);
    }
  }
}