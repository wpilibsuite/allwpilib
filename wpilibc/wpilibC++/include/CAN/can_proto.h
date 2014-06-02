//*****************************************************************************
//
// can_proto.h - Definitions for the CAN protocol used to communicate with the
//               BDC motor controller.
//
// Copyright (c) 2008 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#ifndef __CAN_PROTO_H__
#define __CAN_PROTO_H__

//*****************************************************************************
//
// The masks of the fields that are used in the message identifier.
//
//*****************************************************************************
#define CAN_MSGID_FULL_M        0x1fffffff
#define CAN_MSGID_DEVNO_M       0x0000003f
#define CAN_MSGID_API_M         0x0000ffc0
#define CAN_MSGID_MFR_M         0x00ff0000
#define CAN_MSGID_DTYPE_M       0x1f000000
#define CAN_MSGID_DEVNO_S       0
#define CAN_MSGID_API_S         6
#define CAN_MSGID_MFR_S         16
#define CAN_MSGID_DTYPE_S       24

//*****************************************************************************
//
// The Reserved device number values in the Message Id.
//
//*****************************************************************************
#define CAN_MSGID_DEVNO_BCAST   0x00000000

//*****************************************************************************
//
// The Reserved system control API numbers in the Message Id.
//
//*****************************************************************************
#define CAN_MSGID_API_SYSHALT   0x00000000
#define CAN_MSGID_API_SYSRST    0x00000040
#define CAN_MSGID_API_DEVASSIGN 0x00000080
#define CAN_MSGID_API_DEVQUERY  0x000000c0
#define CAN_MSGID_API_HEARTBEAT 0x00000140
#define CAN_MSGID_API_SYNC      0x00000180
#define CAN_MSGID_API_UPDATE    0x000001c0
#define CAN_MSGID_API_FIRMVER   0x00000200
#define CAN_MSGID_API_ENUMERATE 0x00000240
#define CAN_MSGID_API_SYSRESUME 0x00000280

//*****************************************************************************
//
// The 32 bit values associated with the CAN_MSGID_API_STATUS request.
//
//*****************************************************************************
#define CAN_STATUS_CODE_M       0x0000ffff
#define CAN_STATUS_MFG_M        0x00ff0000
#define CAN_STATUS_DTYPE_M      0x1f000000
#define CAN_STATUS_CODE_S       0
#define CAN_STATUS_MFG_S        16
#define CAN_STATUS_DTYPE_S      24

//*****************************************************************************
//
// The Reserved manufacturer identifiers in the Message Id.
//
//*****************************************************************************
#define CAN_MSGID_MFR_NI        0x00010000
#define CAN_MSGID_MFR_LM        0x00020000
#define CAN_MSGID_MFR_DEKA      0x00030000

//*****************************************************************************
//
// The Reserved device type identifiers in the Message Id.
//
//*****************************************************************************
#define CAN_MSGID_DTYPE_BCAST   0x00000000
#define CAN_MSGID_DTYPE_ROBOT   0x01000000
#define CAN_MSGID_DTYPE_MOTOR   0x02000000
#define CAN_MSGID_DTYPE_RELAY   0x03000000
#define CAN_MSGID_DTYPE_GYRO    0x04000000
#define CAN_MSGID_DTYPE_ACCEL   0x05000000
#define CAN_MSGID_DTYPE_USONIC  0x06000000
#define CAN_MSGID_DTYPE_GEART   0x07000000
#define CAN_MSGID_DTYPE_UPDATE  0x1f000000

//*****************************************************************************
//
// LM Motor Control API Classes API Class and ID masks.
//
//*****************************************************************************
#define CAN_MSGID_API_CLASS_M   0x0000fc00
#define CAN_MSGID_API_ID_M      0x000003c0

//*****************************************************************************
//
// LM Motor Control API Classes in the Message Id for non-broadcast.
// These are the upper 6 bits of the API field, the lower 4 bits determine
// the APIId.
//
//*****************************************************************************
#define CAN_API_MC_VOLTAGE      0x00000000
#define CAN_API_MC_SPD          0x00000400
#define CAN_API_MC_VCOMP        0x00000800
#define CAN_API_MC_POS          0x00000c00
#define CAN_API_MC_ICTRL        0x00001000
#define CAN_API_MC_STATUS       0x00001400
#define CAN_API_MC_PSTAT        0x00001800
#define CAN_API_MC_CFG          0x00001c00
#define CAN_API_MC_ACK          0x00002000

