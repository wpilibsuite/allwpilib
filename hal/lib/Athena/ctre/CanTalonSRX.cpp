/**
 * @brief CAN TALON SRX driver.
 *
 * The TALON SRX is designed to instrument all runtime signals periodically.
 * The default periods are chosen to support 16 TALONs with 10ms update rate
 * for control (throttle or setpoint).  However these can be overridden with
 * SetStatusFrameRate. @see SetStatusFrameRate
 * The getters for these unsolicited signals are auto generated at the bottom
 * of this module.
 *
 * Likewise most control signals are sent periodically using the fire-and-forget
 * CAN API.  The setters for these unsolicited signals are auto generated at the
 * bottom of this module.
 *
 * Signals that are not available in an unsolicited fashion are the Close Loop
 * gains.  For teams that have a single profile for their TALON close loop they
 * can use either the webpage to configure their TALONs once or set the PIDF,
 * Izone, CloseLoopRampRate, etc... once in the robot application.  These
 * parameters are saved to flash so once they are loaded in the TALON, they
 * will persist through power cycles and mode changes.
 *
 * For teams that have one or two profiles to switch between, they can use the
 * same strategy since there are two slots to choose from and the
 * ProfileSlotSelect is periodically sent in the 10 ms control frame.
 *
 * For teams that require changing gains frequently, they can use the soliciting
 * API to get and set those parameters.  Most likely they will only need to set
 * them in a periodic fashion as a function of what motion the application is
 * attempting.  If this API is used, be mindful of the CAN utilization reported
 * in the driver station.
 *
 * If calling application has used the config routines to configure the
 * selected feedback sensor, then all positions are measured in floating point
 * precision rotations.  All sensor velocities are specified in floating point
 * precision RPM.
 * @see ConfigPotentiometerTurns
 * @see ConfigEncoderCodesPerRev
 * HOWEVER, if calling application has not called the config routine for
 * selected feedback sensor, then all getters/setters for position/velocity use
 * the native engineering units of the Talon SRX firm (just like in 2015).
 * Signals explained below.
 *
 * Encoder position is measured in encoder edges.  Every edge is counted
 * (similar to roboRIO 4X mode).  Analog position is 10 bits, meaning 1024
 * ticks per rotation (0V => 3.3V).  Use SetFeedbackDeviceSelect to select
 * which sensor type you need.  Once you do that you can use GetSensorPosition()
 * and GetSensorVelocity().  These signals are updated on CANBus every 20ms (by
 * default).  If a relative sensor is selected, you can zero (or change the
 * current value) using SetSensorPosition.
 *
 * Analog Input and quadrature position (and velocity) are also explicitly
 * reported in GetEncPosition, GetEncVel, GetAnalogInWithOv, GetAnalogInVel.
 * These signals are available all the time, regardless of what sensor is
 * selected at a rate of 100ms.  This allows easy instrumentation for "in the
 * pits" checking of all sensors regardless of modeselect.  The 100ms rate is
 * overridable for teams who want to acquire sensor data for processing, not
 * just instrumentation.  Or just select the sensor using
 * SetFeedbackDeviceSelect to get it at 20ms.
 *
 * Velocity is in position ticks / 100ms.
 *
 * All output units are in respect to duty cycle (throttle) which is -1023(full
 * reverse) to +1023 (full forward).  This includes demand (which specifies
 * duty cycle when in duty cycle mode) and rampRamp, which is in throttle units
 * per 10ms (if nonzero).
 *
 * Pos and velocity close loops are calc'd as
 *   err = target - posOrVel.
 *   iErr += err;
 *   if(   (IZone!=0)  and  abs(err) > IZone)
 *       ClearIaccum()
 *   output = P X err + I X iErr + D X dErr + F X target
 *   dErr = err - lastErr
 * P, I, and D gains are always positive. F can be negative.
 * Motor direction can be reversed using SetRevMotDuringCloseLoopEn if
 * sensor and motor are out of phase. Similarly feedback sensor can also be
 * reversed (multiplied by -1) if you prefer the sensor to be inverted.
 *
 * P gain is specified in throttle per error tick.  For example, a value of 102
 * is ~9.9% (which is 102/1023) throttle per 1 ADC unit(10bit) or 1 quadrature
 * encoder edge depending on selected sensor.
 *
 * I gain is specified in throttle per integrated error. For example, a value
 * of 10 equates to ~0.99% (which is 10/1023) for each accumulated ADC unit
 * (10 bit) or 1 quadrature encoder edge depending on selected sensor.
 * Close loop and integral accumulator runs every 1ms.
 *
 * D gain is specified in throttle per derivative error. For example a value of
 * 102 equates to ~9.9% (which is 102/1023) per change of 1 unit (ADC or
 * encoder) per ms.
 *
 * I Zone is specified in the same units as sensor position (ADC units or
 * quadrature edges).  If pos/vel error is outside of this value, the
 * integrated error will auto-clear...
 *   if(   (IZone!=0)  and  abs(err) > IZone)
 *       ClearIaccum()
 * ...this is very useful in preventing integral windup and is highly
 * recommended if using full PID to keep stability low.
 *
 * CloseLoopRampRate is in throttle units per 1ms.  Set to zero to disable
 * ramping.  Works the same as RampThrottle but only is in effect when a close
 * loop mode and profile slot is selected.
 *
 * auto generated using spreadsheet and wpiclassgen.py
 * @link https://docs.google.com/spreadsheets/d/1OU_ZV7fZLGYUQ-Uhc8sVAmUmWTlT8XBFYK8lfjg_tac/edit#gid=1766046967
 */
#include "HAL/CanTalonSRX.h"
#include "FRC_NetworkCommunication/CANSessionMux.h"  //CAN Comm
#include <string.h>                                  // memset
#include <unistd.h>                                  // usleep

#define STATUS_1 0x02041400
#define STATUS_2 0x02041440
#define STATUS_3 0x02041480
#define STATUS_4 0x020414C0
#define STATUS_5 0x02041500
#define STATUS_6 0x02041540
#define STATUS_7 0x02041580
#define STATUS_8 0x020415C0
#define STATUS_9 0x02041600

#define CONTROL_1 0x02040000
#define CONTROL_2 0x02040040
#define CONTROL_3 0x02040080
#define CONTROL_5 0x02040100
#define CONTROL_6 0x02040140

