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
#ifndef CanTalonSRX_H_
#define CanTalonSRX_H_
#include "ctre/ctre.h"  //BIT Defines + Typedefs, TALON_Control_6_MotProfAddTrajPoint_t
#include "ctre/CtreCanNode.h"
#include <FRC_NetworkCommunication/CANSessionMux.h>  //CAN Comm
#include <map>
#include <atomic>
#include <deque>
#include <mutex>
class CanTalonSRX : public CtreCanNode {
 private:
  // Use this for determining whether the default move constructor has been
  // called; this prevents us from calling the destructor twice.
  struct HasBeenMoved {
    HasBeenMoved(HasBeenMoved &&other) {
      other.moved = true;
      moved = false;
    }
    HasBeenMoved() = default;
    std::atomic<bool> moved{false};
    operator bool() const { return moved; }
  } m_hasBeenMoved;

  // Vars for opening a CAN stream if caller needs signals that require
  // soliciting
  uint32_t _can_h;    //!< Session handle for catching response params.
  int32_t _can_stat;  //!< Session handle status.
  struct tCANStreamMessage _msgBuff[20];
  static int const kMsgCapacity = 20;
  typedef std::map<uint32_t, uint32_t> sigs_t;
  // Catches signal updates that are solicited.  Expect this to be very few.
  sigs_t _sigs;
  void OpenSessionIfNeedBe();
  void ProcessStreamMessages();
  /**
   * Called in various places to double check we are using the best control
   * frame.  If the Talon firmware is too old, use control 1 framing, which
   * does not allow setting control signals until robot is enabled.  If Talon
   * firmware can suport control5, use that since that frame can be
   * transmitted during robot-disable.  If calling application uses setParam
   * to set the signal eLegacyControlMode, caller can force using control1
   * if needed for some reason.
   */
  void UpdateControlId();
  /**
   * @return true if Talon is reporting that it supports control5, and therefore
   *              RIO can send control5 to update control params (even when
   *              disabled).
   */
  bool IsControl5Supported();
  /**
   * Get a copy of the control frame to send.
   * @param [out] pointer to eight byte array to fill.
   */
  void GetControlFrameCopy(uint8_t *toFill);
  /**
   * @return the tx task that transmits Control6 (motion profile control).
   *         If it's not scheduled, then schedule it.  This is part
   *         of making the lazy-framing that only peforms MotionProf framing
   *         when needed to save bandwidth.
   */
  CtreCanNode::txTask<TALON_Control_6_MotProfAddTrajPoint_t> GetControl6();
  /**
   * Caller is either pushing a new motion profile point, or is
   * calling the Process buffer routine.  In either case check our
   * flow control to see if we need to start sending control6.
   */
  void ReactToMotionProfileCall();
  /**
   * Update the NextPt signals inside the control frame given the next pt to
   * send.
   * @param control pointer to the CAN frame payload containing control6.  Only
   *                the signals that serialize the next trajectory point are
   *                updated from the contents of newPt.
   * @param newPt point to the next trajectory that needs to be inserted into
   *              Talon RAM.
   */
  void CopyTrajPtIntoControl(
      TALON_Control_6_MotProfAddTrajPoint_t *control,
      const TALON_Control_6_MotProfAddTrajPoint_t *newPt);
  //---------------------- General Control framing ---------------------------//
  /**
   * Frame period for control1 or control5, depending on which one we are using.
   */
  int _controlPeriodMs = kDefaultControlPeriodMs;
  /**
   * Frame Period of the motion profile control6 frame.
   */
  int _control6PeriodMs = kDefaultControl6PeriodMs;
  /**
   * When using control5, we still need to send a frame to enable robot.  This
   * controls the period.  This only is used when we are in the control5 state.
   * @see ControlFrameSelControl5
   */
  int _enablePeriodMs = kDefaultEnablePeriodMs;
  /**
   * ArbID to use for control frame.  Should be either CONTROL_1 or CONTROL_5.
   */
  uint32_t _controlFrameArbId;
  /**
   * Boolean flag to signal calling applications intent to allow using control5
   * assuming Talon firmware supports it.  This can be cleared to force control1
   * framing.
   */
  bool _useControl5ifSupported = true;
  //--------------------- Buffering Motion Profile ---------------------------//
  /**
   * Top level Buffer for motion profile trajectory buffering.
   * Basically this buffers up the eight byte CAN frame payloads that are
   * handshaked into the Talon RAM.
   * TODO: Should this be moved into a separate header, and if so where
   * logically should it reside?
   * TODO: Add compression so that multiple CAN frames can be compressed into
   * one exchange.
   */
  class TrajectoryBuffer {
   public:
    void Clear() { _motProfTopBuffer.clear(); }
    /**
     * push caller's uncompressed simple trajectory point.
     */
    void Push(TALON_Control_6_MotProfAddTrajPoint_huff0_t &pt) {
      _motProfTopBuffer.push_back(pt);
    }
    /**
     * Get the next trajectory point CAN frame to send.
     * Underlying layer may compress the next few points together
     * into one control_6 frame.
     */
    TALON_Control_6_MotProfAddTrajPoint_t *Front() {
      /* TODO : peek ahead and use compression strategies */
      _lastFront = _motProfTopBuffer.front();
      return (TALON_Control_6_MotProfAddTrajPoint_t *)&_lastFront;
    }
    void Pop() {
      /* TODO : pop multiple points if last front'd point was compressed. */
      _motProfTopBuffer.pop_front();
    }
    unsigned int GetNumTrajectories() { return _motProfTopBuffer.size(); }
    bool IsEmpty() { return _motProfTopBuffer.empty(); }