//*****************************************************************************
//
// The Stellaris Motor Class Control Voltage API definitions.
//
//*****************************************************************************
#define LM_API_VOLT             (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_VOLTAGE)
#define LM_API_VOLT_EN          (LM_API_VOLT | (0 << CAN_MSGID_API_S))
#define LM_API_VOLT_DIS         (LM_API_VOLT | (1 << CAN_MSGID_API_S))
#define LM_API_VOLT_SET         (LM_API_VOLT | (2 << CAN_MSGID_API_S))
#define LM_API_VOLT_SET_RAMP    (LM_API_VOLT | (3 << CAN_MSGID_API_S))
//##### FIRST BEGIN #####
#define LM_API_VOLT_T_EN        (LM_API_VOLT | (4 << CAN_MSGID_API_S))
#define LM_API_VOLT_T_SET       (LM_API_VOLT | (5 << CAN_MSGID_API_S))
#define LM_API_VOLT_T_SET_NO_ACK                                              \
                                (LM_API_VOLT | (7 << CAN_MSGID_API_S))
//##### FIRST END #####
#define LM_API_VOLT_SET_NO_ACK  (LM_API_VOLT | (8 << CAN_MSGID_API_S))

//*****************************************************************************
//
// The Stellaris Motor Class Control API definitions for LM_API_VOLT_SET_RAMP.
//
//*****************************************************************************
#define LM_API_VOLT_RAMP_DIS    0

//*****************************************************************************
//
// The Stellaris Motor Class Control API definitions for CAN_MSGID_API_SYNC.
//
//*****************************************************************************
#define LM_API_SYNC_PEND_NOW    0

//*****************************************************************************
//
// The Stellaris Motor Class Speed Control API definitions.
//
//*****************************************************************************
#define LM_API_SPD              (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_SPD)
#define LM_API_SPD_EN           (LM_API_SPD | (0 << CAN_MSGID_API_S))
#define LM_API_SPD_DIS          (LM_API_SPD | (1 << CAN_MSGID_API_S))
#define LM_API_SPD_SET          (LM_API_SPD | (2 << CAN_MSGID_API_S))
#define LM_API_SPD_PC           (LM_API_SPD | (3 << CAN_MSGID_API_S))
#define LM_API_SPD_IC           (LM_API_SPD | (4 << CAN_MSGID_API_S))
#define LM_API_SPD_DC           (LM_API_SPD | (5 << CAN_MSGID_API_S))
#define LM_API_SPD_REF          (LM_API_SPD | (6 << CAN_MSGID_API_S))
//##### FIRST BEGIN #####
#define LM_API_SPD_T_EN         (LM_API_SPD | (7 << CAN_MSGID_API_S))
#define LM_API_SPD_T_SET        (LM_API_SPD | (8 << CAN_MSGID_API_S))
#define LM_API_SPD_T_SET_NO_ACK (LM_API_SPD | (10 << CAN_MSGID_API_S))
//##### FIRST END #####
#define LM_API_SPD_SET_NO_ACK   (LM_API_SPD | (11 << CAN_MSGID_API_S))

//*****************************************************************************
//
// The Stellaris Motor Control Voltage Compensation Control API definitions.
//
//*****************************************************************************
#define LM_API_VCOMP            (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_VCOMP)
#define LM_API_VCOMP_EN         (LM_API_VCOMP | (0 << CAN_MSGID_API_S))
#define LM_API_VCOMP_DIS        (LM_API_VCOMP | (1 << CAN_MSGID_API_S))
#define LM_API_VCOMP_SET        (LM_API_VCOMP | (2 << CAN_MSGID_API_S))
#define LM_API_VCOMP_IN_RAMP    (LM_API_VCOMP | (3 << CAN_MSGID_API_S))
#define LM_API_VCOMP_COMP_RAMP  (LM_API_VCOMP | (4 << CAN_MSGID_API_S))
//##### FIRST BEGIN #####
#define LM_API_VCOMP_T_EN       (LM_API_VCOMP | (5 << CAN_MSGID_API_S))
#define LM_API_VCOMP_T_SET      (LM_API_VCOMP | (6 << CAN_MSGID_API_S))
#define LM_API_VCOMP_T_SET_NO_ACK                                             \
                                (LM_API_VCOMP | (8 << CAN_MSGID_API_S))
//##### FIRST END #####
#define LM_API_VCOMP_SET_NO_ACK (LM_API_VCOMP | (9 << CAN_MSGID_API_S))

