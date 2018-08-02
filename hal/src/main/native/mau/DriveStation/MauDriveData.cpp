#include "MauDriveData.h"

// ----- HAL Data: Write ----- //

Mau_DriveData::Mau_DriveData(HAL_ControlWord* word, HAL_AllianceStationID* id, HAL_MatchInfo* info) {
    currControlWord = word;
    allianceID = id;
    matchInfo = info;
}

void Mau_DriveData::writeJoystick(int joyNumber, HAL_JoystickAxes axes, HAL_JoystickPOVs povs,
                                  HAL_JoystickButtons buttons, HAL_JoystickDescriptor desc) {
    Mau_SharedJoystick newJoy;
    newJoy.joyAxes = &axes;
    newJoy.joyPOVs = &povs;
    newJoy.joyButtons = &buttons;
    newJoy.joyDescriptor = &desc;

    joysticks[joyNumber] = newJoy;
}

// ----- HAL Data: Update ----- //

void Mau_DriveData::updateControlWord(bool enabled, bool auton, bool test, bool estop, bool fms, bool ds) {
    HAL_ControlWord newWord;
    newWord.enabled = enabled;
    newWord.autonomous = auton;
    newWord.test = test;
    newWord.eStop = estop;
    newWord.fmsAttached = fms;
    newWord.dsAttached = ds;

    prevControlWord = currControlWord;
    currControlWord = &newWord;
    wordUnread = true;
}

void Mau_DriveData::updateJoyAxis(int joyNumber, int axisIndex, float axis) {
    joysticks[joyNumber].joyAxes->axes[axisIndex] = axis;
}

void Mau_DriveData::updateJoyPOV(int joyNumber, int povIndex, int16_t pov) {
    joysticks[joyNumber].joyPOVs->povs[povIndex] = pov;
}

void Mau_DriveData::updateJoyButtons(int joyNumber, uint32_t buttons) {
    joysticks[joyNumber].joyButtons->buttons = buttons;
}

// ----- HAL Data: Read ----- //

Mau_SharedJoystick Mau_DriveData::getJoystick(int joyNumber) {
    return joysticks[joyNumber];
}

HAL_ControlWord* Mau_DriveData::readPreviousControl() {
    return prevControlWord;
}

HAL_ControlWord* Mau_DriveData::readCurrentControl() {
    wordUnread == false;
    return currControlWord;
}

bool Mau_DriveData::readIfControlRead() {
    return wordUnread;
}

HAL_AllianceStationID* Mau_DriveData::readAllianceID() {
    return allianceID;
}

HAL_MatchInfo* Mau_DriveData::readMatchInfo() {
    return matchInfo;
}

HAL_MatchType* Mau_DriveData::readMatchType() {
    HAL_MatchType type = matchInfo->matchType;
    return &type;
}

HAL_JoystickAxes* Mau_DriveData::readJoyAxes(int joyNumber) {
    return getJoystick(joyNumber).joyAxes;
}

HAL_JoystickPOVs* Mau_DriveData::readJoyPOVs(int joyNumber) {
    return getJoystick(joyNumber).joyPOVs;
}

HAL_JoystickButtons* Mau_DriveData::readJoyButtons(int joyNumber) {
    return getJoystick(joyNumber).joyButtons;
}

HAL_JoystickDescriptor* Mau_DriveData::readJoyDescriptor(int joyNumber) {
    return getJoystick(joyNumber).joyDescriptor;
}



