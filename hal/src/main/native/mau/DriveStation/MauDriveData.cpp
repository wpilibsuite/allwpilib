#include "MauDriveData.h"

// ----- HAL Data: Creator ----- //

void Mau_DriveData::createMemoryMutex() {
    wpi::priority_mutex newLock;
    memLock = &newLock;
}

Mau_DriveData::Mau_DriveData() {
    createMemoryMutex();
}

Mau_DriveData::Mau_DriveData(HAL_ControlWord* word, HAL_AllianceStationID* id, HAL_MatchInfo* info) {
    createMemoryMutex();
    currControlWord = word;
    allianceID = id;
    matchInfo = info;
}

// ----- HAL Data: Write ----- //

void
Mau_DriveData::writeJoystick(int joyNumber, HAL_JoystickAxes axes, HAL_JoystickPOVs povs, HAL_JoystickButtons buttons,
                             HAL_JoystickDescriptor desc) {
    memLock->lock();
    Mau_SharedJoystick newJoy;
    newJoy.joyAxes = &axes;
    newJoy.joyPOVs = &povs;
    newJoy.joyButtons = &buttons;
    newJoy.joyDescriptor = &desc;

    joysticks[joyNumber] = newJoy;
    memLock->unlock();
}

// ----- HAL Data: Update ----- //

void Mau_DriveData::updateControlWord(bool enabled, bool auton, bool test, bool estop, bool fms, bool ds) {
    memLock->lock();
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
    memLock->unlock();
}

void Mau_DriveData::updateJoyAxis(int joyNumber, int axisIndex, float axis) {
    memLock->lock();
    joysticks[joyNumber].joyAxes->axes[axisIndex] = axis;
    memLock->unlock();
}

void Mau_DriveData::updateJoyPOV(int joyNumber, int povIndex, uint16_t pov) {
    memLock->lock();
    joysticks[joyNumber].joyPOVs->povs[povIndex] = pov;
    memLock->unlock();
}

void Mau_DriveData::updateJoyButtons(int joyNumber, uint32_t buttons) {
    memLock->lock();
    joysticks[joyNumber].joyButtons->buttons = buttons;
    memLock->unlock();
}

// ----- HAL Data: Read ----- //

Mau_SharedJoystick Mau_DriveData::getJoystick(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return joysticks[joyNumber];
}

HAL_ControlWord* Mau_DriveData::readPreviousControl() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return prevControlWord;
}

HAL_ControlWord* Mau_DriveData::readCurrentControl() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    wordUnread == false;
    return currControlWord;
}

bool Mau_DriveData::readIfControlRead() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return wordUnread;
}

HAL_AllianceStationID* Mau_DriveData::readAllianceID() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return allianceID;
}

HAL_MatchInfo* Mau_DriveData::readMatchInfo() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return matchInfo;
}

HAL_MatchType* Mau_DriveData::readMatchType() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return &(matchInfo->matchType);
}

HAL_JoystickAxes* Mau_DriveData::readJoyAxes(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return getJoystick(joyNumber).joyAxes;
}

HAL_JoystickPOVs* Mau_DriveData::readJoyPOVs(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return getJoystick(joyNumber).joyPOVs;
}

HAL_JoystickButtons* Mau_DriveData::readJoyButtons(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return getJoystick(joyNumber).joyButtons;
}

HAL_JoystickDescriptor* Mau_DriveData::readJoyDescriptor(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return getJoystick(joyNumber).joyDescriptor;
}