   private:
    std::deque<TALON_Control_6_MotProfAddTrajPoint_huff0_t> _motProfTopBuffer;
    TALON_Control_6_MotProfAddTrajPoint_huff0_t _lastFront;
  };
  TrajectoryBuffer _motProfTopBuffer;
  /**
   * To keep buffers from getting out of control, place a cap on the top level
   * buffer.  Calling application
   * can stream addition points as they are fed to Talon.
   * Approx memory footprint is this capacity X 8 bytes.
   */
  static const int kMotionProfileTopBufferCapacity = 2048;
  /**
   * Flow control for streaming trajectories.
   */
  int32_t _motProfFlowControl = -1;
  /**
   * Since we may need the MP pts to be emptied into Talon in the background
   * make sure the buffering is thread-safe.
   */
  std::mutex _mutMotProf;
  /**
   * Send a one shot frame to set an arbitrary signal.
   * Most signals are in the control frame so avoid using this API unless you
   * have to.
   * Use this api for...
   * -A motor controller profile signal eProfileParam_XXXs.  These are backed up
   * in flash.  If you are gain-scheduling then call this periodically.
   * -Default brake and limit switch signals... eOnBoot_XXXs.  Avoid doing this,
   * use the override signals in the control frame.
   * Talon will automatically send a PARAM_RESPONSE after the set, so
   * GetParamResponse will catch the latest value after a couple ms.
   */
  CTR_Code SetParamRaw(uint32_t paramEnum, int32_t rawBits);
  /**
   * Checks cached CAN frames and updating solicited signals.
   */
  CTR_Code GetParamResponseRaw(uint32_t paramEnum, int32_t &rawBits);

