#pragma once

#include "fwi.hpp"
#include "digital.fwi.hpp"
#include "relay.fwi.hpp"
#include "power.fwi.hpp"
#include "analog.fwi.hpp"
#include "pneumatics.fwi.hpp"
#include "motor.fwi.hpp"
#include "sensor.fwi.hpp"
#include "ds.fwi.hpp"

namespace Toast {
	namespace Memory {
		enum class RobotState {
			DISABLED = 0,
			AUTO = 1,
			TELEOP = 2,
			TEST = 3
		};
		enum class ModuleActState {
			NOT_FOUND = 0,
			DISCOVERED = 1,
			ACTIVE = 2,
			CRASHED = 3,
			RESTARTED = 4
		};
		struct SharedPool;
	}
}

namespace Toast {
	namespace Memory {
		struct SharedPool : public FWI::Block {
			static const int SIZE = 2802;

			FWIFUNC Shared::IO::Power * power();

			FWIFUNC Shared::IO::DIO * dio(int index);

			FWIFUNC Shared::IO::Relay * relay(int index);

			FWIFUNC Shared::IO::AnalogOut * analog_out(int index);

			FWIFUNC Shared::IO::AnalogIn * analog_in(int index);

			FWIFUNC Shared::IO::AnalogInSettings * analog_in_settings();

			FWIFUNC Shared::IO::Pneumatics * pneumatics(int index);

			FWIFUNC Shared::IO::Motor * motor(int index);

			FWIFUNC Shared::IO::PWM * pwm(int index);

			FWIFUNC Shared::IO::Servo * servo(int index);

			FWIFUNC Shared::IO::ServoStatic * servo_static();

			FWIFUNC Shared::IO::OnboardAccel * onboard_accel();

			FWIFUNC Shared::DS::Joystick * joystick(int index);

			FWIFUNC Shared::DS::DSInfo * ds_info();

			FWIFUNC uint8_t get_endian();
			FWIFUNC void set_endian(uint8_t value);

			FWIFUNC int32_t get_bootstrap_pid();
			FWIFUNC void set_bootstrap_pid(int32_t value);

			FWIFUNC RobotState get_state_current();
			FWIFUNC void set_state_current(RobotState value);

			FWIFUNC RobotState get_state_last();
			FWIFUNC void set_state_last(RobotState value);

			FWIFUNC uint8_t get_tick_timing();
			FWIFUNC void set_tick_timing(uint8_t value);

			FWIFUNC bool get_debug();
			FWIFUNC void set_debug(bool value);

			FWIFUNC ModuleActState get_module_activity_state(int index);
			FWIFUNC void set_module_activity_state(int index, ModuleActState value);

			Shared::IO::Power _power;
			Shared::IO::DIO _dio[26];
			Shared::IO::Relay _relay[4];
			Shared::IO::AnalogOut _analog_out[2];
			Shared::IO::AnalogIn _analog_in[8];
			Shared::IO::AnalogInSettings _analog_in_settings;
			Shared::IO::Pneumatics _pneumatics[2];
			Shared::IO::Motor _motors[16];
			Shared::IO::PWM _pwm[20];
			Shared::IO::Servo _servos[20];
			Shared::IO::ServoStatic _servo_static;
			Shared::IO::OnboardAccel _onboard_accel;
			Shared::DS::Joystick _joysticks[6];
			Shared::DS::DSInfo _ds_info;

			virtual void _update_ptr();
		}; // struct: SharedPool
	} // namespace: Memory
} // namespace: Toast
