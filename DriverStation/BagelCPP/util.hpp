#pragma once

#include "library.hpp"
#include <string>

namespace Toast {
	namespace Net {
		namespace Util {
			enum class AdapterType {
				OTHER,
				ETHERNET,
				WIRELESS,
				LOOPBACK,
				UNKNOWN
			};

			struct Adapter {
				AdapterType type;
				int index;
				std::string name;
				std::string description;
				std::string ip_addr;
			};

			API void get_ip(std::string notation, unsigned char *ip);
			API std::string get_ip_str(std::string notation);
			API std::string ip_to_string(unsigned char *ip);
			API bool get_ip_for_adapter(std::string adapter, unsigned char *ip);

			API void refresh_adapters();
			API int get_adapter_count();
			API Adapter *get_adapter(int i);

		}
	}
}