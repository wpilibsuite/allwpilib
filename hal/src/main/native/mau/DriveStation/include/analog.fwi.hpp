#pragma once

#include "fwi.hpp"
namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct AnalogOut;
				struct AnalogIn;
				struct AnalogInSettings;
			}
		}
	}
}

namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct AnalogOut : public FWI::Block {
					static const int SIZE = 5;

					FWIFUNC bool get_init();
					FWIFUNC void set_init(bool value);

					FWIFUNC bool get_bootstrapped();
					FWIFUNC void set_bootstrapped(bool value);

					FWIFUNC float get_voltage();
					FWIFUNC void set_voltage(float value);


				}; // struct: AnalogOut
				struct AnalogIn : public FWI::Block {
					static const int SIZE = 59;

					FWIFUNC bool get_init();
					FWIFUNC void set_init(bool value);

					FWIFUNC bool get_bootstrap();
					FWIFUNC void set_bootstrap(bool value);

					FWIFUNC int16_t get_value();
					FWIFUNC void set_value(int16_t value);

					FWIFUNC int32_t get_average_value();
					FWIFUNC void set_average_value(int32_t value);

					FWIFUNC float get_voltage();
					FWIFUNC void set_voltage(float value);

					FWIFUNC float get_average_voltage();
					FWIFUNC void set_average_voltage(float value);

					FWIFUNC uint32_t get_average_bits();
					FWIFUNC void set_average_bits(uint32_t value);

					FWIFUNC uint32_t get_oversample_bits();
					FWIFUNC void set_oversample_bits(uint32_t value);

					FWIFUNC uint32_t get_lsb_weight();
					FWIFUNC void set_lsb_weight(uint32_t value);

					FWIFUNC int32_t get_offset();
					FWIFUNC void set_offset(int32_t value);

					FWIFUNC bool get_is_accum();
					FWIFUNC void set_is_accum(bool value);

					FWIFUNC bool get_accum_pending();
					FWIFUNC void set_accum_pending(bool value);

					FWIFUNC bool get_accum_reset_pending();
					FWIFUNC void set_accum_reset_pending(bool value);

					FWIFUNC bool get_accum_initial_pending();
					FWIFUNC void set_accum_initial_pending(bool value);

					FWIFUNC bool get_accum_centre_pending();
					FWIFUNC void set_accum_centre_pending(bool value);

					FWIFUNC bool get_accum_deadband_pending();
					FWIFUNC void set_accum_deadband_pending(bool value);

					FWIFUNC uint32_t get_accum_count();
					FWIFUNC void set_accum_count(uint32_t value);

					FWIFUNC int32_t get_accum_deadband();
					FWIFUNC void set_accum_deadband(int32_t value);

					FWIFUNC int32_t get_accum_centre();
					FWIFUNC void set_accum_centre(int32_t value);

					FWIFUNC int64_t get_accum_value();
					FWIFUNC void set_accum_value(int64_t value);

					FWIFUNC int64_t get_accum_initial();
					FWIFUNC void set_accum_initial(int64_t value);


				}; // struct: AnalogIn
				struct AnalogInSettings : public FWI::Block {
					static const int SIZE = 4;

					FWIFUNC float get_sample_rate();
					FWIFUNC void set_sample_rate(float value);


				}; // struct: AnalogInSettings
			} // namespace: IO
		} // namespace: Shared
	} // namespace: Memory
} // namespace: Toast
