#pragma once

#include "fwi.hpp"
namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				enum class AccelRange {
					Range_2G = 0,
					Range_4G = 1,
					Range_8G = 2,
					Range_16G = 3
				};
				struct OnboardAccel;
			}
		}
	}
}

namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct OnboardAccel : public FWI::Block {
					static const int SIZE = 14;

					FWIFUNC bool get_init();
					FWIFUNC void set_init(bool value);

					FWIFUNC bool get_bootstrap();
					FWIFUNC void set_bootstrap(bool value);

					FWIFUNC AccelRange get_range();
					FWIFUNC void set_range(AccelRange value);

					FWIFUNC bool get_range_update();
					FWIFUNC void set_range_update(bool value);

					FWIFUNC float get_x();
					FWIFUNC void set_x(float value);

					FWIFUNC float get_y();
					FWIFUNC void set_y(float value);

					FWIFUNC float get_z();
					FWIFUNC void set_z(float value);


				}; // struct: OnboardAccel
			} // namespace: IO
		} // namespace: Shared
	} // namespace: Memory
} // namespace: Toast
