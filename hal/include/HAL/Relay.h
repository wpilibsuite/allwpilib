#pragma once
#include <stdint.h>

extern "C" {
bool checkRelayChannel(void* digital_port_pointer);

void setRelayForward(void* digital_port_pointer, bool on, int32_t* status);
void setRelayReverse(void* digital_port_pointer, bool on, int32_t* status);
bool getRelayForward(void* digital_port_pointer, int32_t* status);
bool getRelayReverse(void* digital_port_pointer, int32_t* status);
}
