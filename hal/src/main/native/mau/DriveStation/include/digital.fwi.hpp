#pragma once

#include "fwi.hpp"
namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				enum class DIO_Mode {
					INPUT = 1,
					OUTPUT = 2
				};
				struct DIO;
			}
		}
	}
}

namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct DIO : public FWI::Block {
					static const int SIZE = 15;

					FWIFUNC bool get_init();
					FWIFUNC void set_init(bool value);

					FWIFUNC bool get_bootstrap();
					FWIFUNC void set_bootstrap(bool value);

					FWIFUNC DIO_Mode get_mode();
					FWIFUNC void set_mode(DIO_Mode value);

					FWIFUNC bool get_value();
					FWIFUNC void set_value(bool value);

					FWIFUNC bool get_pwm_enabled();
					FWIFUNC void set_pwm_enabled(bool value);

					FWIFUNC bool get_pwm_enabled_pending();
					FWIFUNC void set_pwm_enabled_pending(bool value);

					FWIFUNC bool is_pulsing();
					FWIFUNC void set_is_pulsing(bool value);

					FWIFUNC bool get_pulse_pending();
					FWIFUNC void set_pulse_pending(bool value);

					FWIFUNC bool get_pwm_rate_pending();
					FWIFUNC void set_pwm_rate_pending(bool value);

					FWIFUNC bool get_pwm_duty_cycle_pending();
					FWIFUNC void set_pwm_duty_cycle_pending(bool value);

					FWIFUNC float get_pulse_length();
					FWIFUNC void set_pulse_length(float value);

					FWIFUNC float get_pwm_duty_cycle();
					FWIFUNC void set_pwm_duty_cycle(float value);

					FWIFUNC float get_pwm_rate();
					FWIFUNC void set_pwm_rate(float value);


				}; // struct: DIO
			} // namespace: IO
		} // namespace: Shared
	} // namespace: Memory
} // namespace: Toast
