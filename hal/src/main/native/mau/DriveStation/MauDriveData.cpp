#include "MauDriveData.h"
#include <cstring>

wpi::priority_mutex Mau_DriveData::memLock;
wpi::priority_condition_variable Mau_DriveData::memSignal;

HAL_AllianceStationID Mau_DriveData::allianceID;
HAL_MatchInfo Mau_DriveData::matchInfo;
HAL_ControlWord Mau_DriveData::controlWord;
Mau_SharedJoystick Mau_DriveData::joysticks[6];

void Mau_DriveData::unlockAndSignal() {
    memSignal.notify_all();
    memLock.unlock();
}

void Mau_DriveData::updateAllianceID(HAL_AllianceStationID id) {
    memLock.lock();
    allianceID = id;
    unlockAndSignal();
}

void Mau_DriveData::updateMatchInfo(HAL_MatchInfo* info) {
    memLock.lock();
    matchInfo.eventName = info->eventName;
    matchInfo.matchType = info->matchType;
    matchInfo.matchNumber = info->matchNumber;
    matchInfo.replayNumber = info->replayNumber;
    matchInfo.gameSpecificMessage = info->gameSpecificMessage;
    unlockAndSignal();
}

void Mau_DriveData::updateMatchType(HAL_MatchType type) {
    memLock.lock();
    matchInfo.matchType = type;
    unlockAndSignal();
}

// --- Update: ControlWord --- //

void Mau_DriveData::updateIsEnabled(bool enabled) {
    memLock.lock();
    controlWord.enabled = enabled;
    unlockAndSignal();
}

void Mau_DriveData::updateIsAutonomous(bool auton) {
    memLock.lock();
    controlWord.autonomous = auton;
    unlockAndSignal();
}

void Mau_DriveData::updateIsTest(bool test) {
    memLock.lock();
    controlWord.test = test;
    unlockAndSignal();
}

void Mau_DriveData::updateEStop(bool eStop) {
    memLock.lock();
    controlWord.eStop = eStop;
    unlockAndSignal();
}

void Mau_DriveData::updateIsFmsAttached(bool fms) {
    memLock.lock();
    controlWord.fmsAttached = fms;
    unlockAndSignal();
}

void Mau_DriveData::updateIsDsAttached(bool ds) {
    memLock.lock();
    controlWord.dsAttached = ds;
    unlockAndSignal();
}

// --- Update: Joystick --- //

void Mau_DriveData::updateJoyAxis(int joyNumber, int16_t axisCount, uint8_t* axes) {
    memLock.lock();
    joysticks[joyNumber].joyAxes.count = axisCount;
    for (int index = 0; index < axisCount; index++) {
        joysticks[joyNumber].joyAxes.axes[index] = (float) axes[index];
    }
    unlockAndSignal();
}

void Mau_DriveData::updateJoyPOV(int joyNumber, int povsCount, uint16_t* povs) {
    memLock.lock();
    joysticks[joyNumber].joyPOVs.count = povsCount;
    for (int index = 0; index < povsCount; index++) {
        joysticks[joyNumber].joyPOVs.povs[index] = povs[index];
        index++;
    }
    unlockAndSignal();
}

void Mau_DriveData::updateJoyButtons(int joyNumber, uint8_t buttonCount, uint32_t buttons) {
    memLock.lock();
    joysticks[joyNumber].joyButtons.count = buttonCount;
    joysticks[joyNumber].joyButtons.buttons = buttons;
    unlockAndSignal();
}

void Mau_DriveData::updateJoyDescriptor(int joyNumber, HAL_JoystickDescriptor* desc) {
    memLock.lock();
    joysticks[joyNumber].joyDescriptor = *desc;
    unlockAndSignal();
}

void Mau_DriveData::updateJoyOutputs(int32_t joyNumber, int64_t outputs, int32_t leftRumble, int32_t rightRumble) {
//    memLock.lock();
//    Mau_SharedJoystick* joy = &joysticks[joyNumber];
//    joy->outputs = outputs;
//    joy->leftRumble = leftRumble;
//    joy->rightRumble = rightRumble;
//    unlockAndSignal();
}