//*****************************************************************************
//
// The Stellaris Motor Class Position Control API definitions.
//
//*****************************************************************************
#define LM_API_POS              (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_POS)
#define LM_API_POS_EN           (LM_API_POS | (0 << CAN_MSGID_API_S))
#define LM_API_POS_DIS          (LM_API_POS | (1 << CAN_MSGID_API_S))
#define LM_API_POS_SET          (LM_API_POS | (2 << CAN_MSGID_API_S))
#define LM_API_POS_PC           (LM_API_POS | (3 << CAN_MSGID_API_S))
#define LM_API_POS_IC           (LM_API_POS | (4 << CAN_MSGID_API_S))
#define LM_API_POS_DC           (LM_API_POS | (5 << CAN_MSGID_API_S))
#define LM_API_POS_REF          (LM_API_POS | (6 << CAN_MSGID_API_S))
//##### FIRST BEGIN #####
#define LM_API_POS_T_EN         (LM_API_POS | (7 << CAN_MSGID_API_S))
#define LM_API_POS_T_SET        (LM_API_POS | (8 << CAN_MSGID_API_S))
#define LM_API_POS_T_SET_NO_ACK (LM_API_POS | (10 << CAN_MSGID_API_S))
//##### FIRST END #####
#define LM_API_POS_SET_NO_ACK   (LM_API_POS | (11 << CAN_MSGID_API_S))

//*****************************************************************************
//
// The Stellaris Motor Class Current Control API definitions.
//
//*****************************************************************************
#define LM_API_ICTRL            (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_ICTRL)
#define LM_API_ICTRL_EN         (LM_API_ICTRL | (0 << CAN_MSGID_API_S))
#define LM_API_ICTRL_DIS        (LM_API_ICTRL | (1 << CAN_MSGID_API_S))
#define LM_API_ICTRL_SET        (LM_API_ICTRL | (2 << CAN_MSGID_API_S))
#define LM_API_ICTRL_PC         (LM_API_ICTRL | (3 << CAN_MSGID_API_S))
#define LM_API_ICTRL_IC         (LM_API_ICTRL | (4 << CAN_MSGID_API_S))
#define LM_API_ICTRL_DC         (LM_API_ICTRL | (5 << CAN_MSGID_API_S))
//##### FIRST BEGIN #####
#define LM_API_ICTRL_T_EN       (LM_API_ICTRL | (6 << CAN_MSGID_API_S))
#define LM_API_ICTRL_T_SET      (LM_API_ICTRL | (7 << CAN_MSGID_API_S))
#define LM_API_ICTRL_T_SET_NO_ACK                                             \
                                (LM_API_ICTRL | (9 << CAN_MSGID_API_S))
//##### FIRST END #####
#define LM_API_ICTRL_SET_NO_ACK (LM_API_ICTRL | (10 << CAN_MSGID_API_S))

//*****************************************************************************
//
// The Stellaris Motor Class Firmware Update API definitions.
//
//*****************************************************************************
#define LM_API_UPD              (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_UPDATE)
#define LM_API_UPD_PING         (LM_API_UPD | (0 << CAN_MSGID_API_S))
#define LM_API_UPD_DOWNLOAD     (LM_API_UPD | (1 << CAN_MSGID_API_S))
#define LM_API_UPD_SEND_DATA    (LM_API_UPD | (2 << CAN_MSGID_API_S))
#define LM_API_UPD_RESET        (LM_API_UPD | (3 << CAN_MSGID_API_S))
#define LM_API_UPD_ACK          (LM_API_UPD | (4 << CAN_MSGID_API_S))
#define LM_API_HWVER            (LM_API_UPD | (5 << CAN_MSGID_API_S))
#define LM_API_UPD_REQUEST      (LM_API_UPD | (6 << CAN_MSGID_API_S))
//##### FIRST BEGIN #####
#define LM_API_UNTRUST_EN       (LM_API_UPD | (11 << CAN_MSGID_API_S))
#define LM_API_TRUST_EN         (LM_API_UPD | (12 << CAN_MSGID_API_S))
#define LM_API_TRUST_HEARTBEAT  (LM_API_UPD | (13 << CAN_MSGID_API_S))
//##### FIRST END #####

