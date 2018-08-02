#pragma once

#include "fwi.hpp"
namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct Power;
			}
		}
	}
}

namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct Power : public FWI::Block {
					static const int SIZE = 124;

					FWIFUNC uint8_t get_pdp_can_id();
					FWIFUNC void set_pdp_can_id(uint8_t value);

					FWIFUNC float get_pdp_voltage();
					FWIFUNC void set_pdp_voltage(float value);

					FWIFUNC float get_pdp_temperature();
					FWIFUNC void set_pdp_temperature(float value);

					FWIFUNC float get_pdp_total_power();
					FWIFUNC void set_pdp_total_power(float value);

					FWIFUNC float get_pdp_total_energy();
					FWIFUNC void set_pdp_total_energy(float value);

					FWIFUNC float get_pdp_total_current();
					FWIFUNC void set_pdp_total_current(float value);

					FWIFUNC float get_pdp_current(int index);
					FWIFUNC void set_pdp_current(int index, float value);

					FWIFUNC float get_rio_input_voltage();
					FWIFUNC void set_rio_input_voltage(float value);

					FWIFUNC float get_rio_input_current();
					FWIFUNC void set_rio_input_current(float value);

					FWIFUNC uint16_t get_rio_faults_3V3();
					FWIFUNC void set_rio_faults_3V3(uint16_t value);

					FWIFUNC uint16_t get_rio_faults_5V();
					FWIFUNC void set_rio_faults_5V(uint16_t value);

					FWIFUNC uint16_t get_rio_faults_6V();
					FWIFUNC void set_rio_faults_6V(uint16_t value);

					FWIFUNC bool get_rio_3V3_enabled();
					FWIFUNC void set_rio_3V3_enabled(bool value);

					FWIFUNC bool get_rio_5V_enabled();
					FWIFUNC void set_rio_5V_enabled(bool value);

					FWIFUNC bool get_rio_6V_enabled();
					FWIFUNC void set_rio_6V_enabled(bool value);

					FWIFUNC bool get_rio_brownout();
					FWIFUNC void set_rio_brownout(bool value);

					FWIFUNC float get_rio_voltage_3V3();
					FWIFUNC void set_rio_voltage_3V3(float value);

					FWIFUNC float get_rio_voltage_5V();
					FWIFUNC void set_rio_voltage_5V(float value);

					FWIFUNC float get_rio_voltage_6V();
					FWIFUNC void set_rio_voltage_6V(float value);

					FWIFUNC float get_rio_current_3V3();
					FWIFUNC void set_rio_current_3V3(float value);

					FWIFUNC float get_rio_current_5V();
					FWIFUNC void set_rio_current_5V(float value);

					FWIFUNC float get_rio_current_6V();
					FWIFUNC void set_rio_current_6V(float value);


				}; // struct: Power
			} // namespace: IO
		} // namespace: Shared
	} // namespace: Memory
} // namespace: Toast
