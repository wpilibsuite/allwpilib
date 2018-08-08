#pragma once
#include <cstdint>
#include <mutex>

#define IS_BIT_SET(expression, bit) ((expression & (1 << bit)) != 0)

namespace mau {
    namespace comms {
        typedef struct {
            uint8_t axis_count, pov_count, button_count;
            uint16_t pov[4];
            uint8_t axis[16];
            uint32_t button_mask;
            bool has_update;
        } _TempJoyData;

        static std::mutex runLock;
        static bool isRunning;

        void start();
        void stop();
        void decodePacket(char* data, int length);
        void encodePacket(char* data);
        void decodeTcpPacket(char* data, int length);
        void periodicUpdate();
    }
}
	