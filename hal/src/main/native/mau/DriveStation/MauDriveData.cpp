#include "MauDriveData.h"


void Mau_DriveData::unlockAndSignal() {
    memSignal->notify_all();
    memLock->unlock();
}

//// ----- HAL Data: Create ----- ////

Mau_DriveData::Mau_DriveData() {
    wpi::priority_mutex newLock;
    memLock = &newLock;
    wpi::priority_condition_variable newSignal;
    memSignal = &newSignal;
}

//// ----- HAL Data: Write ----- ////

void Mau_DriveData::writeJoystick(int joyNumber, HAL_JoystickAxes axes, HAL_JoystickPOVs povs,
                                  HAL_JoystickButtons buttons, HAL_JoystickDescriptor desc) {
    memLock->lock();
    Mau_SharedJoystick newJoy;
    newJoy.joyAxes = &axes;
    newJoy.joyPOVs = &povs;
    newJoy.joyButtons = &buttons;
    newJoy.joyDescriptor = &desc;

    joysticks[joyNumber] = newJoy;
    unlockAndSignal();
}

//// ----- HAL Data: Update ----- ////

void Mau_DriveData::updateAllianceID(HAL_AllianceStationID id) {
    memLock->lock();
    allianceID = id;
    unlockAndSignal();
}

void Mau_DriveData::updateMatchInfo(HAL_MatchInfo* info) {
    memLock->lock();
    matchInfo = info;
    unlockAndSignal();
}

void Mau_DriveData::updateMatchType(HAL_MatchType* type) {
    memLock->lock();
    matchInfo->matchType = *type;
    unlockAndSignal();
}

// --- Update: ControlWord --- //

void Mau_DriveData::updateIsEnabled(bool enabled) {
    memLock->lock();
    controlWord->enabled = enabled;
    unlockAndSignal();
}

void Mau_DriveData::updateIsAutonomous(bool auton) {
    memLock->lock();
    controlWord->autonomous = auton;
    unlockAndSignal();
}

void Mau_DriveData::updateIsTest(bool test) {
    memLock->lock();
    controlWord->test = test;
    unlockAndSignal();
}

void Mau_DriveData::updateEStop(bool eStop) {
    memLock->lock();
    controlWord->eStop = eStop;
    unlockAndSignal();
}

void Mau_DriveData::updateIsFmsAttached(bool fms) {
    memLock->lock();
    controlWord->fmsAttached = fms;
    unlockAndSignal();
}

void Mau_DriveData::updateIsDsAttached(bool ds) {
    memLock->lock();
    controlWord->dsAttached = ds;
    unlockAndSignal();
}

void Mau_DriveData::updateControlWord(bool enabled, bool auton, bool test, bool estop, bool fms, bool ds) {
    updateIsEnabled(enabled);
    updateIsAutonomous(auton);
    updateIsTest(test);
    updateEStop(estop);
    updateIsFmsAttached(fms);
    updateIsDsAttached(ds);
}

// --- Update: Joystick --- //

void Mau_DriveData::updateJoyAxis(int joyNumber, int16_t axisCount, uint8_t* axes) {
    memLock->lock();
    joysticks[joyNumber].joyAxes->count = axisCount;
    for (int index = 0; index < axisCount; index++) {
        joysticks[joyNumber].joyAxes->axes[index] = (float) axes[index];
    }
    unlockAndSignal();
}

void Mau_DriveData::updateJoyPOV(int joyNumber, int povsCount, uint16_t* povs) {
    memLock->lock();
    joysticks[joyNumber].joyPOVs->count = povsCount;
    for (int index = 0; index < povsCount; index++) {
        joysticks[joyNumber].joyPOVs->povs[index] = povs[index];
        index++;
    }
    unlockAndSignal();
}

void Mau_DriveData::updateJoyButtons(int joyNumber, uint8_t buttonCount, uint32_t buttons) {
    memLock->lock();
    joysticks[joyNumber].joyButtons->count = buttonCount;
    joysticks[joyNumber].joyButtons->buttons = buttons;
    unlockAndSignal();
}

void Mau_DriveData::updateJoyDescriptor(int joyNumber, HAL_JoystickDescriptor* desc) {
    memLock->lock();
    joysticks[joyNumber].joyDescriptor = desc;
    unlockAndSignal();
}

void Mau_DriveData::updateJoyOutputs(int32_t joyNumber, int64_t outputs, int32_t leftRumble, int32_t rightRumble) {
    memLock->lock();
    Mau_SharedJoystick* joy = &joysticks[joyNumber];
    joy->outputs = outputs;
    joy->leftRumble = leftRumble;
    joy->rightRumble = rightRumble;
    unlockAndSignal();
}

//// ----- HAL Data: Read ----- ////

HAL_ControlWord* Mau_DriveData::readControlWord() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return controlWord;
}

HAL_AllianceStationID* Mau_DriveData::readAllianceID() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return &allianceID;
}

HAL_MatchInfo* Mau_DriveData::readMatchInfo() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return matchInfo;
}

HAL_MatchType* Mau_DriveData::readMatchType() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return &(matchInfo->matchType);
}

// --- Read: ControlWord --- //

bool Mau_DriveData::readIsEnabled() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return controlWord->enabled;
}

bool Mau_DriveData::readIsAutonomous() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return controlWord->autonomous;
}

bool Mau_DriveData::readIsTest() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return controlWord->test;
}

bool Mau_DriveData::readEStop() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return controlWord->eStop;
}

bool Mau_DriveData::readIsFmsAttached() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return controlWord->fmsAttached;
}

bool Mau_DriveData::readIsDsAttached() {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return controlWord->dsAttached;
}

// --- Read: Joystick --- //

Mau_SharedJoystick Mau_DriveData::getJoystick(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(*memLock);
    return joysticks[joyNumber];
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

//// ----- HAL Data: Get ----- ////

wpi::priority_mutex* Mau_DriveData::getMutex() {
    return memLock;
}

wpi::priority_condition_variable* Mau_DriveData::getDataSignal() {
    return memSignal;
}