 public:
  // default control update rate is 10ms.
  static const int kDefaultControlPeriodMs = 10;
  // default enable update rate is 50ms (when using the new control5 frame).
  static const int kDefaultEnablePeriodMs = 50;
  // Default update rate for motion profile control 6.  This only takes effect
  // when calling uses MP functions.
  static const int kDefaultControl6PeriodMs = 10;
  explicit CanTalonSRX(int deviceNumber = 0,
                       int controlPeriodMs = kDefaultControlPeriodMs,
                       int enablePeriodMs = kDefaultEnablePeriodMs);
  ~CanTalonSRX();
  void Set(double value);
  /* mode select enumerations */
  // Demand is 11bit signed duty cycle [-1023,1023].
  static const int kMode_DutyCycle = 0;
  // Position PIDF.
  static const int kMode_PositionCloseLoop = 1;
  // Velocity PIDF.
  static const int kMode_VelocityCloseLoop = 2;
  // Current close loop - not done.
  static const int kMode_CurrentCloseLoop = 3;
  // Voltage Compensation Mode - not done.  Demand is fixed pt target 8.8 volts.
  static const int kMode_VoltCompen = 4;
  // Demand is the 6 bit Device ID of the 'master' TALON SRX.
  static const int kMode_SlaveFollower = 5;
  // Demand is '0' (Disabled), '1' (Enabled), or '2' (Hold).
  static const int kMode_MotionProfile = 6;
  // Zero the output (honors brake/coast) regardless of demand.
  // Might be useful if we need to change modes but can't atomically
  // change all the signals we want in between.
  static const int kMode_NoDrive = 15;
  /* limit switch enumerations */
  static const int kLimitSwitchOverride_UseDefaultsFromFlash = 1;
  static const int kLimitSwitchOverride_DisableFwd_DisableRev = 4;
  static const int kLimitSwitchOverride_DisableFwd_EnableRev = 5;
  static const int kLimitSwitchOverride_EnableFwd_DisableRev = 6;
  static const int kLimitSwitchOverride_EnableFwd_EnableRev = 7;
  /* brake override enumerations */
  static const int kBrakeOverride_UseDefaultsFromFlash = 0;
  static const int kBrakeOverride_OverrideCoast = 1;
  static const int kBrakeOverride_OverrideBrake = 2;
  /* feedback device enumerations */
  static const int kFeedbackDev_DigitalQuadEnc = 0;
  static const int kFeedbackDev_AnalogPot = 2;
  static const int kFeedbackDev_AnalogEncoder = 3;
  static const int kFeedbackDev_CountEveryRisingEdge = 4;
  static const int kFeedbackDev_CountEveryFallingEdge = 5;
  static const int kFeedbackDev_PosIsPulseWidth = 8;
  /* ProfileSlotSelect enumerations*/
  static const int kProfileSlotSelect_Slot0 = 0;
  static const int kProfileSlotSelect_Slot1 = 1;
  /* status frame rate types */
  static const int kStatusFrame_General = 0;
  static const int kStatusFrame_Feedback = 1;
  static const int kStatusFrame_Encoder = 2;
  static const int kStatusFrame_AnalogTempVbat = 3;
  static const int kStatusFrame_PulseWidthMeas = 4;
  static const int kStatusFrame_MotionProfile = 5;
  /* Motion Profile status bits */
  static const int kMotionProfileFlag_ActTraj_IsValid = 0x1;
  static const int kMotionProfileFlag_HasUnderrun = 0x2;
  static const int kMotionProfileFlag_IsUnderrun = 0x4;
  static const int kMotionProfileFlag_ActTraj_IsLast = 0x8;
  static const int kMotionProfileFlag_ActTraj_VelOnly = 0x10;
  /* Motion Profile Set Output */
  // Motor output is neutral, Motion Profile Executer is not running.
  static const int kMotionProf_Disabled = 0;
  // Motor output is updated from Motion Profile Executer, MPE will
  // process the buffered points.
  static const int kMotionProf_Enable = 1;
  // Motor output is updated from Motion Profile Executer, MPE will
  // stay processing current trajectory point.
  static const int kMotionProf_Hold = 2;
  /**
   * Signal enumeration for generic signal access.
   * Although every signal is enumerated, only use this for traffic that must
   * be solicited.
   * Use the auto generated getters/setters at bottom of this header as much as
   * possible.
   */
  enum param_t {
    eProfileParamSlot0_P = 1,
    eProfileParamSlot0_I = 2,
    eProfileParamSlot0_D = 3,
    eProfileParamSlot0_F = 4,
    eProfileParamSlot0_IZone = 5,
    eProfileParamSlot0_CloseLoopRampRate = 6,
    eProfileParamSlot1_P = 11,
    eProfileParamSlot1_I = 12,
    eProfileParamSlot1_D = 13,
    eProfileParamSlot1_F = 14,
    eProfileParamSlot1_IZone = 15,
    eProfileParamSlot1_CloseLoopRampRate = 16,
    eProfileParamSoftLimitForThreshold = 21,
    eProfileParamSoftLimitRevThreshold = 22,
    eProfileParamSoftLimitForEnable = 23,
    eProfileParamSoftLimitRevEnable = 24,
    eOnBoot_BrakeMode = 31,
    eOnBoot_LimitSwitch_Forward_NormallyClosed = 32,
    eOnBoot_LimitSwitch_Reverse_NormallyClosed = 33,
    eOnBoot_LimitSwitch_Forward_Disable = 34,
    eOnBoot_LimitSwitch_Reverse_Disable = 35,
    eFault_OverTemp = 41,
    eFault_UnderVoltage = 42,
    eFault_ForLim = 43,
    eFault_RevLim = 44,
    eFault_HardwareFailure = 45,
    eFault_ForSoftLim = 46,
    eFault_RevSoftLim = 47,
    eStckyFault_OverTemp = 48,
    eStckyFault_UnderVoltage = 49,
    eStckyFault_ForLim = 50,
    eStckyFault_RevLim = 51,
    eStckyFault_ForSoftLim = 52,
    eStckyFault_RevSoftLim = 53,
    eAppliedThrottle = 61,
    eCloseLoopErr = 62,
    eFeedbackDeviceSelect = 63,
    eRevMotDuringCloseLoopEn = 64,
    eModeSelect = 65,
    eProfileSlotSelect = 66,
    eRampThrottle = 67,
    eRevFeedbackSensor = 68,
    eLimitSwitchEn = 69,
    eLimitSwitchClosedFor = 70,
    eLimitSwitchClosedRev = 71,
    eSensorPosition = 73,
    eSensorVelocity = 74,
    eCurrent = 75,
    eBrakeIsEnabled = 76,
    eEncPosition = 77,
    eEncVel = 78,
    eEncIndexRiseEvents = 79,
    eQuadApin = 80,
    eQuadBpin = 81,
    eQuadIdxpin = 82,
    eAnalogInWithOv = 83,
    eAnalogInVel = 84,
    eTemp = 85,
    eBatteryV = 86,
    eResetCount = 87,
    eResetFlags = 88,
    eFirmVers = 89,
    eSettingsChanged = 90,
    eQuadFilterEn = 91,
    ePidIaccum = 93,
    eStatus1FrameRate = 94,  // TALON_Status_1_General_10ms_t
    eStatus2FrameRate = 95,  // TALON_Status_2_Feedback_20ms_t
    eStatus3FrameRate = 96,  // TALON_Status_3_Enc_100ms_t
    eStatus4FrameRate = 97,  // TALON_Status_4_AinTempVbat_100ms_t
    eStatus6FrameRate = 98,  // TALON_Status_6_Eol_t
    eStatus7FrameRate = 99,  // TALON_Status_7_Debug_200ms_t
    eClearPositionOnIdx = 100,
    // reserved,
    // reserved,
    // reserved,
    ePeakPosOutput = 104,
    eNominalPosOutput = 105,
    ePeakNegOutput = 106,
    eNominalNegOutput = 107,
    eQuadIdxPolarity = 108,
    eStatus8FrameRate = 109,  // TALON_Status_8_PulseWid_100ms_t
    eAllowPosOverflow = 110,
    eProfileParamSlot0_AllowableClosedLoopErr = 111,
    eNumberPotTurns = 112,
    eNumberEncoderCPR = 113,
    ePwdPosition = 114,
    eAinPosition = 115,
    eProfileParamVcompRate = 116,
    eProfileParamSlot1_AllowableClosedLoopErr = 117,
    eStatus9FrameRate = 118,  // TALON_Status_9_MotProfBuffer_100ms_t
    eMotionProfileHasUnderrunErr = 119,
    eReserved120 = 120,
    eLegacyControlMode = 121,
  };
  //---- setters and getters that use the solicated param request/response ---//
  /**
   * Send a one shot frame to set an arbitrary signal.
   * Most signals are in the control frame so avoid using this API unless you
   * have to.
   * Use this api for...
   * -A motor controller profile signal eProfileParam_XXXs.  These are backed
   * up in flash.  If you are gain-scheduling then call this periodically.
   * -Default brake and limit switch signals... eOnBoot_XXXs.  Avoid doing
   * this, use the override signals in the control frame.
   * Talon will automatically send a PARAM_RESPONSE after the set, so
   * GetParamResponse will catch the latest value after a couple ms.
   */
  CTR_Code SetParam(param_t paramEnum, double value);
  /**
   * Asks TALON to immedietely respond with signal value.  This API is only used
   * for signals that are not sent periodically.
   * This can be useful for reading params that rarely change like Limit Switch
   * settings and PIDF values.
   * @param param to request.
   */
  CTR_Code RequestParam(param_t paramEnum);
  CTR_Code GetParamResponse(param_t paramEnum, double &value);
  CTR_Code GetParamResponseInt32(param_t paramEnum, int &value);
  //----------- getters and setters that use param request/response ----------//
  /**
   * These signals are backed up in flash and will survive a power cycle.
   * If your application requires changing these values consider using both
   * slots and switch between slot0 <=> slot1.
   * If your application requires changing these signals frequently then it
   * makes sense to leverage this API.
   * Getters don't block, so it may require several calls to get the latest
   * value.
   */
  CTR_Code SetPgain(unsigned slotIdx, double gain);
  CTR_Code SetIgain(unsigned slotIdx, double gain);
  CTR_Code SetDgain(unsigned slotIdx, double gain);
  CTR_Code SetFgain(unsigned slotIdx, double gain);
  CTR_Code SetIzone(unsigned slotIdx, int zone);
  CTR_Code SetCloseLoopRampRate(unsigned slotIdx, int closeLoopRampRate);
  CTR_Code SetVoltageCompensationRate(double voltagePerMs);
  CTR_Code SetSensorPosition(int pos);
  CTR_Code SetForwardSoftLimit(int forwardLimit);
  CTR_Code SetReverseSoftLimit(int reverseLimit);
  CTR_Code SetForwardSoftEnable(int enable);
  CTR_Code SetReverseSoftEnable(int enable);
  CTR_Code GetPgain(unsigned slotIdx, double &gain);
  CTR_Code GetIgain(unsigned slotIdx, double &gain);
  CTR_Code GetDgain(unsigned slotIdx, double &gain);
  CTR_Code GetFgain(unsigned slotIdx, double &gain);
  CTR_Code GetIzone(unsigned slotIdx, int &zone);
  CTR_Code GetCloseLoopRampRate(unsigned slotIdx, int &closeLoopRampRate);
  CTR_Code GetVoltageCompensationRate(double &voltagePerMs);
  CTR_Code GetForwardSoftLimit(int &forwardLimit);
  CTR_Code GetReverseSoftLimit(int &reverseLimit);
  CTR_Code GetForwardSoftEnable(int &enable);
  CTR_Code GetReverseSoftEnable(int &enable);
  CTR_Code GetPulseWidthRiseToFallUs(int &param);
  CTR_Code IsPulseWidthSensorPresent(int &param);
  CTR_Code SetModeSelect(int modeSelect, int demand);
  /**
   * Change the periodMs of a TALON's status frame.  See kStatusFrame_* enums
   * for what's available.
   */
  CTR_Code SetStatusFrameRate(unsigned frameEnum, unsigned periodMs);
  /**
   * Clear all sticky faults in TALON.
   */
  CTR_Code ClearStickyFaults();
  /**
   * Calling application can opt to speed up the handshaking between the robot
   * API and the Talon to increase the
   * download rate of the Talon's Motion Profile.  Ideally the period should be
   * no more than half the period
   * of a trajectory point.
   */
  void ChangeMotionControlFramePeriod(uint32_t periodMs);
  /**
   * Clear the buffered motion profile in both Talon RAM (bottom), and in the
   * API (top).
   */
  void ClearMotionProfileTrajectories();
  /**
   * Retrieve just the buffer count for the api-level (top) buffer.
   * This routine performs no CAN or data structure lookups, so its fast and
   * ideal if caller needs to quickly poll the progress of trajectory points
   * being emptied into Talon's RAM. Otherwise just use GetMotionProfileStatus.
   * @return number of trajectory points in the top buffer.
   */
  uint32_t GetMotionProfileTopLevelBufferCount();
  /**
   * Retrieve just the buffer full for the api-level (top) buffer.
   * This routine performs no CAN or data structure lookups, so its fast and
   * ideal if caller needs to quickly poll. Otherwise just use
   * GetMotionProfileStatus.
   * @return number of trajectory points in the top buffer.
   */
  bool IsMotionProfileTopLevelBufferFull();
  /**
   * Push another trajectory point into the top level buffer (which is emptied
   * into the Talon's bottom buffer as room allows).
   * @param targPos  servo position in native Talon units (sensor units).
   * @param targVel  velocity to feed-forward in native Talon units (sensor
   *                 units per 100ms).
   * @param profileSlotSelect  which slot to pull PIDF gains from.  Currently
   *                           supports 0 or 1.
   * @param timeDurMs  time in milliseconds of how long to apply this point.
   * @param velOnly  set to nonzero to signal Talon that only the feed-foward
   *                 velocity should be used, i.e. do not perform PID on
   *                 position.  This is equivalent to setting PID gains to zero,
   *                 but much more efficient and synchronized to MP.
   * @param isLastPoint  set to nonzero to signal Talon to keep processing this
   *                     trajectory point, instead of jumping to the next one
   *                     when timeDurMs expires.  Otherwise MP executer will
   *                     eventually see an empty buffer after the last point
   *                     expires, causing it to assert the IsUnderRun flag.
   *                     However this may be desired if calling application
   *                     nevers wants to terminate the MP.
   * @param zeroPos  set to nonzero to signal Talon to "zero" the selected
   *                 position sensor before executing this trajectory point.
   *                 Typically the first point should have this set only thus
   *                 allowing the remainder of the MP positions to be relative
   *                 to zero.
   * @return CTR_OKAY if trajectory point push ok. CTR_BufferFull if buffer is
   *         full due to kMotionProfileTopBufferCapacity.
   */
  CTR_Code PushMotionProfileTrajectory(int targPos, int targVel,
                                       int profileSlotSelect, int timeDurMs,
                                       int velOnly, int isLastPoint,
                                       int zeroPos);
  /**
   * This must be called periodically to funnel the trajectory points from the
   * API's top level buffer to the Talon's bottom level buffer.  Recommendation
   * is to call this twice as fast as the executation rate of the motion
   * profile.  So if MP is running with 20ms trajectory points, try calling
   * this routine every 10ms.  All motion profile functions are thread-safe
   * through the use of a mutex, so there is no harm in having the caller
   * utilize threading.
   */
  void ProcessMotionProfileBuffer();
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
  CTR_Code GetMotionProfileStatus(uint32_t &flags, uint32_t &profileSlotSelect,
                                  int32_t &targPos, int32_t &targVel,
                                  uint32_t &topBufferRemaining,
                                  uint32_t &topBufferCnt,
                                  uint32_t &btmBufferCnt,
                                  uint32_t &outputEnable);
//------------------------ auto generated ------------------------------------//
/* This API is optimal since it uses the fire-and-forget CAN interface.
 * These signals should cover the majority of all use cases.
 */
  CTR_Code GetFault_OverTemp(int &param);
  CTR_Code GetFault_UnderVoltage(int &param);
  CTR_Code GetFault_ForLim(int &param);
  CTR_Code GetFault_RevLim(int &param);
  CTR_Code GetFault_HardwareFailure(int &param);
  CTR_Code GetFault_ForSoftLim(int &param);
  CTR_Code GetFault_RevSoftLim(int &param);
  CTR_Code GetStckyFault_OverTemp(int &param);
  CTR_Code GetStckyFault_UnderVoltage(int &param);
  CTR_Code GetStckyFault_ForLim(int &param);
  CTR_Code GetStckyFault_RevLim(int &param);
  CTR_Code GetStckyFault_ForSoftLim(int &param);
  CTR_Code GetStckyFault_RevSoftLim(int &param);
  CTR_Code GetAppliedThrottle(int &param);
  CTR_Code GetCloseLoopErr(int &param);
  CTR_Code GetFeedbackDeviceSelect(int &param);
  CTR_Code GetModeSelect(int &param);
  CTR_Code GetLimitSwitchEn(int &param);
  CTR_Code GetLimitSwitchClosedFor(int &param);
  CTR_Code GetLimitSwitchClosedRev(int &param);
  CTR_Code GetSensorPosition(int &param);
  CTR_Code GetSensorVelocity(int &param);
  CTR_Code GetCurrent(double &param);
  CTR_Code GetBrakeIsEnabled(int &param);
  CTR_Code GetEncPosition(int &param);
  CTR_Code GetEncVel(int &param);
  CTR_Code GetEncIndexRiseEvents(int &param);
  CTR_Code GetQuadApin(int &param);
  CTR_Code GetQuadBpin(int &param);
  CTR_Code GetQuadIdxpin(int &param);
  CTR_Code GetAnalogInWithOv(int &param);
  CTR_Code GetAnalogInVel(int &param);
  CTR_Code GetTemp(double &param);
  CTR_Code GetBatteryV(double &param);
  CTR_Code GetResetCount(int &param);
  CTR_Code GetResetFlags(int &param);
  CTR_Code GetFirmVers(int &param);
  CTR_Code GetPulseWidthPosition(int &param);
  CTR_Code GetPulseWidthVelocity(int &param);
  CTR_Code GetPulseWidthRiseToRiseUs(int &param);
  CTR_Code GetActTraj_IsValid(int &param);
  CTR_Code GetActTraj_ProfileSlotSelect(int &param);
  CTR_Code GetActTraj_VelOnly(int &param);
  CTR_Code GetActTraj_IsLast(int &param);
  CTR_Code GetOutputType(int &param);
  CTR_Code GetHasUnderrun(int &param);
  CTR_Code GetIsUnderrun(int &param);
  CTR_Code GetNextID(int &param);
  CTR_Code GetBufferIsFull(int &param);
  CTR_Code GetCount(int &param);
  CTR_Code GetActTraj_Velocity(int &param);
  CTR_Code GetActTraj_Position(int &param);
  CTR_Code SetDemand(int param);
  CTR_Code SetOverrideLimitSwitchEn(int param);
  CTR_Code SetFeedbackDeviceSelect(int param);
  CTR_Code SetRevMotDuringCloseLoopEn(int param);
  CTR_Code SetOverrideBrakeType(int param);
  CTR_Code SetModeSelect(int param);
  CTR_Code SetProfileSlotSelect(int param);
  CTR_Code SetRampThrottle(int param);
  CTR_Code SetRevFeedbackSensor(int param);
};
extern "C" {
  void *c_TalonSRX_Create3(int deviceNumber, int controlPeriodMs, int enablePeriodMs);
  void *c_TalonSRX_Create2(int deviceNumber, int controlPeriodMs);
  void *c_TalonSRX_Create1(int deviceNumber);
  void c_TalonSRX_Destroy(void *handle);
  void c_TalonSRX_Set(void *handle, double value);
  CTR_Code c_TalonSRX_SetParam(void *handle, int paramEnum, double value);
  CTR_Code c_TalonSRX_RequestParam(void *handle, int paramEnum);
  CTR_Code c_TalonSRX_GetParamResponse(void *handle, int paramEnum, double *value);
  CTR_Code c_TalonSRX_GetParamResponseInt32(void *handle, int paramEnum, int *value);
  CTR_Code c_TalonSRX_SetPgain(void *handle, int slotIdx, double gain);
  CTR_Code c_TalonSRX_SetIgain(void *handle, int slotIdx, double gain);
  CTR_Code c_TalonSRX_SetDgain(void *handle, int slotIdx, double gain);
  CTR_Code c_TalonSRX_SetFgain(void *handle, int slotIdx, double gain);
  CTR_Code c_TalonSRX_SetIzone(void *handle, int slotIdx, int zone);
  CTR_Code c_TalonSRX_SetCloseLoopRampRate(void *handle, int slotIdx, int closeLoopRampRate);
  CTR_Code c_TalonSRX_SetVoltageCompensationRate(void *handle, double voltagePerMs);
  CTR_Code c_TalonSRX_SetSensorPosition(void *handle, int pos);
  CTR_Code c_TalonSRX_SetForwardSoftLimit(void *handle, int forwardLimit);
  CTR_Code c_TalonSRX_SetReverseSoftLimit(void *handle, int reverseLimit);
  CTR_Code c_TalonSRX_SetForwardSoftEnable(void *handle, int enable);
  CTR_Code c_TalonSRX_SetReverseSoftEnable(void *handle, int enable);
  CTR_Code c_TalonSRX_GetPgain(void *handle, int slotIdx, double *gain);
  CTR_Code c_TalonSRX_GetIgain(void *handle, int slotIdx, double *gain);
  CTR_Code c_TalonSRX_GetDgain(void *handle, int slotIdx, double *gain);
  CTR_Code c_TalonSRX_GetFgain(void *handle, int slotIdx, double *gain);
  CTR_Code c_TalonSRX_GetIzone(void *handle, int slotIdx, int *zone);
  CTR_Code c_TalonSRX_GetCloseLoopRampRate(void *handle, int slotIdx, int *closeLoopRampRate);
  CTR_Code c_TalonSRX_GetVoltageCompensationRate(void *handle, double *voltagePerMs);
  CTR_Code c_TalonSRX_GetForwardSoftLimit(void *handle, int *forwardLimit);
  CTR_Code c_TalonSRX_GetReverseSoftLimit(void *handle, int *reverseLimit);
  CTR_Code c_TalonSRX_GetForwardSoftEnable(void *handle, int *enable);
  CTR_Code c_TalonSRX_GetReverseSoftEnable(void *handle, int *enable);
  CTR_Code c_TalonSRX_GetPulseWidthRiseToFallUs(void *handle, int *param);
  CTR_Code c_TalonSRX_IsPulseWidthSensorPresent(void *handle, int *param);
  CTR_Code c_TalonSRX_SetModeSelect2(void *handle, int modeSelect, int demand);
  CTR_Code c_TalonSRX_SetStatusFrameRate(void *handle, int frameEnum, int periodMs);
  CTR_Code c_TalonSRX_ClearStickyFaults(void *handle);
  void c_TalonSRX_ChangeMotionControlFramePeriod(void *handle, int periodMs);
  void c_TalonSRX_ClearMotionProfileTrajectories(void *handle);
  int c_TalonSRX_GetMotionProfileTopLevelBufferCount(void *handle);
  int c_TalonSRX_IsMotionProfileTopLevelBufferFull(void *handle);
  CTR_Code c_TalonSRX_PushMotionProfileTrajectory(void *handle, int targPos, int targVel, int profileSlotSelect, int timeDurMs, int velOnly, int isLastPoint, int zeroPos);
  void c_TalonSRX_ProcessMotionProfileBuffer(void *handle);
  CTR_Code c_TalonSRX_GetMotionProfileStatus(void *handle, int *flags, int *profileSlotSelect, int *targPos, int *targVel, int *topBufferRemaining, int *topBufferCnt, int *btmBufferCnt, int *outputEnable);
  CTR_Code c_TalonSRX_GetFault_OverTemp(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFault_UnderVoltage(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFault_ForLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFault_RevLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFault_HardwareFailure(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFault_ForSoftLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFault_RevSoftLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetStckyFault_OverTemp(void *handle, int *param);
  CTR_Code c_TalonSRX_GetStckyFault_UnderVoltage(void *handle, int *param);
  CTR_Code c_TalonSRX_GetStckyFault_ForLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetStckyFault_RevLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetStckyFault_ForSoftLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetStckyFault_RevSoftLim(void *handle, int *param);
  CTR_Code c_TalonSRX_GetAppliedThrottle(void *handle, int *param);
  CTR_Code c_TalonSRX_GetCloseLoopErr(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFeedbackDeviceSelect(void *handle, int *param);
  CTR_Code c_TalonSRX_GetModeSelect(void *handle, int *param);
  CTR_Code c_TalonSRX_GetLimitSwitchEn(void *handle, int *param);
  CTR_Code c_TalonSRX_GetLimitSwitchClosedFor(void *handle, int *param);
  CTR_Code c_TalonSRX_GetLimitSwitchClosedRev(void *handle, int *param);
  CTR_Code c_TalonSRX_GetSensorPosition(void *handle, int *param);
  CTR_Code c_TalonSRX_GetSensorVelocity(void *handle, int *param);
  CTR_Code c_TalonSRX_GetCurrent(void *handle, double *param);
  CTR_Code c_TalonSRX_GetBrakeIsEnabled(void *handle, int *param);
  CTR_Code c_TalonSRX_GetEncPosition(void *handle, int *param);
  CTR_Code c_TalonSRX_GetEncVel(void *handle, int *param);
  CTR_Code c_TalonSRX_GetEncIndexRiseEvents(void *handle, int *param);
  CTR_Code c_TalonSRX_GetQuadApin(void *handle, int *param);
  CTR_Code c_TalonSRX_GetQuadBpin(void *handle, int *param);
  CTR_Code c_TalonSRX_GetQuadIdxpin(void *handle, int *param);
  CTR_Code c_TalonSRX_GetAnalogInWithOv(void *handle, int *param);
  CTR_Code c_TalonSRX_GetAnalogInVel(void *handle, int *param);
  CTR_Code c_TalonSRX_GetTemp(void *handle, double *param);
  CTR_Code c_TalonSRX_GetBatteryV(void *handle, double *param);
  CTR_Code c_TalonSRX_GetResetCount(void *handle, int *param);
  CTR_Code c_TalonSRX_GetResetFlags(void *handle, int *param);
  CTR_Code c_TalonSRX_GetFirmVers(void *handle, int *param);
  CTR_Code c_TalonSRX_GetPulseWidthPosition(void *handle, int *param);
  CTR_Code c_TalonSRX_GetPulseWidthVelocity(void *handle, int *param);
  CTR_Code c_TalonSRX_GetPulseWidthRiseToRiseUs(void *handle, int *param);
  CTR_Code c_TalonSRX_GetActTraj_IsValid(void *handle, int *param);
  CTR_Code c_TalonSRX_GetActTraj_ProfileSlotSelect(void *handle, int *param);
  CTR_Code c_TalonSRX_GetActTraj_VelOnly(void *handle, int *param);
  CTR_Code c_TalonSRX_GetActTraj_IsLast(void *handle, int *param);
  CTR_Code c_TalonSRX_GetOutputType(void *handle, int *param);
  CTR_Code c_TalonSRX_GetHasUnderrun(void *handle, int *param);
  CTR_Code c_TalonSRX_GetIsUnderrun(void *handle, int *param);
  CTR_Code c_TalonSRX_GetNextID(void *handle, int *param);
  CTR_Code c_TalonSRX_GetBufferIsFull(void *handle, int *param);
  CTR_Code c_TalonSRX_GetCount(void *handle, int *param);
  CTR_Code c_TalonSRX_GetActTraj_Velocity(void *handle, int *param);
  CTR_Code c_TalonSRX_GetActTraj_Position(void *handle, int *param);
  CTR_Code c_TalonSRX_SetDemand(void *handle, int param);
  CTR_Code c_TalonSRX_SetOverrideLimitSwitchEn(void *handle, int param);
  CTR_Code c_TalonSRX_SetFeedbackDeviceSelect(void *handle, int param);
  CTR_Code c_TalonSRX_SetRevMotDuringCloseLoopEn(void *handle, int param);
  CTR_Code c_TalonSRX_SetOverrideBrakeType(void *handle, int param);
  CTR_Code c_TalonSRX_SetModeSelect(void *handle, int param);
  CTR_Code c_TalonSRX_SetProfileSlotSelect(void *handle, int param);
  CTR_Code c_TalonSRX_SetRampThrottle(void *handle, int param);
  CTR_Code c_TalonSRX_SetRevFeedbackSensor(void *handle, int param);
}
#endif
