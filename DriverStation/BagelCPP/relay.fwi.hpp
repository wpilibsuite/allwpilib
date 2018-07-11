#pragma once

#include "fwi.hpp"
namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct Relay;
			}
		}
	}
}

namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace IO {
				struct Relay : public FWI::Block {
					static const int SIZE = 1;

					FWIFUNC bool get_init();
					FWIFUNC void set_init(bool value);

					FWIFUNC bool get_bootstrap();
					FWIFUNC void set_bootstrap(bool value);

					FWIFUNC bool get_fwd();
					FWIFUNC void set_fwd(bool value);

					FWIFUNC bool get_rvs();
					FWIFUNC void set_rvs(bool value);


				}; // struct: Relay
			} // namespace: IO
		} // namespace: Shared
	} // namespace: Memory
} // namespace: Toast