//// ----- HAL Data: Scribe ----- ////

void Mau_DriveData::scribeMatchInfo(HAL_MatchInfo* info) {
    memLock.lock();
    std::strcpy(info->eventName, matchInfo.eventName);
    info->matchType = matchInfo.matchType;
    info->matchNumber = matchInfo.matchNumber;
    info->replayNumber = matchInfo.replayNumber;
    std::strcpy(info->gameSpecificMessage, matchInfo.gameSpecificMessage);
    memLock.unlock();
}

void Mau_DriveData::scribeJoyAxes(int joyNumber, HAL_JoystickAxes* axes) {
    memLock.lock();
    HAL_JoystickAxes* dataAxes = &joysticks[joyNumber].joyAxes;

    axes->count = dataAxes->count;
    std::memcpy(axes->axes, dataAxes->axes, HAL_kMaxJoystickAxes);
    memLock.unlock();
}

void Mau_DriveData::scribeJoyPOVs(int joyNumber, HAL_JoystickPOVs* povs) {
    memLock.lock();
    HAL_JoystickPOVs* dataPOVs = &joysticks[joyNumber].joyPOVs;

    povs->count = dataPOVs->count;
    std::memcpy(povs->povs, dataPOVs->povs, HAL_kMaxJoystickPOVs);
    memLock.unlock();
}

void Mau_DriveData::scribeJoyButtons(int joyNumber, HAL_JoystickButtons* buttons) {
    memLock.lock();
    HAL_JoystickButtons* dataButtons = &joysticks[joyNumber].joyButtons;

    buttons->count = dataButtons->count;
    buttons->buttons = dataButtons->buttons;
    memLock.unlock();
}

void Mau_DriveData::scribeJoyDescriptor(int joyNumber, HAL_JoystickDescriptor* desc) {
    memLock.lock();
    HAL_JoystickDescriptor* dataDesc = &joysticks[joyNumber].joyDescriptor;

    desc->isXbox = dataDesc->isXbox;
    desc->type = dataDesc->type;
    std::strcpy(desc->name, dataDesc->name);
    desc->axisCount = dataDesc->axisCount;
    std::memcpy(desc->axisTypes, dataDesc->axisTypes, HAL_kMaxJoystickAxes);
    desc->buttonCount = dataDesc->buttonCount;
    desc->povCount = dataDesc->povCount;
    memLock.unlock();
}

void Mau_DriveData::scribeJoyName(int joyNumber, char* name) {
    memLock.lock();
    std::strcpy(name, joysticks[joyNumber].joyDescriptor.name);
    memLock.unlock();
}

//// ----- HAL Data: Read ----- ////

HAL_ControlWord Mau_DriveData::readControlWord() {
    std::lock_guard<wpi::priority_mutex> lock(memLock);
    return controlWord;
}

HAL_AllianceStationID Mau_DriveData::readAllianceID() {
    std::lock_guard<wpi::priority_mutex> lock(memLock);
    return allianceID;
}

HAL_MatchType Mau_DriveData::readMatchType() {
    std::lock_guard<wpi::priority_mutex> lock(memLock);
    return matchInfo.matchType;
}

HAL_Bool Mau_DriveData::readJoyIsXbox(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(memLock);
    return joysticks[joyNumber].joyDescriptor.isXbox;
}

int32_t Mau_DriveData::readJoyType(int joyNumber) {
    std::lock_guard<wpi::priority_mutex> lock(memLock);
    return joysticks[joyNumber].joyDescriptor.type;
}

int32_t Mau_DriveData::readJoyAxisType(int joyNumber, int axisNumber) {
    std::lock_guard<wpi::priority_mutex> lock(memLock);
    return joysticks[joyNumber].joyDescriptor.axisTypes[axisNumber];
}

//// ----- HAL Data: Get ----- ////

wpi::priority_mutex* Mau_DriveData::getMutex() {
    return &memLock;
}

wpi::priority_condition_variable* Mau_DriveData::getDataSignal() {
    return &memSignal;
}