#define EXPECTED_RESPONSE_TIMEOUT_MS (200)
#define GET_STATUS1()                                                    \
  CtreCanNode::recMsg<TALON_Status_1_General_10ms_t> rx =                \
      GetRx<TALON_Status_1_General_10ms_t>(STATUS_1 | GetDeviceNumber(), \
                                           EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS2()                                                     \
  CtreCanNode::recMsg<TALON_Status_2_Feedback_20ms_t> rx =                \
      GetRx<TALON_Status_2_Feedback_20ms_t>(STATUS_2 | GetDeviceNumber(), \
                                            EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS3()                                                 \
  CtreCanNode::recMsg<TALON_Status_3_Enc_100ms_t> rx =                \
      GetRx<TALON_Status_3_Enc_100ms_t>(STATUS_3 | GetDeviceNumber(), \
                                        EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS4()                                                         \
  CtreCanNode::recMsg<TALON_Status_4_AinTempVbat_100ms_t> rx =                \
      GetRx<TALON_Status_4_AinTempVbat_100ms_t>(STATUS_4 | GetDeviceNumber(), \
                                                EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS5()                                                       \
  CtreCanNode::recMsg<TALON_Status_5_Startup_OneShot_t> rx =                \
      GetRx<TALON_Status_5_Startup_OneShot_t>(STATUS_5 | GetDeviceNumber(), \
                                              EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS6()                                                         \
  CtreCanNode::recMsg<TALON_Status_6_Eol_t> rx = GetRx<TALON_Status_6_Eol_t>( \
      STATUS_6 | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS7()                                                   \
  CtreCanNode::recMsg<TALON_Status_7_Debug_200ms_t> rx =                \
      GetRx<TALON_Status_7_Debug_200ms_t>(STATUS_7 | GetDeviceNumber(), \
                                          EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS8()                                                      \
  CtreCanNode::recMsg<TALON_Status_8_PulseWid_100ms_t> rx =                \
      GetRx<TALON_Status_8_PulseWid_100ms_t>(STATUS_8 | GetDeviceNumber(), \
                                             EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS9()                                            \
  CtreCanNode::recMsg<TALON_Status_9_MotProfBuffer_100ms_t> rx = \
      GetRx<TALON_Status_9_MotProfBuffer_100ms_t>(               \
          STATUS_9 | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)

#define PARAM_REQUEST 0x02041800
#define PARAM_RESPONSE 0x02041840
#define PARAM_SET 0x02041880

const int kParamArbIdValue = PARAM_RESPONSE;
const int kParamArbIdMask = 0xFFFFFFFF;

const double FLOAT_TO_FXP_10_22 = (double)0x400000;
const double FXP_TO_FLOAT_10_22 = 0.0000002384185791015625;

const double FLOAT_TO_FXP_0_8 = (double)0x100;
const double FXP_TO_FLOAT_0_8 = 0.00390625;

CanTalonSRX::CanTalonSRX(int deviceNumber, int controlPeriodMs,
                         int enablePeriodMs)
    : CtreCanNode(deviceNumber), _can_h(0), _can_stat(0) {
  _controlPeriodMs = controlPeriodMs;
  _enablePeriodMs = enablePeriodMs;

  /* bound period to be within [1 ms,95 ms] */
  if (_controlPeriodMs < 1)
    _controlPeriodMs = 1;
  else if (_controlPeriodMs > 95)
    _controlPeriodMs = 95;
  if (_enablePeriodMs < 1)
    _enablePeriodMs = 1;
  else if (_enablePeriodMs > 95)
    _enablePeriodMs = 95;

  RegisterRx(STATUS_1 | (UINT8)deviceNumber);
  RegisterRx(STATUS_2 | (UINT8)deviceNumber);
  RegisterRx(STATUS_3 | (UINT8)deviceNumber);
  RegisterRx(STATUS_4 | (UINT8)deviceNumber);
  RegisterRx(STATUS_5 | (UINT8)deviceNumber);
  RegisterRx(STATUS_6 | (UINT8)deviceNumber);
  RegisterRx(STATUS_7 | (UINT8)deviceNumber);
  /* use the legacy command frame until we have evidence we can use the new
   * frame.
   */
  RegisterTx(CONTROL_1 | (UINT8)deviceNumber, (UINT8)_controlPeriodMs);
  _controlFrameArbId = CONTROL_1;
  /* the only default param that is nonzero is limit switch.
   * Default to using the flash settings.
   */
  SetOverrideLimitSwitchEn(kLimitSwitchOverride_UseDefaultsFromFlash);
  /* Check if we can upgrade the control framing */
  UpdateControlId();
}
/* CanTalonSRX D'tor
 */
CanTalonSRX::~CanTalonSRX() {
  if (m_hasBeenMoved) {
    /* Another CANTalonSRX still exists, so don't un-register the periodic
     * control frame
     */
  } else {
    /* un-register the control frame so Talon is disabled */
    RegisterTx(CONTROL_1 | (UINT8)GetDeviceNumber(), 0);
    RegisterTx(CONTROL_5 | (UINT8)GetDeviceNumber(), 0);
  }
  /* free the stream we used for SetParam/GetParamResponse */
  if (_can_h) {
    FRC_NetworkCommunication_CANSessionMux_closeStreamSession(_can_h);
    _can_h = 0;
  }
}
/**
 * @return true if Talon is reporting that it supports control5, and therefore
 *         RIO can send control5 to update control params (even when disabled).
 */
bool CanTalonSRX::IsControl5Supported() {
  /* only bother to poll status2 if we are looking for cmd5allowed */
  GET_STATUS2();
  if (rx.err != CTR_OKAY) {
    /* haven't received it */
    return false;
  } else if (0 == rx->Cmd5Allowed) {
    /* firmware doesn't support it */
    return false;
  }
  /* we can use control5, this gives application the ability to set control
   * params prior to Talon-enable */
  return true;
}
/**
 * Get a copy of the control frame to send.
 * @param [out] pointer to eight byte array to fill.
 */
void CanTalonSRX::GetControlFrameCopy(uint8_t *toFill) {
  /* get the copy of the control frame in control1 */
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> task =
      GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId |
                                            GetDeviceNumber());
  /* control1's payload will move to 5, but update the new sigs in control5 */
  if (task.IsEmpty())
    memset(toFill, 0, 8);
  else
    memcpy(toFill, task.toSend, 8);
  /* zero first two bytes - these are reserved. */
  toFill[0] = 0;
  toFill[1] = 0;
}
/**
 * Called in various places to double check we are using the best control frame.
 * If the Talon firmware is too old, use control 1 framing, which does not allow
 * setting
 * control signals until robot is enabled.  If Talon firmware can suport
 * control5, use that
 * since that frame can be transmitted during robot-disable.  If calling
 * application
 * uses setParam to set the signal eLegacyControlMode, caller can force using
 * control1
 * if needed for some reason.
 */
void CanTalonSRX::UpdateControlId() {
  uint8_t work[8];
  uint32_t frameToUse;
  /* deduce if we should change IDs.  If firm supports the new frame, and
   * calling app isn't forcing legacy mode
   * use control5.*/
  if (_useControl5ifSupported && IsControl5Supported()) {
    frameToUse = CONTROL_5;
  } else {
    frameToUse = CONTROL_1;
  }
  /* is there anything to do */
  if (frameToUse == _controlFrameArbId) {
    /* nothing to do, we are using the best frame. */
  } else if (frameToUse == CONTROL_5) {
    /* get a copy of the control frame */
    GetControlFrameCopy(work);
    /* Change control1's DLC to 2.  Passing nullptr means all payload bytes are
     * zero. */
    RegisterTx(CONTROL_1 | GetDeviceNumber(), _enablePeriodMs, 2, nullptr);
    /* reregister the control frame using the new ID */
    RegisterTx(frameToUse | GetDeviceNumber(), _controlPeriodMs, 8, work);
    /* save the correct frame ArbID */
    _controlFrameArbId = frameToUse;
  } else if (frameToUse == CONTROL_1) {
    GetControlFrameCopy(work);
    /* stop sending control 5 */
    UnregisterTx(CONTROL_5 | GetDeviceNumber());
    /* reregister the control frame using the new ID */
    RegisterTx(frameToUse | GetDeviceNumber(), _controlPeriodMs, 8, work);
    /* save the correct frame ArbID */
    _controlFrameArbId = frameToUse;
  }
}
void CanTalonSRX::OpenSessionIfNeedBe() {
  _can_stat = 0;
  if (_can_h == 0) {
    /* bit30 - bit8 must match $000002XX.  Top bit is not masked to get remote
     * frames */
    FRC_NetworkCommunication_CANSessionMux_openStreamSession(
        &_can_h, kParamArbIdValue | GetDeviceNumber(), kParamArbIdMask,
        kMsgCapacity, &_can_stat);
    if (_can_stat == 0) {
      /* success */
    } else {
      /* something went wrong, try again later */
      _can_h = 0;
    }
  }
}
void CanTalonSRX::ProcessStreamMessages() {
  if (0 == _can_h) OpenSessionIfNeedBe();
  /* process receive messages */
  uint32_t i;
  uint32_t messagesToRead = sizeof(_msgBuff) / sizeof(_msgBuff[0]);
  uint32_t messagesRead = 0;
  /* read out latest bunch of messages */
  _can_stat = 0;
  if (_can_h) {
    FRC_NetworkCommunication_CANSessionMux_readStreamSession(
        _can_h, _msgBuff, messagesToRead, &messagesRead, &_can_stat);
  }
  /* loop thru each message of interest */
  for (i = 0; i < messagesRead; ++i) {
    tCANStreamMessage *msg = _msgBuff + i;
    if (msg->messageID == (PARAM_RESPONSE | GetDeviceNumber())) {
      TALON_Param_Response_t *paramResp = (TALON_Param_Response_t *)msg->data;
      /* decode value */
      int32_t val = paramResp->ParamValueH;
      val <<= 8;
      val |= paramResp->ParamValueMH;
      val <<= 8;
      val |= paramResp->ParamValueML;
      val <<= 8;
      val |= paramResp->ParamValueL;
      /* save latest signal */
      _sigs[paramResp->ParamEnum] = val;
    } else {
      int brkpthere = 42;
      ++brkpthere;
    }
  }
}
void CanTalonSRX::Set(double value) {
  if (value > 1)
    value = 1;
  else if (value < -1)
    value = -1;
  SetDemand(1023 * value); /* must be within [-1023,1023] */
}
/*---------------------setters and getters that use the param
 * request/response-------------*/
/**
 * Send a one shot frame to set an arbitrary signal.
 * Most signals are in the control frame so avoid using this API unless you have
 * to.
 * Use this api for...
 * -A motor controller profile signal eProfileParam_XXXs.  These are backed up
 * in flash.  If you are gain-scheduling then call this periodically.
 * -Default brake and limit switch signals... eOnBoot_XXXs.  Avoid doing this,
 * use the override signals in the control frame.
 * Talon will automatically send a PARAM_RESPONSE after the set, so
 * GetParamResponse will catch the latest value after a couple ms.
 */
CTR_Code CanTalonSRX::SetParamRaw(unsigned paramEnum, int rawBits) {
  /* caller is using param API.  Open session if it hasn'T been done. */
  if (0 == _can_h) OpenSessionIfNeedBe();
  TALON_Param_Response_t frame;
  memset(&frame, 0, sizeof(frame));
  frame.ParamEnum = paramEnum;
  frame.ParamValueH = rawBits >> 0x18;
  frame.ParamValueMH = rawBits >> 0x10;
  frame.ParamValueML = rawBits >> 0x08;
  frame.ParamValueL = rawBits;
  int32_t status = 0;
  FRC_NetworkCommunication_CANSessionMux_sendMessage(
      PARAM_SET | GetDeviceNumber(), (const uint8_t *)&frame, 5, 0, &status);
  /* small hook here if we want the API itself to react to set commands */
  switch (paramEnum) {
    case eLegacyControlMode:
      if (rawBits != 0) {
        /* caller wants to force legacy framing */
        _useControl5ifSupported = false;
      } else {
        /* caller wants to let the API decide */
        _useControl5ifSupported = true;
      }
      /* recheck IDs now that flag has changed */
      UpdateControlId();
      break;
  }
  /* for now have a general failure if we can't transmit */
  if (status) return CTR_TxFailed;
  return CTR_OKAY;
}
/**
 * Checks cached CAN frames and updating solicited signals.
 */
CTR_Code CanTalonSRX::GetParamResponseRaw(unsigned paramEnum, int &rawBits) {
  CTR_Code retval = CTR_OKAY;
  /* process received param events. We don't expect many since this API is not
   * used often. */
  ProcessStreamMessages();
  /* grab the solicited signal value */
  sigs_t::iterator i = _sigs.find(paramEnum);
  if (i == _sigs.end()) {
    retval = CTR_SigNotUpdated;
  } else {
    rawBits = i->second;
  }
  return retval;
}
/**
 * Asks TALON to immedietely respond with signal value.  This API is only used
 * for signals that are not sent periodically.
 * This can be useful for reading params that rarely change like Limit Switch
 * settings and PIDF values.
  * @param param to request.
 */
CTR_Code CanTalonSRX::RequestParam(param_t paramEnum) {
  /* process received param events. We don't expect many since this API is not
   * used often. */
  ProcessStreamMessages();
  TALON_Param_Request_t frame;
  memset(&frame, 0, sizeof(frame));
  frame.ParamEnum = paramEnum;
  int32_t status = 0;
  FRC_NetworkCommunication_CANSessionMux_sendMessage(
      PARAM_REQUEST | GetDeviceNumber(), (const uint8_t *)&frame, 1, 0,
      &status);
  if (status) return CTR_TxFailed;
  return CTR_OKAY;
}

CTR_Code CanTalonSRX::SetParam(param_t paramEnum, double value) {
  int32_t rawbits = 0;
  switch (paramEnum) {
    case eProfileParamSlot0_P: /* unsigned 10.22 fixed pt value */
    case eProfileParamSlot0_I:
    case eProfileParamSlot0_D:
    case eProfileParamSlot1_P:
    case eProfileParamSlot1_I:
    case eProfileParamSlot1_D: {
      uint32_t urawbits;
      value = std::min(
          value, 1023.0); /* bounds check doubles that are outside u10.22 */
      value = std::max(value, 0.0);
      urawbits = value * FLOAT_TO_FXP_10_22; /* perform unsign arithmetic */
      rawbits = urawbits; /* copy bits over.  SetParamRaw just stuffs into CAN
                             frame with no sense of signedness */
    } break;
    case eProfileParamSlot1_F: /* signed 10.22 fixed pt value */
    case eProfileParamSlot0_F:
      value = std::min(
          value, 512.0); /* bounds check doubles that are outside s10.22 */
      value = std::max(value, -512.0);
      rawbits = value * FLOAT_TO_FXP_10_22;
      break;
    case eProfileParamVcompRate: /* unsigned 0.8 fixed pt value volts per ms */
                                 /* within [0,1) volts per ms.
                                         Slowest ramp is 1/256 VperMilliSec or 3.072 seconds from 0-to-12V.
                                         Fastest ramp is 255/256 VperMilliSec or 12.1ms from 0-to-12V.
                                         */
      if (value <= 0) {
        /* negative or zero (disable), send raw value of zero */
        rawbits = 0;
      } else {
        /* nonzero ramping */
        rawbits = value * FLOAT_TO_FXP_0_8;
        /* since whole part is cleared, cap to just under whole unit */
        if (rawbits > (FLOAT_TO_FXP_0_8 - 1)) rawbits = (FLOAT_TO_FXP_0_8 - 1);
        /* since ramping is nonzero, cap to smallest ramp rate possible */
        if (rawbits == 0) {
          /* caller is providing a nonzero ramp rate that's too small
                  to serialize, so cap to smallest possible */
          rawbits = 1;
        }
      }
      break;
    default: /* everything else is integral */
      rawbits = (int32_t)value;
      break;
  }
  return SetParamRaw(paramEnum, rawbits);
}
CTR_Code CanTalonSRX::GetParamResponse(param_t paramEnum, double &value) {
  int32_t rawbits = 0;
  CTR_Code retval = GetParamResponseRaw(paramEnum, rawbits);
  switch (paramEnum) {
    case eProfileParamSlot0_P: /* 10.22 fixed pt value */
    case eProfileParamSlot0_I:
    case eProfileParamSlot0_D:
    case eProfileParamSlot0_F:
    case eProfileParamSlot1_P:
    case eProfileParamSlot1_I:
    case eProfileParamSlot1_D:
    case eProfileParamSlot1_F:
    case eCurrent:
    case eTemp:
    case eBatteryV:
      value = ((double)rawbits) * FXP_TO_FLOAT_10_22;
      break;
    case eProfileParamVcompRate:
      value = ((double)rawbits) * FXP_TO_FLOAT_0_8;
      break;
    default: /* everything else is integral */
      value = (double)rawbits;
      break;
  }
  return retval;
}
CTR_Code CanTalonSRX::GetParamResponseInt32(param_t paramEnum, int &value) {
  double dvalue = 0;
  CTR_Code retval = GetParamResponse(paramEnum, dvalue);
  value = (int32_t)dvalue;
  return retval;
}
/*----- getters and setters that use param request/response. These signals are
 * backed up in flash and will survive a power cycle. ---------*/
/*----- If your application requires changing these values consider using both
 * slots and switch between slot0 <=> slot1. ------------------*/
/*----- If your application requires changing these signals frequently then it
 * makes sense to leverage this API. --------------------------*/
/*----- Getters don't block, so it may require several calls to get the latest
 * value. --------------------------*/
CTR_Code CanTalonSRX::SetPgain(unsigned slotIdx, double gain) {
  if (slotIdx == 0) return SetParam(eProfileParamSlot0_P, gain);
  return SetParam(eProfileParamSlot1_P, gain);
}
CTR_Code CanTalonSRX::SetIgain(unsigned slotIdx, double gain) {
  if (slotIdx == 0) return SetParam(eProfileParamSlot0_I, gain);
  return SetParam(eProfileParamSlot1_I, gain);
}
CTR_Code CanTalonSRX::SetDgain(unsigned slotIdx, double gain) {
  if (slotIdx == 0) return SetParam(eProfileParamSlot0_D, gain);
  return SetParam(eProfileParamSlot1_D, gain);
}
CTR_Code CanTalonSRX::SetFgain(unsigned slotIdx, double gain) {
  if (slotIdx == 0) return SetParam(eProfileParamSlot0_F, gain);
  return SetParam(eProfileParamSlot1_F, gain);
}
CTR_Code CanTalonSRX::SetIzone(unsigned slotIdx, int zone) {
  if (slotIdx == 0) return SetParam(eProfileParamSlot0_IZone, zone);
  return SetParam(eProfileParamSlot1_IZone, zone);
}
CTR_Code CanTalonSRX::SetCloseLoopRampRate(unsigned slotIdx,
                                           int closeLoopRampRate) {
  if (slotIdx == 0)
    return SetParam(eProfileParamSlot0_CloseLoopRampRate, closeLoopRampRate);
  return SetParam(eProfileParamSlot1_CloseLoopRampRate, closeLoopRampRate);
}
CTR_Code CanTalonSRX::SetVoltageCompensationRate(double voltagePerMs) {
  return SetParam(eProfileParamVcompRate, voltagePerMs);
}
CTR_Code CanTalonSRX::GetPgain(unsigned slotIdx, double &gain) {
  if (slotIdx == 0) return GetParamResponse(eProfileParamSlot0_P, gain);
  return GetParamResponse(eProfileParamSlot1_P, gain);
}
CTR_Code CanTalonSRX::GetIgain(unsigned slotIdx, double &gain) {
  if (slotIdx == 0) return GetParamResponse(eProfileParamSlot0_I, gain);
  return GetParamResponse(eProfileParamSlot1_I, gain);
}
CTR_Code CanTalonSRX::GetDgain(unsigned slotIdx, double &gain) {
  if (slotIdx == 0) return GetParamResponse(eProfileParamSlot0_D, gain);
  return GetParamResponse(eProfileParamSlot1_D, gain);
}
CTR_Code CanTalonSRX::GetFgain(unsigned slotIdx, double &gain) {
  if (slotIdx == 0) return GetParamResponse(eProfileParamSlot0_F, gain);
  return GetParamResponse(eProfileParamSlot1_F, gain);
}
CTR_Code CanTalonSRX::GetIzone(unsigned slotIdx, int &zone) {
  if (slotIdx == 0)
    return GetParamResponseInt32(eProfileParamSlot0_IZone, zone);
  return GetParamResponseInt32(eProfileParamSlot1_IZone, zone);
}
CTR_Code CanTalonSRX::GetCloseLoopRampRate(unsigned slotIdx,
                                           int &closeLoopRampRate) {
  if (slotIdx == 0)
    return GetParamResponseInt32(eProfileParamSlot0_CloseLoopRampRate,
                                 closeLoopRampRate);
  return GetParamResponseInt32(eProfileParamSlot1_CloseLoopRampRate,
                               closeLoopRampRate);
}
CTR_Code CanTalonSRX::GetVoltageCompensationRate(double &voltagePerMs) {
  return GetParamResponse(eProfileParamVcompRate, voltagePerMs);
}
CTR_Code CanTalonSRX::SetSensorPosition(int pos) {
  return SetParam(eSensorPosition, pos);
}
CTR_Code CanTalonSRX::SetForwardSoftLimit(int forwardLimit) {
  return SetParam(eProfileParamSoftLimitForThreshold, forwardLimit);
}
CTR_Code CanTalonSRX::SetReverseSoftLimit(int reverseLimit) {
  return SetParam(eProfileParamSoftLimitRevThreshold, reverseLimit);
}
CTR_Code CanTalonSRX::SetForwardSoftEnable(int enable) {
  return SetParam(eProfileParamSoftLimitForEnable, enable);
}
CTR_Code CanTalonSRX::SetReverseSoftEnable(int enable) {
  return SetParam(eProfileParamSoftLimitRevEnable, enable);
}
CTR_Code CanTalonSRX::GetForwardSoftLimit(int &forwardLimit) {
  return GetParamResponseInt32(eProfileParamSoftLimitForThreshold,
                               forwardLimit);
}
CTR_Code CanTalonSRX::GetReverseSoftLimit(int &reverseLimit) {
  return GetParamResponseInt32(eProfileParamSoftLimitRevThreshold,
                               reverseLimit);
}
CTR_Code CanTalonSRX::GetForwardSoftEnable(int &enable) {
  return GetParamResponseInt32(eProfileParamSoftLimitForEnable, enable);
}
CTR_Code CanTalonSRX::GetReverseSoftEnable(int &enable) {
  return GetParamResponseInt32(eProfileParamSoftLimitRevEnable, enable);
}
/**
 * @param param [out] Rise to fall time period in microseconds.
 */
CTR_Code CanTalonSRX::GetPulseWidthRiseToFallUs(int &param) {
  int temp = 0;
  int periodUs = 0;
  /* first grab our 12.12 position */
  CTR_Code retval1 = GetPulseWidthPosition(temp);
  /* mask off number of turns */
  temp &= 0xFFF;
  /* next grab the waveform period. This value
   * will be zero if we stop getting pulses **/
  CTR_Code retval2 = GetPulseWidthRiseToRiseUs(periodUs);
  /* now we have 0.12 position that is scaled to the waveform period.
          Use fixed pt multiply to scale our 0.16 period into us.*/
  param = (temp * periodUs) / BIT12;
  /* pass the worst error code to caller.
          Assume largest value is the most pressing error code.*/
  return (CTR_Code)std::max((int)retval1, (int)retval2);
}
CTR_Code CanTalonSRX::IsPulseWidthSensorPresent(int &param) {
  int periodUs = 0;
  CTR_Code retval = GetPulseWidthRiseToRiseUs(periodUs);
  /* if a nonzero period is present, we are getting good pules.
          Otherwise the sensor is not present. */
  if (periodUs != 0)
    param = 1;
  else
    param = 0;
  return retval;
}
/**
 * @param modeSelect selects which mode.
 * @param demand setpt or throttle or masterId to follow.
 * @return error code, 0 iff successful.
 * This function has the advantage of atomically setting mode and demand.
 */
CTR_Code CanTalonSRX::SetModeSelect(int modeSelect, int demand) {
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill =
      GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId |
                                            GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->ModeSelect = modeSelect;
  toFill->DemandH = demand >> 16;
  toFill->DemandM = demand >> 8;
  toFill->DemandL = demand >> 0;
  FlushTx(toFill);
  return CTR_OKAY;
}
/**
 * Change the periodMs of a TALON's status frame.  See kStatusFrame_* enums for
 * what's available.
 */
CTR_Code CanTalonSRX::SetStatusFrameRate(unsigned frameEnum,
                                         unsigned periodMs) {
  CTR_Code retval = CTR_OKAY;
  int32_t paramEnum = 0;
  /* bounds check the period */
  if (periodMs < 1)
    periodMs = 1;
  else if (periodMs > 255)
    periodMs = 255;
  uint8_t period = (uint8_t)periodMs;
  /* lookup the correct param enum based on what frame to rate-change */
  switch (frameEnum) {
    case kStatusFrame_General:
      paramEnum = eStatus1FrameRate;
      break;
    case kStatusFrame_Feedback:
      paramEnum = eStatus2FrameRate;
      break;
    case kStatusFrame_Encoder:
      paramEnum = eStatus3FrameRate;
      break;
    case kStatusFrame_AnalogTempVbat:
      paramEnum = eStatus4FrameRate;
      break;
    case kStatusFrame_PulseWidthMeas:
      paramEnum = eStatus8FrameRate;
      break;
    case kStatusFrame_MotionProfile:
      paramEnum = eStatus9FrameRate;
      break;
    default:
      /* caller's request is not support, return an error code */
      retval = CTR_InvalidParamValue;
      break;
  }
  /* if lookup was succesful, send set-request out */
  if (retval == CTR_OKAY) {
    /* paramEnum is updated, sent it out */
    retval = SetParamRaw(paramEnum, period);
  }
  return retval;
}
/**
 * Clear all sticky faults in TALON.
 */
CTR_Code CanTalonSRX::ClearStickyFaults() {
  int32_t status = 0;
  /* build request frame */
  TALON_Control_3_ClearFlags_OneShot_t frame;
  memset(&frame, 0, sizeof(frame));
  frame.ClearStickyFaults = 1;
  FRC_NetworkCommunication_CANSessionMux_sendMessage(
      CONTROL_3 | GetDeviceNumber(), (const uint8_t *)&frame, sizeof(frame), 0,
      &status);
  if (status) return CTR_TxFailed;
  return CTR_OKAY;
}
/**
 * @return the tx task that transmits Control6 (motion profile control).
 *         If it's not scheduled, then schedule it.  This is part of firing
 *         the MotionProf framing only when needed to save bandwidth.
 */
CtreCanNode::txTask<TALON_Control_6_MotProfAddTrajPoint_t>
CanTalonSRX::GetControl6() {
  CtreCanNode::txTask<TALON_Control_6_MotProfAddTrajPoint_t> control6 =
      GetTx<TALON_Control_6_MotProfAddTrajPoint_t>(CONTROL_6 |
                                                   GetDeviceNumber());
  if (control6.IsEmpty()) {
    /* control6 never started, arm it now */
    RegisterTx(CONTROL_6 | GetDeviceNumber(), _control6PeriodMs);
    control6 = GetTx<TALON_Control_6_MotProfAddTrajPoint_t>(CONTROL_6 |
                                                            GetDeviceNumber());
    control6->Idx = 0;
    _motProfFlowControl = 0;
    FlushTx(control6);
  }
  return control6;
}
/**
 * Calling application can opt to speed up the handshaking between the robot API
 * and the Talon to increase the download rate of the Talon's Motion Profile.
 * Ideally the period should be no more than half the period of a trajectory
 * point.
 */
void CanTalonSRX::ChangeMotionControlFramePeriod(uint32_t periodMs) {
  std::unique_lock<std::mutex> lock(_mutMotProf);
  /* if message is already registered, it will get updated.
   * Otherwise it will error if it hasn't been setup yet, but that's ok
   * because the _control6PeriodMs will be used later.
   * @see GetControl6
   */
  _control6PeriodMs = periodMs;
  ChangeTxPeriod(CONTROL_6 | GetDeviceNumber(), _control6PeriodMs);
}
/**
 * Clear the buffered motion profile in both Talon RAM (bottom), and in the API
 * (top).
 */
void CanTalonSRX::ClearMotionProfileTrajectories() {
  std::unique_lock<std::mutex> lock(_mutMotProf);
  /* clear the top buffer */
  _motProfTopBuffer.Clear();
  /* send signal to clear bottom buffer */
  auto toFill = CanTalonSRX::GetControl6();
  toFill->Idx = 0;
  _motProfFlowControl = 0; /* match the transmitted flow control */
  FlushTx(toFill);
}
/**
 * Retrieve just the buffer count for the api-level (top) buffer.
 * This routine performs no CAN or data structure lookups, so its fast and ideal
 * if caller needs to quickly poll the progress of trajectory points being
 * emptied into Talon's RAM. Otherwise just use GetMotionProfileStatus.
 * @return number of trajectory points in the top buffer.
 */
uint32_t CanTalonSRX::GetMotionProfileTopLevelBufferCount() {
  std::unique_lock<std::mutex> lock(_mutMotProf);
  uint32_t retval = (uint32_t)_motProfTopBuffer.GetNumTrajectories();
  return retval;
}
/**
 * Retrieve just the buffer full for the api-level (top) buffer.
 * This routine performs no CAN or data structure lookups, so its fast and ideal
 * if caller needs to quickly poll. Otherwise just use GetMotionProfileStatus.
 * @return number of trajectory points in the top buffer.
 */
bool CanTalonSRX::IsMotionProfileTopLevelBufferFull() {
  std::unique_lock<std::mutex> lock(_mutMotProf);
  if (_motProfTopBuffer.GetNumTrajectories() >= kMotionProfileTopBufferCapacity)
    return true;
  return false;
}
/**
 * Push another trajectory point into the top level buffer (which is emptied
 * into the Talon's bottom buffer as room allows).
 * @param targPos  servo position in native Talon units (sensor units).
 * @param targVel  velocity to feed-forward in native Talon units (sensor units
 *                 per 100ms).
 * @param profileSlotSelect  which slot to pull PIDF gains from.  Currently
 *                           supports 0 or 1.
 * @param timeDurMs  time in milliseconds of how long to apply this point.
 * @param velOnly  set to nonzero to signal Talon that only the feed-foward
 *                 velocity should be used, i.e. do not perform PID on position.
 *                 This is equivalent to setting PID gains to zero, but much
 *                 more efficient and synchronized to MP.
 * @param isLastPoint  set to nonzero to signal Talon to keep processing this
 *                     trajectory point, instead of jumping to the next one
 *                     when timeDurMs expires.  Otherwise MP executer will
 *                     eventually see an empty buffer after the last point
 *                     expires, causing it to assert the IsUnderRun flag.
 *                     However this may be desired if calling application
 *                     never wants to terminate the MP.
 * @param zeroPos  set to nonzero to signal Talon to "zero" the selected
 *                 position sensor before executing this trajectory point.
 *                 Typically the first point should have this set only thus
 *                 allowing the remainder of the MP positions to be relative to
 *                 zero.
 * @return CTR_OKAY if trajectory point push ok. CTR_BufferFull if buffer is
 *         full due to kMotionProfileTopBufferCapacity.
 */
CTR_Code CanTalonSRX::PushMotionProfileTrajectory(int targPos, int targVel,
                                                  int profileSlotSelect,
                                                  int timeDurMs, int velOnly,
                                                  int isLastPoint,
                                                  int zeroPos) {
  ReactToMotionProfileCall();
  /* create our trajectory point */
  TALON_Control_6_MotProfAddTrajPoint_huff0_t traj;
  memset((void *)&traj, 0, sizeof(traj));
  traj.NextPt_ZeroPosition = zeroPos ? 1 : 0;
  traj.NextPt_VelOnly = velOnly ? 1 : 0;
  traj.NextPt_IsLast = isLastPoint ? 1 : 0;
  traj.NextPt_ProfileSlotSelect = (profileSlotSelect > 0) ? 1 : 0;
  if (timeDurMs < 0)
    timeDurMs = 0;
  else if (timeDurMs > 255)
    timeDurMs = 255;
  traj.NextPt_DurationMs = timeDurMs;
  traj.NextPt_VelocityH = targVel >> 0x08;
  traj.NextPt_VelocityL = targVel & 0xFF;
  traj.NextPt_PositionH = targPos >> 0x10;
  traj.NextPt_PositionM = targPos >> 0x08;
  traj.NextPt_PositionL = targPos & 0xFF;

  std::unique_lock<std::mutex> lock(_mutMotProf);
  if (_motProfTopBuffer.GetNumTrajectories() >= kMotionProfileTopBufferCapacity)
    return CTR_BufferFull;
  _motProfTopBuffer.Push(traj);
  return CTR_OKAY;
}
/**
 * Increment our flow control to manage streaming to the Talon.
 * f(x) = { 1,   x = 15,
 *          x+1,  x < 15
 *        }
 */
#define MotionProf_IncrementSync(idx) ((idx >= 15) ? 1 : 0) + ((idx + 1) & 0xF)
/**
 * Update the NextPt signals inside the control frame given the next pt to send.
 * @param control pointer to the CAN frame payload containing control6.  Only
 * the signals that serialize the next trajectory point are updated from the
 * contents of newPt.
 * @param newPt point to the next trajectory that needs to be inserted into
 *        Talon RAM.
 */
void CanTalonSRX::CopyTrajPtIntoControl(
    TALON_Control_6_MotProfAddTrajPoint_t *control,
    const TALON_Control_6_MotProfAddTrajPoint_t *newPt) {
  /* Bring over the common signals in the first two bytes */
  control->NextPt_ProfileSlotSelect = newPt->NextPt_ProfileSlotSelect;
  control->NextPt_ZeroPosition = newPt->NextPt_ZeroPosition;
  control->NextPt_VelOnly = newPt->NextPt_VelOnly;
  control->NextPt_IsLast = newPt->NextPt_IsLast;
  control->huffCode = newPt->huffCode;
  /* the last six bytes are entirely for hold NextPt's values. */
  uint8_t *dest = (uint8_t *)control;
  const uint8_t *src = (const uint8_t *)newPt;
  dest[2] = src[2];
  dest[3] = src[3];
  dest[4] = src[4];
  dest[5] = src[5];
  dest[6] = src[6];
  dest[7] = src[7];
}
/**
 * Caller is either pushing a new motion profile point, or is
 * calling the Process buffer routine.  In either case check our
 * flow control to see if we need to start sending control6.
 */
void CanTalonSRX::ReactToMotionProfileCall() {
  if (_motProfFlowControl < 0) {
    /* we have not yet armed the periodic frame.  We do this lazilly to
     * save bus utilization since most Talons on the bus probably are not
     * MP'ing.
     */
    ClearMotionProfileTrajectories(); /* this moves flow control so only fires
                                         once if ever */
  }
}
/**
 * This must be called periodically to funnel the trajectory points from the
 * API's top level buffer to the Talon's bottom level buffer.  Recommendation
 * is to call this twice as fast as the executation rate of the motion profile.
 * So if MP is running with 20ms trajectory points, try calling this routine
 * every 10ms.  All motion profile functions are thread-safe through the use of
 * a mutex, so there is no harm in having the caller utilize threading.
 */
void CanTalonSRX::ProcessMotionProfileBuffer() {
  ReactToMotionProfileCall();
  /* get the latest status frame */
  GET_STATUS9();
  /* lock */
  std::unique_lock<std::mutex> lock(_mutMotProf);
  /* calc what we expect to receive */
  if (_motProfFlowControl == rx->NextID) {
    /* Talon has completed the last req */
    if (_motProfTopBuffer.IsEmpty()) {
      /* nothing to do */
    } else {
      /* get the latest control frame */
      auto toFill = GetControl6();
      TALON_Control_6_MotProfAddTrajPoint_t *front = _motProfTopBuffer.Front();
      CopyTrajPtIntoControl(toFill.toSend, front);
      _motProfTopBuffer.Pop();
      _motProfFlowControl = MotionProf_IncrementSync(_motProfFlowControl);
      toFill->Idx = _motProfFlowControl;
      FlushTx(toFill);
    }
  } else {
    /* still waiting on Talon */
  }
}
/**
 * Retrieve all status information.
 * Since this all comes from one CAN frame, its ideal to have one routine to
 * retrieve the frame once and decode everything.
 * @param [out] flags  bitfield for status bools. Starting with least
 *        significant bit: IsValid, HasUnderrun, IsUnderrun, IsLast, VelOnly.
 *
 *        IsValid  set when MP executer is processing a trajectory point,
 *                 and that point's status is instrumented with IsLast,
 *                 VelOnly, targPos, targVel.  However if MP executor is
 *                 not processing a trajectory point, then this flag is
 *                 false, and the instrumented signals will be zero.
 *        HasUnderrun  is set anytime the MP executer is ready to pop
 *                     another trajectory point from the Talon's RAM,
 *                     but the buffer is empty.  It can only be cleared
 *                     by using SetParam(eMotionProfileHasUnderrunErr,0);
 *        IsUnderrun  is set when the MP executer is ready for another
 *                    point, but the buffer is empty, and cleared when
 *                    the MP executer does not need another point.
 *                    HasUnderrun shadows this registor when this
 *                    register gets set, however HasUnderrun stays
 *                    asserted until application has process it, and
 *                    IsUnderrun auto-clears when the condition is
 *                    resolved.
 *        IsLast  is set/cleared based on the MP executer's current
 *                trajectory point's IsLast value.  This assumes
 *                IsLast was set when PushMotionProfileTrajectory
 *                was used to insert the currently processed trajectory
 *                point.
 *        VelOnly  is set/cleared based on the MP executer's current
 *                 trajectory point's VelOnly value.
 *
 * @param [out] profileSlotSelect  The currently processed trajectory point's
 *        selected slot.  This can differ in the currently selected slot used
 *        for Position and Velocity servo modes.
 * @param [out] targPos The currently processed trajectory point's position
 *        in native units.  This param is zero if IsValid is zero.
 * @param [out] targVel The currently processed trajectory point's velocity
 *        in native units.  This param is zero if IsValid is zero.
 * @param [out] topBufferRem The remaining number of points in the top level
 *        buffer.
 * @param [out] topBufferCnt The number of points in the top level buffer to
 *        be sent to Talon.
 * @param [out] btmBufferCnt The number of points in the bottom level buffer
 *        inside Talon.
 * @return CTR error code
 */
CTR_Code CanTalonSRX::GetMotionProfileStatus(
    uint32_t &flags, uint32_t &profileSlotSelect, int32_t &targPos,
    int32_t &targVel, uint32_t &topBufferRem, uint32_t &topBufferCnt,
    uint32_t &btmBufferCnt, uint32_t &outputEnable) {
  /* get the latest status frame */
  GET_STATUS9();

  /* clear signals in case we never received an update, caller should check
   * return
   */
  flags = 0;
  profileSlotSelect = 0;
  targPos = 0;
  targVel = 0;
  btmBufferCnt = 0;

  /* these signals are always available */
  topBufferCnt = _motProfTopBuffer.GetNumTrajectories();
  topBufferRem =
      kMotionProfileTopBufferCapacity - _motProfTopBuffer.GetNumTrajectories();

  /* TODO: make enums or make a better method prototype */
  if (rx->ActTraj_IsValid) flags |= kMotionProfileFlag_ActTraj_IsValid;
  if (rx->HasUnderrun) flags |= kMotionProfileFlag_HasUnderrun;
  if (rx->IsUnderrun) flags |= kMotionProfileFlag_IsUnderrun;
  if (rx->ActTraj_IsLast) flags |= kMotionProfileFlag_ActTraj_IsLast;
  if (rx->ActTraj_VelOnly) flags |= kMotionProfileFlag_ActTraj_VelOnly;

  btmBufferCnt = rx->Count;

  targPos = rx->ActTraj_PositionH;
  targPos <<= 8;
  targPos |= rx->ActTraj_PositionM;
  targPos <<= 8;
  targPos |= rx->ActTraj_PositionL;

  targVel = rx->ActTraj_VelocityH;
  targVel <<= 8;
  targVel |= rx->ActTraj_VelocityL;

  profileSlotSelect = rx->ActTraj_ProfileSlotSelect;

  switch (rx->OutputType) {
    case kMotionProf_Disabled:
    case kMotionProf_Enable:
    case kMotionProf_Hold:
      outputEnable = rx->OutputType;
      break;
    default:
      /* do now allow invalid values for sake of user-facing enum types */
      outputEnable = kMotionProf_Disabled;
      break;
  }
  return rx.err;
}
//------------------------ auto generated ------------------------------------//
/* This API is optimal since it uses the fire-and-forget CAN interface.
 * These signals should cover the majority of all use cases.
 */
CTR_Code CanTalonSRX::GetFault_OverTemp(int &param)
{
  GET_STATUS1();
  param = rx->Fault_OverTemp;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFault_UnderVoltage(int &param)
{
  GET_STATUS1();
  param = rx->Fault_UnderVoltage;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFault_ForLim(int &param)
{
  GET_STATUS1();
  param = rx->Fault_ForLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFault_RevLim(int &param)
{
  GET_STATUS1();
  param = rx->Fault_RevLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFault_HardwareFailure(int &param)
{
  GET_STATUS1();
  param = rx->Fault_HardwareFailure;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFault_ForSoftLim(int &param)
{
  GET_STATUS1();
  param = rx->Fault_ForSoftLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFault_RevSoftLim(int &param)
{
  GET_STATUS1();
  param = rx->Fault_RevSoftLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_OverTemp(int &param)
{
  GET_STATUS2();
  param = rx->StckyFault_OverTemp;
  return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_UnderVoltage(int &param)
{
  GET_STATUS2();
  param = rx->StckyFault_UnderVoltage;
  return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_ForLim(int &param)
{
  GET_STATUS2();
  param = rx->StckyFault_ForLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_RevLim(int &param)
{
  GET_STATUS2();
  param = rx->StckyFault_RevLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_ForSoftLim(int &param)
{
  GET_STATUS2();
  param = rx->StckyFault_ForSoftLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_RevSoftLim(int &param)
{
  GET_STATUS2();
  param = rx->StckyFault_RevSoftLim;
  return rx.err;
}
CTR_Code CanTalonSRX::GetAppliedThrottle(int &param)
{
  GET_STATUS1();
  int32_t raw = 0;
  raw |= rx->AppliedThrottle_h3;
  raw <<= 8;
  raw |= rx->AppliedThrottle_l8;
  raw <<= (32-11); /* sign extend */
  raw >>= (32-11); /* sign extend */
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetCloseLoopErr(int &param)
{
  GET_STATUS1();
  int32_t raw = 0;
  raw |= rx->CloseLoopErrH;
  raw <<= 16 - 8;
  raw |= rx->CloseLoopErrM;
  raw <<= 8;
  raw |= rx->CloseLoopErrL;
  raw <<= (32-24); /* sign extend */
  raw >>= (32-24); /* sign extend */
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFeedbackDeviceSelect(int &param)
{
  GET_STATUS1();
  param = rx->FeedbackDeviceSelect;
  return rx.err;
}
CTR_Code CanTalonSRX::GetModeSelect(int &param)
{
  GET_STATUS1();
  uint32_t raw = 0;
  raw |= rx->ModeSelect_h1;
  raw <<= 3;
  raw |= rx->ModeSelect_b3;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetLimitSwitchEn(int &param)
{
  GET_STATUS1();
  param = rx->LimitSwitchEn;
  return rx.err;
}
CTR_Code CanTalonSRX::GetLimitSwitchClosedFor(int &param)
{
  GET_STATUS1();
  param = rx->LimitSwitchClosedFor;
  return rx.err;
}
CTR_Code CanTalonSRX::GetLimitSwitchClosedRev(int &param)
{
  GET_STATUS1();
  param = rx->LimitSwitchClosedRev;
  return rx.err;
}
CTR_Code CanTalonSRX::GetSensorPosition(int &param)
{
  GET_STATUS2();
  int32_t raw = 0;
  raw |= rx->SensorPositionH;
  raw <<= 16 - 8;
  raw |= rx->SensorPositionM;
  raw <<= 8;
  raw |= rx->SensorPositionL;
  raw <<= (32-24); /* sign extend */
  raw >>= (32-24); /* sign extend */
  if(rx->PosDiv8)
    raw *= 8;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetSensorVelocity(int &param)
{
  GET_STATUS2();
  int32_t raw = 0;
  raw |= rx->SensorVelocityH;
  raw <<= 8;
  raw |= rx->SensorVelocityL;
  raw <<= (32-16); /* sign extend */
  raw >>= (32-16); /* sign extend */
  if(rx->VelDiv4)
    raw *= 4;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetCurrent(double &param)
{
  GET_STATUS2();
  uint32_t raw = 0;
  raw |= rx->Current_h8;
  raw <<= 2;
  raw |= rx->Current_l2;
  param = (double)raw * 0.125 + 0;
  return rx.err;
}
CTR_Code CanTalonSRX::GetBrakeIsEnabled(int &param)
{
  GET_STATUS2();
  param = rx->BrakeIsEnabled;
  return rx.err;
}
CTR_Code CanTalonSRX::GetEncPosition(int &param)
{
  GET_STATUS3();
  int32_t raw = 0;
  raw |= rx->EncPositionH;
  raw <<= 16 - 8;
  raw |= rx->EncPositionM;
  raw <<= 8;
  raw |= rx->EncPositionL;
  raw <<= (32-24); /* sign extend */
  raw >>= (32-24); /* sign extend */
  if(rx->PosDiv8)
    raw *= 8;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetEncVel(int &param)
{
  GET_STATUS3();
  int32_t raw = 0;
  raw |= rx->EncVelH;
  raw <<= 8;
  raw |= rx->EncVelL;
  raw <<= (32-16); /* sign extend */
  raw >>= (32-16); /* sign extend */
  if(rx->VelDiv4)
    raw *= 4;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetEncIndexRiseEvents(int &param)
{
  GET_STATUS3();
  uint32_t raw = 0;
  raw |= rx->EncIndexRiseEventsH;
  raw <<= 8;
  raw |= rx->EncIndexRiseEventsL;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetQuadApin(int &param)
{
  GET_STATUS3();
  param = rx->QuadApin;
  return rx.err;
}
CTR_Code CanTalonSRX::GetQuadBpin(int &param)
{
  GET_STATUS3();
  param = rx->QuadBpin;
  return rx.err;
}
CTR_Code CanTalonSRX::GetQuadIdxpin(int &param)
{
  GET_STATUS3();
  param = rx->QuadIdxpin;
  return rx.err;
}
CTR_Code CanTalonSRX::GetAnalogInWithOv(int &param)
{
  GET_STATUS4();
  int32_t raw = 0;
  raw |= rx->AnalogInWithOvH;
  raw <<= 16 - 8;
  raw |= rx->AnalogInWithOvM;
  raw <<= 8;
  raw |= rx->AnalogInWithOvL;
  raw <<= (32-24); /* sign extend */
  raw >>= (32-24); /* sign extend */
  if(rx->PosDiv8)
    raw *= 8;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetAnalogInVel(int &param)
{
  GET_STATUS4();
  int32_t raw = 0;
  raw |= rx->AnalogInVelH;
  raw <<= 8;
  raw |= rx->AnalogInVelL;
  raw <<= (32-16); /* sign extend */
  raw >>= (32-16); /* sign extend */
  if(rx->VelDiv4)
    raw *= 4;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetTemp(double &param)
{
  GET_STATUS4();
  uint32_t raw = rx->Temp;
  param = (double)raw * 0.6451612903 + -50;
  return rx.err;
}
CTR_Code CanTalonSRX::GetBatteryV(double &param)
{
  GET_STATUS4();
  uint32_t raw = rx->BatteryV;
  param = (double)raw * 0.05 + 4;
  return rx.err;
}
CTR_Code CanTalonSRX::GetResetCount(int &param)
{
  GET_STATUS5();
  uint32_t raw = 0;
  raw |= rx->ResetCountH;
  raw <<= 8;
  raw |= rx->ResetCountL;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetResetFlags(int &param)
{
  GET_STATUS5();
  uint32_t raw = 0;
  raw |= rx->ResetFlagsH;
  raw <<= 8;
  raw |= rx->ResetFlagsL;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetFirmVers(int &param)
{
  GET_STATUS5();
  uint32_t raw = 0;
  raw |= rx->FirmVersH;
  raw <<= 8;
  raw |= rx->FirmVersL;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetPulseWidthPosition(int &param)
{
  GET_STATUS8();
  int32_t raw = 0;
  raw |= rx->PulseWidPositionH;
  raw <<= 16 - 8;
  raw |= rx->PulseWidPositionM;
  raw <<= 8;
  raw |= rx->PulseWidPositionL;
  raw <<= (32-24); /* sign extend */
  raw >>= (32-24); /* sign extend */
  if(rx->PosDiv8)
    raw *= 8;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetPulseWidthVelocity(int &param)
{
  GET_STATUS8();
  int32_t raw = 0;
  raw |= rx->PulseWidVelH;
  raw <<= 8;
  raw |= rx->PulseWidVelL;
  raw <<= (32-16); /* sign extend */
  raw >>= (32-16); /* sign extend */
  if(rx->VelDiv4)
    raw *= 4;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetPulseWidthRiseToRiseUs(int &param)
{
  GET_STATUS8();
  uint32_t raw = 0;
  raw |= rx->PeriodUsM8;
  raw <<= 8;
  raw |= rx->PeriodUsL8;
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetActTraj_IsValid(int &param)
{
  GET_STATUS9();
  param = rx->ActTraj_IsValid;
  return rx.err;
}
CTR_Code CanTalonSRX::GetActTraj_ProfileSlotSelect(int &param)
{
  GET_STATUS9();
  param = rx->ActTraj_ProfileSlotSelect;
  return rx.err;
}
CTR_Code CanTalonSRX::GetActTraj_VelOnly(int &param)
{
  GET_STATUS9();
  param = rx->ActTraj_VelOnly;
  return rx.err;
}
CTR_Code CanTalonSRX::GetActTraj_IsLast(int &param)
{
  GET_STATUS9();
  param = rx->ActTraj_IsLast;
  return rx.err;
}
CTR_Code CanTalonSRX::GetOutputType(int &param)
{
  GET_STATUS9();
  param = rx->OutputType;
  return rx.err;
}
CTR_Code CanTalonSRX::GetHasUnderrun(int &param)
{
  GET_STATUS9();
  param = rx->HasUnderrun;
  return rx.err;
}
CTR_Code CanTalonSRX::GetIsUnderrun(int &param)
{
  GET_STATUS9();
  param = rx->IsUnderrun;
  return rx.err;
}
CTR_Code CanTalonSRX::GetNextID(int &param)
{
  GET_STATUS9();
  param = rx->NextID;
  return rx.err;
}
CTR_Code CanTalonSRX::GetBufferIsFull(int &param)
{
  GET_STATUS9();
  param = rx->BufferIsFull;
  return rx.err;
}
CTR_Code CanTalonSRX::GetCount(int &param)
{
  GET_STATUS9();
  param = rx->Count;
  return rx.err;
}
CTR_Code CanTalonSRX::GetActTraj_Velocity(int &param)
{
  GET_STATUS9();
  int32_t raw = 0;
  raw |= rx->ActTraj_VelocityH;
  raw <<= 8;
  raw |= rx->ActTraj_VelocityL;
  raw <<= (32-16); /* sign extend */
  raw >>= (32-16); /* sign extend */
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::GetActTraj_Position(int &param)
{
  GET_STATUS9();
  int32_t raw = 0;
  raw |= rx->ActTraj_PositionH;
  raw <<= 16 - 8;
  raw |= rx->ActTraj_PositionM;
  raw <<= 8;
  raw |= rx->ActTraj_PositionL;
  raw <<= (32-24); /* sign extend */
  raw >>= (32-24); /* sign extend */
  param = (int)raw;
  return rx.err;
}
CTR_Code CanTalonSRX::SetDemand(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->DemandH = param>>16;
  toFill->DemandM = param>>8;
  toFill->DemandL = param>>0;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetOverrideLimitSwitchEn(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->OverrideLimitSwitchEn = param;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetFeedbackDeviceSelect(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->FeedbackDeviceSelect = param;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetRevMotDuringCloseLoopEn(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->RevMotDuringCloseLoopEn = param;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetOverrideBrakeType(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->OverrideBrakeType = param;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetModeSelect(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->ModeSelect = param;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetProfileSlotSelect(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->ProfileSlotSelect = param;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetRampThrottle(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->RampThrottle = param;
  FlushTx(toFill);
  return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetRevFeedbackSensor(int param)
{
  CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(_controlFrameArbId | GetDeviceNumber());
  if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
  toFill->RevFeedbackSensor = param ? 1 : 0;
  FlushTx(toFill);
  return CTR_OKAY;
}
//------------------ C interface --------------------------------------------//
extern "C" {
void *c_TalonSRX_Create3(int deviceNumber, int controlPeriodMs, int enablePeriodMs)
{
  return new CanTalonSRX(deviceNumber, controlPeriodMs, enablePeriodMs);
}
void *c_TalonSRX_Create2(int deviceNumber, int controlPeriodMs)
{
  return new CanTalonSRX(deviceNumber, controlPeriodMs);
}
void *c_TalonSRX_Create1(int deviceNumber)
{
  return new CanTalonSRX(deviceNumber);
}
void c_TalonSRX_Destroy(void *handle)
{
  delete (CanTalonSRX*)handle;
}
void c_TalonSRX_Set(void *handle, double value)
{
  return ((CanTalonSRX*)handle)->Set(value);
}
CTR_Code c_TalonSRX_SetParam(void *handle, int paramEnum, double value)
{
  return ((CanTalonSRX*)handle)->SetParam((CanTalonSRX::param_t)paramEnum, value);
}
CTR_Code c_TalonSRX_RequestParam(void *handle, int paramEnum)
{
  return ((CanTalonSRX*)handle)->RequestParam((CanTalonSRX::param_t)paramEnum);
}
CTR_Code c_TalonSRX_GetParamResponse(void *handle, int paramEnum, double *value)
{
  return ((CanTalonSRX*)handle)->GetParamResponse((CanTalonSRX::param_t)paramEnum, *value);
}
CTR_Code c_TalonSRX_GetParamResponseInt32(void *handle, int paramEnum, int *value)
{
  return ((CanTalonSRX*)handle)->GetParamResponseInt32((CanTalonSRX::param_t)paramEnum, *value);
}
CTR_Code c_TalonSRX_SetPgain(void *handle, int slotIdx, double gain)
{
  return ((CanTalonSRX*)handle)->SetPgain((unsigned)slotIdx, gain);
}
CTR_Code c_TalonSRX_SetIgain(void *handle, int slotIdx, double gain)
{
  return ((CanTalonSRX*)handle)->SetIgain((unsigned)slotIdx, gain);
}
CTR_Code c_TalonSRX_SetDgain(void *handle, int slotIdx, double gain)
{
  return ((CanTalonSRX*)handle)->SetDgain((unsigned)slotIdx, gain);
}
CTR_Code c_TalonSRX_SetFgain(void *handle, int slotIdx, double gain)
{
  return ((CanTalonSRX*)handle)->SetFgain((unsigned)slotIdx, gain);
}
CTR_Code c_TalonSRX_SetIzone(void *handle, int slotIdx, int zone)
{
  return ((CanTalonSRX*)handle)->SetIzone((unsigned)slotIdx, zone);
}
CTR_Code c_TalonSRX_SetCloseLoopRampRate(void *handle, int slotIdx, int closeLoopRampRate)
{
  return ((CanTalonSRX*)handle)->SetCloseLoopRampRate((unsigned)slotIdx, closeLoopRampRate);
}
CTR_Code c_TalonSRX_SetVoltageCompensationRate(void *handle, double voltagePerMs)
{
  return ((CanTalonSRX*)handle)->SetVoltageCompensationRate(voltagePerMs);
}
CTR_Code c_TalonSRX_SetSensorPosition(void *handle, int pos)
{
  return ((CanTalonSRX*)handle)->SetSensorPosition(pos);
}
CTR_Code c_TalonSRX_SetForwardSoftLimit(void *handle, int forwardLimit)
{
  return ((CanTalonSRX*)handle)->SetForwardSoftLimit(forwardLimit);
}
CTR_Code c_TalonSRX_SetReverseSoftLimit(void *handle, int reverseLimit)
{
  return ((CanTalonSRX*)handle)->SetReverseSoftLimit(reverseLimit);
}
CTR_Code c_TalonSRX_SetForwardSoftEnable(void *handle, int enable)
{
  return ((CanTalonSRX*)handle)->SetForwardSoftEnable(enable);
}
CTR_Code c_TalonSRX_SetReverseSoftEnable(void *handle, int enable)
{
  return ((CanTalonSRX*)handle)->SetReverseSoftEnable(enable);
}
CTR_Code c_TalonSRX_GetPgain(void *handle, int slotIdx, double *gain)
{
  return ((CanTalonSRX*)handle)->GetPgain((unsigned)slotIdx, *gain);
}
CTR_Code c_TalonSRX_GetIgain(void *handle, int slotIdx, double *gain)
{
  return ((CanTalonSRX*)handle)->GetIgain((unsigned)slotIdx, *gain);
}
CTR_Code c_TalonSRX_GetDgain(void *handle, int slotIdx, double *gain)
{
  return ((CanTalonSRX*)handle)->GetDgain((unsigned)slotIdx, *gain);
}
CTR_Code c_TalonSRX_GetFgain(void *handle, int slotIdx, double *gain)
{
  return ((CanTalonSRX*)handle)->GetFgain((unsigned)slotIdx, *gain);
}
CTR_Code c_TalonSRX_GetIzone(void *handle, int slotIdx, int *zone)
{
  return ((CanTalonSRX*)handle)->GetIzone((unsigned)slotIdx, *zone);
}
CTR_Code c_TalonSRX_GetCloseLoopRampRate(void *handle, int slotIdx, int *closeLoopRampRate)
{
  return ((CanTalonSRX*)handle)->GetCloseLoopRampRate((unsigned)slotIdx, *closeLoopRampRate);
}
CTR_Code c_TalonSRX_GetVoltageCompensationRate(void *handle, double *voltagePerMs)
{
  return ((CanTalonSRX*)handle)->GetVoltageCompensationRate(*voltagePerMs);
}
CTR_Code c_TalonSRX_GetForwardSoftLimit(void *handle, int *forwardLimit)
{
  return ((CanTalonSRX*)handle)->GetForwardSoftLimit(*forwardLimit);
}
CTR_Code c_TalonSRX_GetReverseSoftLimit(void *handle, int *reverseLimit)
{
  return ((CanTalonSRX*)handle)->GetReverseSoftLimit(*reverseLimit);
}
CTR_Code c_TalonSRX_GetForwardSoftEnable(void *handle, int *enable)
{
  return ((CanTalonSRX*)handle)->GetForwardSoftEnable(*enable);
}
CTR_Code c_TalonSRX_GetReverseSoftEnable(void *handle, int *enable)
{
  return ((CanTalonSRX*)handle)->GetReverseSoftEnable(*enable);
}
CTR_Code c_TalonSRX_GetPulseWidthRiseToFallUs(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetPulseWidthRiseToFallUs(*param);
}
CTR_Code c_TalonSRX_IsPulseWidthSensorPresent(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->IsPulseWidthSensorPresent(*param);
}
CTR_Code c_TalonSRX_SetModeSelect2(void *handle, int modeSelect, int demand)
{
  return ((CanTalonSRX*)handle)->SetModeSelect(modeSelect, demand);
}
CTR_Code c_TalonSRX_SetStatusFrameRate(void *handle, int frameEnum, int periodMs)
{
  return ((CanTalonSRX*)handle)->SetStatusFrameRate((unsigned)frameEnum, (unsigned)periodMs);
}
CTR_Code c_TalonSRX_ClearStickyFaults(void *handle)
{
  return ((CanTalonSRX*)handle)->ClearStickyFaults();
}
void c_TalonSRX_ChangeMotionControlFramePeriod(void *handle, int periodMs)
{
  return ((CanTalonSRX*)handle)->ChangeMotionControlFramePeriod((uint32_t)periodMs);
}
void c_TalonSRX_ClearMotionProfileTrajectories(void *handle)
{
  return ((CanTalonSRX*)handle)->ClearMotionProfileTrajectories();
}
int c_TalonSRX_GetMotionProfileTopLevelBufferCount(void *handle)
{
  return ((CanTalonSRX*)handle)->GetMotionProfileTopLevelBufferCount();
}
int c_TalonSRX_IsMotionProfileTopLevelBufferFull(void *handle)
{
  return ((CanTalonSRX*)handle)->IsMotionProfileTopLevelBufferFull();
}
CTR_Code c_TalonSRX_PushMotionProfileTrajectory(void *handle, int targPos, int targVel, int profileSlotSelect, int timeDurMs, int velOnly, int isLastPoint, int zeroPos)
{
  return ((CanTalonSRX*)handle)->PushMotionProfileTrajectory(targPos, targVel, profileSlotSelect, timeDurMs, velOnly, isLastPoint, zeroPos);
}
void c_TalonSRX_ProcessMotionProfileBuffer(void *handle)
{
  return ((CanTalonSRX*)handle)->ProcessMotionProfileBuffer();
}
CTR_Code c_TalonSRX_GetMotionProfileStatus(void *handle, int *flags, int *profileSlotSelect, int *targPos, int *targVel, int *topBufferRemaining, int *topBufferCnt, int *btmBufferCnt, int *outputEnable)
{
  uint32_t flags_val;
  uint32_t profileSlotSelect_val;
  int32_t targPos_val;
  int32_t targVel_val;
  uint32_t topBufferRemaining_val;
  uint32_t topBufferCnt_val;
  uint32_t btmBufferCnt_val;
  uint32_t outputEnable_val;
  CTR_Code retval = ((CanTalonSRX*)handle)->GetMotionProfileStatus(flags_val, profileSlotSelect_val, targPos_val, targVel_val, topBufferRemaining_val, topBufferCnt_val, btmBufferCnt_val, outputEnable_val);
  *flags = (int)flags_val;
  *profileSlotSelect = (int)profileSlotSelect_val;
  *targPos = (int)targPos_val;
  *targVel = (int)targVel_val;
  *topBufferRemaining = (int)topBufferRemaining_val;
  *topBufferCnt = (int)topBufferCnt_val;
  *btmBufferCnt = (int)btmBufferCnt_val;
  *outputEnable = (int)outputEnable_val;
  return retval;
}
CTR_Code c_TalonSRX_GetFault_OverTemp(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFault_OverTemp(*param);
}
CTR_Code c_TalonSRX_GetFault_UnderVoltage(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFault_UnderVoltage(*param);
}
CTR_Code c_TalonSRX_GetFault_ForLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFault_ForLim(*param);
}
CTR_Code c_TalonSRX_GetFault_RevLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFault_RevLim(*param);
}
CTR_Code c_TalonSRX_GetFault_HardwareFailure(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFault_HardwareFailure(*param);
}
CTR_Code c_TalonSRX_GetFault_ForSoftLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFault_ForSoftLim(*param);
}
CTR_Code c_TalonSRX_GetFault_RevSoftLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFault_RevSoftLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_OverTemp(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetStckyFault_OverTemp(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_UnderVoltage(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetStckyFault_UnderVoltage(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_ForLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetStckyFault_ForLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_RevLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetStckyFault_RevLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_ForSoftLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetStckyFault_ForSoftLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_RevSoftLim(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetStckyFault_RevSoftLim(*param);
}
CTR_Code c_TalonSRX_GetAppliedThrottle(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetAppliedThrottle(*param);
}
CTR_Code c_TalonSRX_GetCloseLoopErr(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetCloseLoopErr(*param);
}
CTR_Code c_TalonSRX_GetFeedbackDeviceSelect(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFeedbackDeviceSelect(*param);
}
CTR_Code c_TalonSRX_GetModeSelect(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetModeSelect(*param);
}
CTR_Code c_TalonSRX_GetLimitSwitchEn(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetLimitSwitchEn(*param);
}
CTR_Code c_TalonSRX_GetLimitSwitchClosedFor(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetLimitSwitchClosedFor(*param);
}
CTR_Code c_TalonSRX_GetLimitSwitchClosedRev(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetLimitSwitchClosedRev(*param);
}
CTR_Code c_TalonSRX_GetSensorPosition(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetSensorPosition(*param);
}
CTR_Code c_TalonSRX_GetSensorVelocity(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetSensorVelocity(*param);
}
CTR_Code c_TalonSRX_GetCurrent(void *handle, double *param)
{
  return ((CanTalonSRX*)handle)->GetCurrent(*param);
}
CTR_Code c_TalonSRX_GetBrakeIsEnabled(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetBrakeIsEnabled(*param);
}
CTR_Code c_TalonSRX_GetEncPosition(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetEncPosition(*param);
}
CTR_Code c_TalonSRX_GetEncVel(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetEncVel(*param);
}
CTR_Code c_TalonSRX_GetEncIndexRiseEvents(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetEncIndexRiseEvents(*param);
}
CTR_Code c_TalonSRX_GetQuadApin(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetQuadApin(*param);
}
CTR_Code c_TalonSRX_GetQuadBpin(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetQuadBpin(*param);
}
CTR_Code c_TalonSRX_GetQuadIdxpin(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetQuadIdxpin(*param);
}
CTR_Code c_TalonSRX_GetAnalogInWithOv(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetAnalogInWithOv(*param);
}
CTR_Code c_TalonSRX_GetAnalogInVel(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetAnalogInVel(*param);
}
CTR_Code c_TalonSRX_GetTemp(void *handle, double *param)
{
  return ((CanTalonSRX*)handle)->GetTemp(*param);
}
CTR_Code c_TalonSRX_GetBatteryV(void *handle, double *param)
{
  return ((CanTalonSRX*)handle)->GetBatteryV(*param);
}
CTR_Code c_TalonSRX_GetResetCount(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetResetCount(*param);
}
CTR_Code c_TalonSRX_GetResetFlags(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetResetFlags(*param);
}
CTR_Code c_TalonSRX_GetFirmVers(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetFirmVers(*param);
}
CTR_Code c_TalonSRX_GetPulseWidthPosition(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetPulseWidthPosition(*param);
}
CTR_Code c_TalonSRX_GetPulseWidthVelocity(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetPulseWidthVelocity(*param);
}
CTR_Code c_TalonSRX_GetPulseWidthRiseToRiseUs(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetPulseWidthRiseToRiseUs(*param);
}
CTR_Code c_TalonSRX_GetActTraj_IsValid(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetActTraj_IsValid(*param);
}
CTR_Code c_TalonSRX_GetActTraj_ProfileSlotSelect(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetActTraj_ProfileSlotSelect(*param);
}
CTR_Code c_TalonSRX_GetActTraj_VelOnly(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetActTraj_VelOnly(*param);
}
CTR_Code c_TalonSRX_GetActTraj_IsLast(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetActTraj_IsLast(*param);
}
CTR_Code c_TalonSRX_GetOutputType(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetOutputType(*param);
}
CTR_Code c_TalonSRX_GetHasUnderrun(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetHasUnderrun(*param);
}
CTR_Code c_TalonSRX_GetIsUnderrun(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetIsUnderrun(*param);
}
CTR_Code c_TalonSRX_GetNextID(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetNextID(*param);
}
CTR_Code c_TalonSRX_GetBufferIsFull(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetBufferIsFull(*param);
}
CTR_Code c_TalonSRX_GetCount(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetCount(*param);
}
CTR_Code c_TalonSRX_GetActTraj_Velocity(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetActTraj_Velocity(*param);
}
CTR_Code c_TalonSRX_GetActTraj_Position(void *handle, int *param)
{
  return ((CanTalonSRX*)handle)->GetActTraj_Position(*param);
}
CTR_Code c_TalonSRX_SetDemand(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetDemand(param);
}
CTR_Code c_TalonSRX_SetOverrideLimitSwitchEn(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetOverrideLimitSwitchEn(param);
}
CTR_Code c_TalonSRX_SetFeedbackDeviceSelect(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetFeedbackDeviceSelect(param);
}
CTR_Code c_TalonSRX_SetRevMotDuringCloseLoopEn(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetRevMotDuringCloseLoopEn(param);
}
CTR_Code c_TalonSRX_SetOverrideBrakeType(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetOverrideBrakeType(param);
}
CTR_Code c_TalonSRX_SetModeSelect(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetModeSelect(param);
}
CTR_Code c_TalonSRX_SetProfileSlotSelect(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetProfileSlotSelect(param);
}
CTR_Code c_TalonSRX_SetRampThrottle(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetRampThrottle(param);
}
CTR_Code c_TalonSRX_SetRevFeedbackSensor(void *handle, int param)
{
  return ((CanTalonSRX*)handle)->SetRevFeedbackSensor(param);
}
}
