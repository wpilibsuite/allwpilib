#pragma once

#include "fwi.hpp"
namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct Pneumatics;
			}
		}
	}
}

namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct Pneumatics : public FWI::Block {
					static const int SIZE = 10;

					FWIFUNC bool get_init();
					FWIFUNC void set_init(bool value);

					FWIFUNC bool get_bootstrap();
					FWIFUNC void set_bootstrap(bool value);

					FWIFUNC uint8_t get_pcm_can_id();
					FWIFUNC void set_pcm_can_id(uint8_t value);

					FWIFUNC bool get_fault_comp_too_high();
					FWIFUNC void set_fault_comp_too_high(bool value);

					FWIFUNC bool get_fault_comp_too_high_sticky();
					FWIFUNC void set_fault_comp_too_high_sticky(bool value);

					FWIFUNC bool get_fault_comp_shorted();
					FWIFUNC void set_fault_comp_shorted(bool value);

					FWIFUNC bool get_fault_comp_shorted_sticky();
					FWIFUNC void set_fault_comp_shorted_sticky(bool value);

					FWIFUNC bool get_fault_comp_not_conn();
					FWIFUNC void set_fault_comp_not_conn(bool value);

					FWIFUNC bool get_fault_comp_not_conn_sticky();
					FWIFUNC void set_fault_comp_not_conn_sticky(bool value);

					FWIFUNC bool get_comp_sticky_clear_pending();
					FWIFUNC void set_comp_sticky_clear_pending(bool value);

					FWIFUNC bool get_pressure_switch();
					FWIFUNC void set_pressure_switch(bool value);

					FWIFUNC bool get_closed_loop();
					FWIFUNC void set_closed_loop(bool value);

					FWIFUNC bool get_is_enabled();
					FWIFUNC void set_is_enabled(bool value);

					FWIFUNC bool get_start_pending();
					FWIFUNC void set_start_pending(bool value);

					FWIFUNC bool get_stop_pending();
					FWIFUNC void set_stop_pending(bool value);

					FWIFUNC bool get_closed_loop_mode_pending();
					FWIFUNC void set_closed_loop_mode_pending(bool value);

					FWIFUNC bool get_solenoid(int index);
					FWIFUNC void set_solenoid(int index, bool value);

					FWIFUNC bool get_solenoid_black(int index);
					FWIFUNC void set_solenoid_black(int index, bool value);

					FWIFUNC bool get_fault_sol_volt();
					FWIFUNC void set_fault_sol_volt(bool value);

					FWIFUNC bool get_fault_sol_volt_sticky();
					FWIFUNC void set_fault_sol_volt_sticky(bool value);

					FWIFUNC bool get_sol_sticky_clear_pending();
					FWIFUNC void set_sol_sticky_clear_pending(bool value);

					FWIFUNC float get_comp_current();
					FWIFUNC void set_comp_current(float value);


				}; // struct: Pneumatics
			} // namespace: IO
		} // namespace: Shared
	} // namespace: Memory
} // namespace: Toast
