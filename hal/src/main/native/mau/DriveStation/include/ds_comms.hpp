#pragma once

namespace Sim {
	namespace DriverStationComms {
		void start();
		void stop();

		void decode_ds_packet(char *data, int length);
		void encode_ds_packet(char *data);

		void decode_ds_tcp_packet(char *data, int length);

		void periodic_update();
	};
}
	