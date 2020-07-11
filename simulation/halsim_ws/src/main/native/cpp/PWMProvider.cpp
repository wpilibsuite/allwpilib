#include "PWMProvider.h"
#include "MessageSchema.h"

#include <hal/Ports.h>
#include <hal/simulation/PWMData.h>

void PWMProvider::Initialize(WSRegisterFunc wsRegisterFunc) {
  CreateProviders<PWMProvider>("PWM",
                               HAL_GetNumPWMChannels(),
                               HALSIM_RegisterPWMAllCallbacks,
                               wsRegisterFunc);
}

wpi::json PWMProvider::OnSimValueChanged(const char* name) {
  std::string cbType(name);

  if (!HALSIM_GetPWMInitialized(m_channel)) {
    return {};
  }

  if (cbType == "RawValue") {
    // 0-255 -> [-1, 1]
    int32_t rawVal = HALSIM_GetPWMRawValue(m_channel);
    double normalized = ((rawVal / 255.0) * 2.0) - 1.0;

    wpilibws::pwm::value::Message valueMessage{m_channel, normalized};
    return {
      { "topic", "pwm/out" },
      { "payload", valueMessage }
    };
  }
  else if (cbType == "Speed" || cbType == "Position") {
    double value = (cbType == "Speed") ? HALSIM_GetPWMSpeed(m_channel) :
                                         HALSIM_GetPWMPosition(m_channel);
    wpilibws::pwm::value::Message valueMessage{m_channel, value};
    return {
      { "topic", "pwm/out" },
      { "payload", valueMessage }
    };
  }

  return {};
}