//*****************************************************************************
//
// The Stellaris Motor Class Status API definitions.
//
//*****************************************************************************
#define LM_API_STATUS           (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_STATUS)
#define LM_API_STATUS_VOLTOUT   (LM_API_STATUS | (0 << CAN_MSGID_API_S))
#define LM_API_STATUS_VOLTBUS   (LM_API_STATUS | (1 << CAN_MSGID_API_S))
#define LM_API_STATUS_CURRENT   (LM_API_STATUS | (2 << CAN_MSGID_API_S))
#define LM_API_STATUS_TEMP      (LM_API_STATUS | (3 << CAN_MSGID_API_S))
#define LM_API_STATUS_POS       (LM_API_STATUS | (4 << CAN_MSGID_API_S))
#define LM_API_STATUS_SPD       (LM_API_STATUS | (5 << CAN_MSGID_API_S))
#define LM_API_STATUS_LIMIT     (LM_API_STATUS | (6 << CAN_MSGID_API_S))
#define LM_API_STATUS_FAULT     (LM_API_STATUS | (7 << CAN_MSGID_API_S))
#define LM_API_STATUS_POWER     (LM_API_STATUS | (8 << CAN_MSGID_API_S))
#define LM_API_STATUS_CMODE     (LM_API_STATUS | (9 << CAN_MSGID_API_S))
#define LM_API_STATUS_VOUT      (LM_API_STATUS | (10 << CAN_MSGID_API_S))
#define LM_API_STATUS_STKY_FLT  (LM_API_STATUS | (11 << CAN_MSGID_API_S))
#define LM_API_STATUS_FLT_COUNT (LM_API_STATUS | (12 << CAN_MSGID_API_S))

//*****************************************************************************
//
// These definitions are used with the byte that is returned from
// the status request for LM_API_STATUS_LIMIT.
//
//*****************************************************************************
#define LM_STATUS_LIMIT_FWD     0x01
#define LM_STATUS_LIMIT_REV     0x02
#define LM_STATUS_LIMIT_SFWD    0x04
#define LM_STATUS_LIMIT_SREV    0x08
#define LM_STATUS_LIMIT_STKY_FWD                                              \
                                0x10
#define LM_STATUS_LIMIT_STKY_REV                                              \
                                0x20
#define LM_STATUS_LIMIT_STKY_SFWD                                             \
                                0x40
#define LM_STATUS_LIMIT_STKY_SREV                                             \
                                0x80

//*****************************************************************************
//
// LM Motor Control status codes returned due to the CAN_STATUS_CODE_M field.
//
//*****************************************************************************
#define LM_STATUS_FAULT_ILIMIT  0x01
#define LM_STATUS_FAULT_TLIMIT  0x02
#define LM_STATUS_FAULT_VLIMIT  0x04

//*****************************************************************************
//
// The Stellaris Motor Class Configuration API definitions.
//
//*****************************************************************************
#define LM_API_CFG              (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_CFG)
#define LM_API_CFG_NUM_BRUSHES  (LM_API_CFG | (0 << CAN_MSGID_API_S))
#define LM_API_CFG_ENC_LINES    (LM_API_CFG | (1 << CAN_MSGID_API_S))
#define LM_API_CFG_POT_TURNS    (LM_API_CFG | (2 << CAN_MSGID_API_S))
#define LM_API_CFG_BRAKE_COAST  (LM_API_CFG | (3 << CAN_MSGID_API_S))
#define LM_API_CFG_LIMIT_MODE   (LM_API_CFG | (4 << CAN_MSGID_API_S))
#define LM_API_CFG_LIMIT_FWD    (LM_API_CFG | (5 << CAN_MSGID_API_S))
#define LM_API_CFG_LIMIT_REV    (LM_API_CFG | (6 << CAN_MSGID_API_S))
#define LM_API_CFG_MAX_VOUT     (LM_API_CFG | (7 << CAN_MSGID_API_S))
#define LM_API_CFG_FAULT_TIME   (LM_API_CFG | (8 << CAN_MSGID_API_S))

//*****************************************************************************
//
// The Stellaris ACK API definition.
//
//*****************************************************************************
#define LM_API_ACK              (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_ACK)

//*****************************************************************************
//
// The 8 bit values that can be returned by a call to LM_API_STATUS_HWVER.
//
//*****************************************************************************
#define LM_HWVER_UNKNOWN        0x00
#define LM_HWVER_JAG_1_0        0x01
#define LM_HWVER_JAG_2_0        0x02

//*****************************************************************************
//
// The 8 bit values that can be returned by a call to LM_API_STATUS_CMODE.
//
//*****************************************************************************
#define LM_STATUS_CMODE_VOLT    0x00
#define LM_STATUS_CMODE_CURRENT 0x01
#define LM_STATUS_CMODE_SPEED   0x02
#define LM_STATUS_CMODE_POS     0x03
#define LM_STATUS_CMODE_VCOMP   0x04

