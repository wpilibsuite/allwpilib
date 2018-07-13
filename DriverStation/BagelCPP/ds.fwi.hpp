#pragma once

#include "fwi.hpp"
namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace DS {
				enum class Alliance {
					Unknown = 0,
					Red = 1,
					Blue = 2
				};
				enum class JoystickType {
					UNKNOWN = 0,
					XINPUT_GAMEPAD = 1,
					XINPUT_WHEEL = 2,
					XINPUT_ARCADE = 3,
					XINPUT_FLIGHT_STICK = 4,
					XINPUT_DANCE_PAD = 5,
					XINPUT_GUITAR = 6,
					XINPUT_GUITAR_2 = 7,
					XINPUT_DRUM_KIT = 8,
					XINPUT_GUITAR_3 = 11,
					XINPUT_ARCADE_PAD = 19,
					HID_JOYSTICK = 20,
					HID_GAMEPAD = 21,
					HID_DRIVING = 22,
					HID_FLIGHT = 23,
					HID_FIRST_PERSON = 24
				};
				enum class JoystickAxisType {
					X = 0,
					Y = 1,
					Z = 2,
					TWIST = 3,
					THROTTLE = 4
				};
				struct JoystickDescriptor;
				struct Joystick;
				struct DSInfo;
			}
		}
	}
}

namespace Toast {
	namespace Memory {
		namespace Shared {
			namespace DS {
				struct JoystickDescriptor : public FWI::Block {
					static const int SIZE = 80;

					FWIFUNC bool get_is_xbox();
					FWIFUNC void set_is_xbox(bool value);

					FWIFUNC JoystickType get_type();
					FWIFUNC void set_type(JoystickType value);

					FWIFUNC uint8_t get_name_length();
					FWIFUNC void set_name_length(uint8_t value);

					FWIFUNC char * get_name();
					FWIFUNC int name_length();

					FWIFUNC uint8_t get_axis_count();
					FWIFUNC void set_axis_count(uint8_t value);

					FWIFUNC JoystickAxisType get_axis_type(int index);
					FWIFUNC void set_axis_type(int index, JoystickAxisType value);


				}; // struct: JoystickDescriptor
				struct Joystick : public FWI::Block {
					static const int SIZE = 120;

					FWIFUNC JoystickDescriptor * get_descriptor();

					FWIFUNC bool get_init();
					FWIFUNC void set_init(bool value);

					FWIFUNC bool get_bootstrap();
					FWIFUNC void set_bootstrap(bool value);

					FWIFUNC uint8_t get_num_axis();
					FWIFUNC void set_num_axis(uint8_t value);

					FWIFUNC uint8_t get_num_button();
					FWIFUNC void set_num_button(uint8_t value);

					FWIFUNC uint8_t get_num_pov();
					FWIFUNC void set_num_pov(uint8_t value);

					FWIFUNC uint32_t get_button_mask();
					FWIFUNC void set_button_mask(uint32_t value);

					FWIFUNC uint32_t get_outputs_mask();
					FWIFUNC void set_outputs_mask(uint32_t value);

					FWIFUNC int16_t get_pov(int index);
					FWIFUNC void set_pov(int index, int16_t value);

					FWIFUNC int8_t get_axis(int index);
					FWIFUNC void set_axis(int index, int8_t value);

					FWIFUNC int16_t get_rumble_l();
					FWIFUNC void set_rumble_l(int16_t value);

					FWIFUNC int16_t get_rumble_r();
					FWIFUNC void set_rumble_r(int16_t value);

					JoystickDescriptor _descriptor;

					virtual void _update_ptr();
				}; // struct: Joystick
				struct DSInfo : public FWI::Block {
					static const int SIZE = 7;

					FWIFUNC bool get_ds_attached();
					FWIFUNC void set_ds_attached(bool value);

					FWIFUNC bool get_new_control_data();
					FWIFUNC void set_new_control_data(bool value);

					FWIFUNC bool get_fms_attached();
					FWIFUNC void set_fms_attached(bool value);

					FWIFUNC bool get_system_active();
					FWIFUNC void set_system_active(bool value);

					FWIFUNC float get_match_time();
					FWIFUNC void set_match_time(float value);

					FWIFUNC Alliance get_alliance();
					FWIFUNC void set_alliance(Alliance value);

					FWIFUNC uint8_t get_alliance_station();
					FWIFUNC void set_alliance_station(uint8_t value);


				}; // struct: DSInfo
			} // namespace: DS
		} // namespace: Shared
	} // namespace: Memory
} // namespace: Toast
