
#ifndef __LoadOut_h__
#define __LoadOut_h__

#define kMaxModuleNumber 2
namespace nLoadOut
{
    typedef enum {
        kModuleType_Unknown = 0x00,
        kModuleType_Analog = 0x01,
        kModuleType_Digital = 0x02,
        kModuleType_Solenoid = 0x03,
    } tModuleType;
    bool getModulePresence(tModuleType moduleType, uint8_t moduleNumber);
    typedef enum {
        kTargetClass_Unknown = 0x00,
        kTargetClass_FRC1 = 0x10,
        kTargetClass_FRC2 = 0x20,
        kTargetClass_FRC2_Analog = kTargetClass_FRC2 | kModuleType_Analog,
        kTargetClass_FRC2_Digital = kTargetClass_FRC2 | kModuleType_Digital,
        kTargetClass_FRC2_Solenoid = kTargetClass_FRC2 | kModuleType_Solenoid,
        kTargetClass_FamilyMask = 0xF0,
        kTargetClass_ModuleMask = 0x0F,
    } tTargetClass;
    tTargetClass getTargetClass();
}

#ifdef __cplusplus
extern "C" {
#endif

    uint32_t FRC_NetworkCommunication_nLoadOut_getModulePresence(uint32_t moduleType, uint8_t moduleNumber);
    uint32_t FRC_NetworkCommunication_nLoadOut_getTargetClass();

#ifdef __cplusplus
}
#endif

#endif // __LoadOut_h__