//*****************************************************************************
//
// The values that can specified as the position or speed reference.  Not all
// values are valid for each reference; if an invalid reference is set, then
// none will be selected.
//
//*****************************************************************************
#define LM_REF_ENCODER          0x00
#define LM_REF_POT              0x01
#define LM_REF_INV_ENCODER      0x02
#define LM_REF_QUAD_ENCODER     0x03
#define LM_REF_NONE             0xff

//*****************************************************************************
//
// The flags that are used to indicate the currently active fault sources.
//
//*****************************************************************************
#define LM_FAULT_CURRENT        0x01
#define LM_FAULT_TEMP           0x02
#define LM_FAULT_VBUS           0x04
#define LM_FAULT_GATE_DRIVE     0x08
#define LM_FAULT_COMM           0x10

//*****************************************************************************
//
// The Stellaris Motor Class Periodic Status API definitions.
//
//*****************************************************************************
#define LM_API_PSTAT            (CAN_MSGID_MFR_LM | CAN_MSGID_DTYPE_MOTOR |   \
                                 CAN_API_MC_PSTAT)
#define LM_API_PSTAT_PER_EN_S0  (LM_API_PSTAT | (0 << CAN_MSGID_API_S))
#define LM_API_PSTAT_PER_EN_S1  (LM_API_PSTAT | (1 << CAN_MSGID_API_S))
#define LM_API_PSTAT_PER_EN_S2  (LM_API_PSTAT | (2 << CAN_MSGID_API_S))
#define LM_API_PSTAT_PER_EN_S3  (LM_API_PSTAT | (3 << CAN_MSGID_API_S))
#define LM_API_PSTAT_CFG_S0     (LM_API_PSTAT | (4 << CAN_MSGID_API_S))
#define LM_API_PSTAT_CFG_S1     (LM_API_PSTAT | (5 << CAN_MSGID_API_S))
#define LM_API_PSTAT_CFG_S2     (LM_API_PSTAT | (6 << CAN_MSGID_API_S))
#define LM_API_PSTAT_CFG_S3     (LM_API_PSTAT | (7 << CAN_MSGID_API_S))
#define LM_API_PSTAT_DATA_S0    (LM_API_PSTAT | (8 << CAN_MSGID_API_S))
#define LM_API_PSTAT_DATA_S1    (LM_API_PSTAT | (9 << CAN_MSGID_API_S))
#define LM_API_PSTAT_DATA_S2    (LM_API_PSTAT | (10 << CAN_MSGID_API_S))
#define LM_API_PSTAT_DATA_S3    (LM_API_PSTAT | (11 << CAN_MSGID_API_S))

//*****************************************************************************
//
// The values that can be used to configure the data the Periodic Status
// Message bytes.  Bytes of a multi-byte data values are encoded as
// little-endian, therefore B0 is the least significant byte.
//
//*****************************************************************************
#define LM_PSTAT_END            0
#define LM_PSTAT_VOLTOUT_B0     1
#define LM_PSTAT_VOLTOUT_B1     2
#define LM_PSTAT_VOLTBUS_B0     3
#define LM_PSTAT_VOLTBUS_B1     4
#define LM_PSTAT_CURRENT_B0     5
#define LM_PSTAT_CURRENT_B1     6
#define LM_PSTAT_TEMP_B0        7
#define LM_PSTAT_TEMP_B1        8
#define LM_PSTAT_POS_B0         9
#define LM_PSTAT_POS_B1         10
#define LM_PSTAT_POS_B2         11
#define LM_PSTAT_POS_B3         12
#define LM_PSTAT_SPD_B0         13
#define LM_PSTAT_SPD_B1         14
#define LM_PSTAT_SPD_B2         15
#define LM_PSTAT_SPD_B3         16
#define LM_PSTAT_LIMIT_NCLR     17
#define LM_PSTAT_LIMIT_CLR      18
#define LM_PSTAT_FAULT          19
#define LM_PSTAT_STKY_FLT_NCLR  20
#define LM_PSTAT_STKY_FLT_CLR   21
#define LM_PSTAT_VOUT_B0        22
#define LM_PSTAT_VOUT_B1        23
#define LM_PSTAT_FLT_COUNT_CURRENT \
                                24
#define LM_PSTAT_FLT_COUNT_TEMP 25
#define LM_PSTAT_FLT_COUNT_VOLTBUS \
                                26
#define LM_PSTAT_FLT_COUNT_GATE 27
#define LM_PSTAT_FLT_COUNT_COMM 28
#define LM_PSTAT_CANSTS         29
#define LM_PSTAT_CANERR_B0      30
#define LM_PSTAT_CANERR_B1      31

#endif // __CAN_PROTO_H__
