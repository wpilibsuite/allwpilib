
#include "DriveData.h"

Mau_DriveData* driveData;

Mau_DriveData::Mau_DriveData(Alliance al, DSInfo info) {
    alliance = al;
    stationInfo = info;
}

void Mau_DriveData::addJoystick(int joyNumber, Joystick joy, JoystickDescriptor desc, JoystickType type,
                                JoystickAxisType axes) {
    Mau_SharedJoystick newStick;
    newStick.joystick = joy;
    newStick.joyDescriptor = desc;
    newStick.joyType = type;
    newStick.joyAxisType = axes;

    joysticks[joyNumber] = newStick;
}

void Mau_DriveData::setAxisCount(int joyNum, int count) {
	// Mutex.lock()

	getJoystick(joyNum).JoystickDescriptor.set_num_axis(count);

	// Mutex.unlock()
}

void Mau_DriveData::setButtonCount(int joyNum, int count) {
	// Mutex.lock()

	getJoystick(joyNum).JoystickDescriptor.set_num_button(count);

	// Mutex.unlock()
}

void Mau_DriveData::setPovCount(int joyNum, int count) {
	// Mutex.lock()

	getJoystick(joyNum).JoystickDescriptor.set_num_pov(count);

	// Mutex.unlock()
}

void Mau_DriveData::setButtonMask(int JoyNum, int mask) {
	// Mutex.lock()

	getJoystick(joyNum).JoystickDescriptor.set_button_mask(mask);

	// Mutex.unlock()
}

void Mau_DriveData::setAxis(int joyNum, int axis, int value) {
	// Mutex.lock()

	getJoystick(joyNum).JoystickDescriptor.set_axis(axis, value);

	// Mutex.unlock()
}

void Mau_DriveData::setPov(int joyNum, int pov, int value) {
	// Mutex.lock()

	getJoystick(joyNum).JoystickDescriptor.set_pov(pov, value);

	// Mutex.unlock()
}

int Mau_DriveData::getAxisCount(int joyNum) {
	return getJoystick(joyNum).JoystickDescriptor.get_num_axis();
}

int Mau_DriveData::getPovCount(int JoyNum) {
	return getJoystick(joyNum).JoystickDescriptor.get_num_pov();
}

Mau_SharedJoystick Mau_DriveData::getJoystick(int index) {
    if (!(index >= 0 && index < 6)) {
        // Log an Error!
    } else {
        return joysticks[index];
    }
}

Alliance Mau_DriveData::getAlliance() {
    return alliance;
}

DSInfo Mau_DriveData::getDSInfo() {
    return stationInfo;
}

