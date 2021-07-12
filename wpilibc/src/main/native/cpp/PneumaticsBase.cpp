#include "frc/PneumaticsBase.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/SensorUtil.h"
#include "frc/Errors.h"

using namespace frc;

std::shared_ptr<PneumaticsBase> PneumaticsBase::GetForType(int module, PneumaticsModuleType moduleType) {
    if (moduleType == PneumaticsModuleType::CTREPCM) {
        return PneumaticsControlModule::GetForModule(module);
    }
    throw FRC_MakeError(err::InvalidParameter, "{}", moduleType);
}

int PneumaticsBase::GetDefaultForType(PneumaticsModuleType moduleType) {
    if (moduleType == PneumaticsModuleType::CTREPCM) {
        return SensorUtil::GetDefaultCTREPCMModule();
    }
    throw FRC_MakeError(err::InvalidParameter, "{}", moduleType);
}
