/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

import edu.wpi.first.wpilibj.MotorSafety;
import edu.wpi.first.wpilibj.MotorSafetyHelper;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.can.CANExceptionFactory;
import edu.wpi.first.wpilibj.can.CANJaguarVersionException;
import edu.wpi.first.wpilibj.can.CANJNI;
import edu.wpi.first.wpilibj.can.CANTimeoutException;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.HALLibrary;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

public class CANJaguar implements MotorSafety, PIDOutput, SpeedController, LiveWindowSendable {

	public static final int kMaxMessageDataSize = 8; // XXX: Where does this come from?

    // The internal PID control loop in the Jaguar runs at 1kHz.
    public static final int kControllerRate = 1000;
    public static final double kApproxBusVoltage = 12.0;

    /**
     * Mode determines how the Jaguar is controlled
     */
    public static class ControlMode {

        public final int value;
        static final int kPercentVbus_val = 0;
        static final int kCurrent_val = 1;
        static final int kSpeed_val = 2;
        static final int kPosition_val = 3;
        static final int kVoltage_val = 4;
        public static final ControlMode kPercentVbus = new ControlMode(kPercentVbus_val);
        public static final ControlMode kCurrent = new ControlMode(kCurrent_val);
        public static final ControlMode kSpeed = new ControlMode(kSpeed_val);
        public static final ControlMode kPosition = new ControlMode(kPosition_val);
        public static final ControlMode kVoltage = new ControlMode(kVoltage_val);

        private ControlMode(int value) {
            this.value = value;
        }
    }

    /**
     * Faults reported by the Jaguar
     */
    public static class Faults {

        public final int value;
        static final int kCurrentFault_val = 1;
        static final int kTemperatureFault_val = 2;
        static final int kBusVoltageFault_val = 4;
        static final int kGateDriverFault_val = 8;
        public static final Faults kCurrentFault = new Faults(kCurrentFault_val);
        public static final Faults kTemperatureFault = new Faults(kTemperatureFault_val);
        public static final Faults kBusVoltageFault = new Faults(kBusVoltageFault_val);
        public static final Faults kGateDriverFault = new Faults(kGateDriverFault_val);

        private Faults(int value) {
            this.value = value;
        }
    }

    /**
     * Limit switch masks
     */
    public static class Limits {

        public final int value;
        static final int kForwardLimit_val = 1;
        static final int kReverseLimit_val = 2;
        public static final Limits kForwardLimit = new Limits(kForwardLimit_val);
        public static final Limits kReverseLimit = new Limits(kReverseLimit_val);

        private Limits(int value) {
            this.value = value;
        }
    }

    /**
     * Determines which sensor to use for position reference.
     */
    public static class PositionReference {

        public final byte value;
        static final byte kQuadEncoder_val = 0;
        static final byte kPotentiometer_val = 1;
        static final byte kNone_val = (byte)0xFF;
        public static final PositionReference kQuadEncoder = new PositionReference(kQuadEncoder_val);
        public static final PositionReference kPotentiometer = new PositionReference(kPotentiometer_val);
        public static final PositionReference kNone = new PositionReference(kNone_val);

        private PositionReference(byte value) {
            this.value = value;
        }
    }

    /**
     * Determines which sensor to use for speed reference.
     */
    public static class SpeedReference {

        public final byte value;
        static final byte kEncoder_val = 0;
        static final byte kInvEncoder_val = 2;
        static final byte kQuadEncoder_val = 3;
        static final byte kNone_val = (byte)0xFF;
        public static final SpeedReference kEncoder = new SpeedReference(kEncoder_val);
        public static final SpeedReference kInvEncoder = new SpeedReference(kInvEncoder_val);
        public static final SpeedReference kQuadEncoder = new SpeedReference(kQuadEncoder_val);
        public static final SpeedReference kNone = new SpeedReference(kNone_val);

        private SpeedReference(byte value) {
            this.value = value;
        }
    }

    /**
     * Determines how the Jaguar behaves when sending a zero signal.
     */
    public static class NeutralMode {

        public final byte value;
        static final byte kJumper_val = 0;
        static final byte kBrake_val = 1;
        static final byte kCoast_val = 2;
        public static final NeutralMode kJumper = new NeutralMode(kJumper_val);
        public static final NeutralMode kBrake = new NeutralMode(kBrake_val);
        public static final NeutralMode kCoast = new NeutralMode(kCoast_val);

        private NeutralMode(byte value) {
            this.value = value;
        }
    }

    /**
     * Determines which sensor to use for position reference.
     */
    public static class LimitMode {

        public final byte value;
        static final byte kSwitchInputsOnly_val = 0;
        static final byte kSoftPositionLimit_val = 1;
        public static final LimitMode kSwitchInputsOnly = new LimitMode(kSwitchInputsOnly_val);
        public static final LimitMode kSoftPostionLimits = new LimitMode(kSoftPositionLimit_val);

        private LimitMode(byte value) {
            this.value = value;
        }
    }

    private final Object m_transactionMutex = new Object();
    private byte m_deviceNumber;
    private ControlMode m_controlMode;
    private double m_maxOutputVoltage;
    //private Semaphore m_receiveSemaphore;
    private MotorSafetyHelper m_safetyHelper;
    private static final byte[] kNoData = new byte[0];

    private final static int swap16(int x, byte[] buffer) {
        buffer[0] = (byte) ((x >>> 8) & 0x00FF);
        buffer[1] = (byte) x;
        return ((((x) >>> 8) & 0x00FF) | (((x) << 8) & 0xFF00));
    }

    private final static long swap32(long x, byte[] buffer) {
        buffer[0] = (byte) ((x >>> 24) & 0x00FF);
        buffer[1] = (byte) ((x >>> 16) & 0x00FF);
        buffer[2] = (byte) ((x >>> 8) & 0x00FF);
        buffer[3] = (byte) x;
        return ((((x) >> 24) & 0x000000FF) | (((x) >> 8) & 0x0000FF00) | (((x) << 8) & 0x00FF0000) | (((x) << 24) & 0xFF000000));
    }

    /**
     * Unpack 16-bit data from a buffer in little-endian byte order
     * @param buffer The buffer to unpack from
     * @param offset The offset into he buffer to unpack
     * @return The data that was unpacked
     */
    private static final short unpack16(byte[] buffer, int offset) {
        return (short) (((int) buffer[offset] & 0xFF) | (short) ((buffer[offset + 1] << 8)) & 0xFF00);
    }

    /**
     * Unpack 32-bit data from a buffer in little-endian byte order
     * @param buffer The buffer to unpack from
     * @param offset The offset into he buffer to unpack
     * @return The data that was unpacked
     */
    private static final int unpack32(byte[] buffer, int offset) {
        return ((int) buffer[offset] & 0xFF) | ((buffer[offset + 1] << 8) & 0xFF00) |
               ((buffer[offset + 2] << 16) & 0xFF0000) | ((buffer[offset + 3] << 24) & 0xFF000000);
    }
    private static final int kFullMessageIDMask = 0x1FFFFFC0;

    /**
     * Common initialization code called by all constructors.
     */
    private void initCANJaguar() throws CANTimeoutException {
        if (m_deviceNumber < 1 || m_deviceNumber > 63) {
            throw new RuntimeException("Invalid CAN device number \"" +
                                       m_deviceNumber + "\" - must be between 1 and 63.");
        }

        // VxWorks semaphore for calling async CAN receive API.
        //Semaphore.Options options = new Semaphore.Options();
        //options.setPrioritySorted(true);
        //m_receiveSemaphore = new Semaphore(options, false);

        int fwVer = getFirmwareVersion();
        if (fwVer >= CANJaguarVersionException.kMinRDKFirmwareVersion ||
                fwVer < CANJaguarVersionException.kMinLegalFIRSTFirmwareVersion) {
            throw new CANJaguarVersionException(m_deviceNumber, fwVer);
        }

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
        case ControlMode.kVoltage_val:
            enableControl();
            break;
        default:
            break;
        }
        m_safetyHelper = new MotorSafetyHelper(this);

        UsageReporting.report(tResourceType.kResourceType_CANJaguar, m_deviceNumber, m_controlMode.value);
        LiveWindow.addActuator("CANJaguar", m_deviceNumber, 0, this);
    }

    /**
     * Constructor
     * Default to percent Vbus control mode.
     * @param deviceNumber The address of the Jaguar on the CAN bus.
     */
    public CANJaguar(int deviceNumber) throws CANTimeoutException {
        m_deviceNumber = (byte) deviceNumber;
        m_controlMode = ControlMode.kPercentVbus;
        m_maxOutputVoltage = kApproxBusVoltage;
        initCANJaguar();
    }

    /**
     * Constructor
     * @param deviceNumber The address of the Jaguar on the CAN bus.
     * @param controlMode The control mode that the Jaguar will run in.
     */
    public CANJaguar(int deviceNumber, ControlMode controlMode) throws CANTimeoutException {
        m_deviceNumber = (byte) deviceNumber;
        m_controlMode = controlMode;
        m_maxOutputVoltage = kApproxBusVoltage;
        initCANJaguar();
    }

    /**
     * Set the output set-point value.
     *
     * The scale and the units depend on the mode the Jaguar is in.
     * In PercentVbus Mode, the outputValue is from -1.0 to 1.0 (same as PWM Jaguar).
     * In Voltage Mode, the outputValue is in Volts.
     * In Current Mode, the outputValue is in Amps.
     * In Speed Mode, the outputValue is in Rotations/Minute.
     * In Position Mode, the outputValue is in Rotations.
     *
     * @param outputValue The set-point to sent to the motor controller.
     */
    public void setX(double outputValue) throws CANTimeoutException {
        setX(outputValue, (byte) 0);
    }

    /**
     * Set the output set-point value.
     *
     * Needed by the SpeedControl interface (swallows CANTimeoutExceptions).
     *
     * @deprecated Use setX instead.
     * @param outputValue The set-point to sent to the motor controller.
     */
    public void set(double outputValue) {
        set(outputValue, (byte) 0);
    }

    /**
     * Set the output set-point value.
     *
     * The scale and the units depend on the mode the Jaguar is in.
     * In PercentVbus Mode, the outputValue is from -1.0 to 1.0 (same as PWM Jaguar).
     * In Voltage Mode, the outputValue is in Volts.
     * In Current Mode, the outputValue is in Amps.
     * In Speed Mode, the outputValue is in Rotations/Minute.
     * In Position Mode, the outputValue is in Rotations.
     *
     * @param outputValue The set-point to sent to the motor controller.
     * @param syncGroup The update group to add this set() to, pending updateSyncGroup().  If 0, update immediately.
     */
    public void setX(double outputValue, byte syncGroup) throws CANTimeoutException {
        int messageID = 0;
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        if (!m_safetyHelper.isAlive()) {
            enableControl();
        }

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
            messageID = CANJNI.LM_API_VOLT_T_SET;
            if (outputValue > 1.0) outputValue = 1.0;
            if (outputValue < -1.0) outputValue = -1.0;
            packPercentage(dataBuffer, outputValue);
            dataSize = 2;
            break;
        case ControlMode.kSpeed_val: {
            messageID = CANJNI.LM_API_SPD_T_SET;
            dataSize = packFXP16_16(dataBuffer, outputValue);
        }
        break;
        case ControlMode.kPosition_val: {
            messageID = CANJNI.LM_API_POS_T_SET;
            dataSize = packFXP16_16(dataBuffer, outputValue);
        }
        break;
        case ControlMode.kCurrent_val: {
            messageID = CANJNI.LM_API_ICTRL_T_SET;
            dataSize = packFXP8_8(dataBuffer, outputValue);
        }
        break;
        case ControlMode.kVoltage_val: {
            messageID = CANJNI.LM_API_VCOMP_T_SET;
            dataSize = packFXP8_8(dataBuffer, outputValue);
        }
        break;
        default:
            return;
        }
        if (syncGroup != 0) {
            dataBuffer[dataSize] = syncGroup;
            dataSize++;
        }
        setTransaction(messageID, dataBuffer, dataSize);
        m_safetyHelper.feed();
    }

    /**
     * Set the output set-point value.
     *
     * Needed by the SpeedControl interface (swallows CANTimeoutExceptions).
     *
     * @deprecated Use setX instead.
     * @param outputValue The set-point to sent to the motor controller.
     * @param syncGroup The update group to add this set() to, pending updateSyncGroup().  If 0, update immediately.
     */
    public void set(double outputValue, byte syncGroup) {
        try {
            setX(outputValue, syncGroup);
        } catch (CANTimeoutException e) {}
    }

    /**
     * Get the recently set outputValue setpoint.
     *
     * The scale and the units depend on the mode the Jaguar is in.
     * In PercentVbus Mode, the outputValue is from -1.0 to 1.0 (same as PWM Jaguar).
     * In Voltage Mode, the outputValue is in Volts.
     * In Current Mode, the outputValue is in Amps.
     * In Speed Mode, the outputValue is in Rotations/Minute.
     * In Position Mode, the outputValue is in Rotations.
     *
     * @return The most recently set outputValue setpoint.
     */
    public double getX() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
            dataSize = getTransaction(CANJNI.LM_API_VOLT_SET, dataBuffer);
            if (dataSize == 2) {
                return unpackPercentage(dataBuffer);
            }
            break;
        case ControlMode.kSpeed_val:
            dataSize = getTransaction(CANJNI.LM_API_SPD_SET, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kPosition_val:
            dataSize = getTransaction(CANJNI.LM_API_POS_SET, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kCurrent_val:
            dataSize = getTransaction(CANJNI.LM_API_ICTRL_SET, dataBuffer);
            if (dataSize == 2) {
                return unpackFXP8_8(dataBuffer);
            }
            break;
        case ControlMode.kVoltage_val:
            dataSize = getTransaction(CANJNI.LM_API_VCOMP_SET, dataBuffer);
            if (dataSize == 2) {
                return unpackFXP8_8(dataBuffer);
            }
            break;
        }
        return 0.0;

    }

    public void setNoAck(float value, byte syncGroup) throws CANTimeoutException {
    	int messageID;
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        if (!m_safetyHelper.isAlive()) {
            enableControl();
        }

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
            messageID = CANJNI.LM_API_VOLT_T_SET_NO_ACK;
            if (value > 1.0) value = 1.0f;
            if (value < -1.0) value = -1.0f;
            packPercentage(dataBuffer, value);
            dataSize = 2;
            break;
        case ControlMode.kSpeed_val: {
            messageID = CANJNI.LM_API_SPD_T_SET;
            dataSize = packFXP16_16(dataBuffer, value);
        }
        break;
        case ControlMode.kPosition_val: {
            messageID = CANJNI.LM_API_POS_T_SET;
            dataSize = packFXP16_16(dataBuffer, value);
        }
        break;
        case ControlMode.kCurrent_val: {
            messageID = CANJNI.LM_API_ICTRL_T_SET;
            dataSize = packFXP8_8(dataBuffer, value);
        }
        break;
        case ControlMode.kVoltage_val: {
            messageID = CANJNI.LM_API_VCOMP_T_SET_NO_ACK;
            dataSize = packFXP8_8(dataBuffer, value);
        }
        break;
        default:
            return;
        }

        if (syncGroup != 0) {
            dataBuffer[dataSize] = syncGroup;
            dataSize++;
        }

        synchronized(m_transactionMutex) {
        	sendMessage(messageID | m_deviceNumber, dataBuffer, dataSize);
        }

        m_safetyHelper.feed();
    }

    /**
     * Get the recently set outputValue setpoint.
     *
     * Needed by the SpeedControl interface (swallows CANTimeoutExceptions).
     *
     * @deprecated Use getX instead.
     * @return The most recently set outputValue setpoint.
     */
    public double get() {
        try {
            return getX();
        } catch (CANTimeoutException e) {
            return 0.0;
        }
    }

    /**
     * Common interface for disabling a motor.
     *
     * Needed by the SpeedControl interface (swallows CANTimeoutExceptions).
     *
     * @deprecated Use disableControl instead.
     */
    public void disable() {
        try {
            disableControl();
        } catch (CANTimeoutException e) {}
    }

    /**
     * Write out the PID value as seen in the PIDOutput base object.
     *
     * @deprecated Use setX instead.
     * @param output Write out the percentage voltage value as was computed by the PIDController
     */
    public void pidWrite(double output) {
        if (m_controlMode == ControlMode.kPercentVbus) {
            set(output);
        } else {
            // TODO: Error... only percent vbus mode supported for PID API
        }
    }

    byte packPercentage(byte[] buffer, double value) {
        short intValue = (short) (value * 32767.0);
        swap16(intValue, buffer);
        return 2;
    }

    byte packFXP8_8(byte[] buffer, double value) {
        short intValue = (short) (value * 256.0);
        swap16(intValue, buffer);
        return 2;
    }

    byte packFXP16_16(byte[] buffer, double value) {
        int intValue = (int) (value * 65536.0);
        swap32(intValue, buffer);
        return 4;
    }

    byte packINT16(byte[] buffer, short value) {
        swap16(value, buffer);
        return 2;
    }

    byte packINT32(byte[] buffer, int value) {
        swap32(value, buffer);
        return 4;
    }

    double unpackPercentage(byte[] buffer) {
        return unpack16(buffer,0) / 32767.0;
    }

    double unpackFXP8_8(byte[] buffer) {
        return unpack16(buffer,0) / 256.0;
    }

    double unpackFXP16_16(byte[] buffer) {
        return unpack32(buffer,0) / 65536.0;
    }

    int unpackINT16(byte[] buffer) {
        return unpack16(buffer,0);
    }

    long unpackINT32(byte[] buffer) {
        return unpack32(buffer,0);
    }
    private static final ByteBuffer sendTrustedDataBuffer = ByteBuffer.allocateDirect(kMaxMessageDataSize);

    /**
     * Send a message on the CAN bus through the CAN driver in FRC_NetworkCommunication
     *
     * Trusted messages require a 2-byte token at the beginning of the data payload.
     * If the message being sent is trusted, make space for the token.
     *
     * @param messageID The messageID to be used on the CAN bus
     * @param data The up to 8 bytes of data to be sent with the message
     * @param dataSize Specify how much of the data in "data" to send
     */
    protected static void sendMessage(int messageID, byte[] data, int dataSize) throws CANTimeoutException {
        final int[] kTrustedMessages = {
        		CANJNI.LM_API_VOLT_T_EN, CANJNI.LM_API_VOLT_T_SET,
        		CANJNI.LM_API_SPD_T_EN, CANJNI.LM_API_SPD_T_SET,
        		CANJNI.LM_API_VCOMP_T_EN, CANJNI.LM_API_VCOMP_T_SET,
        		CANJNI.LM_API_POS_T_EN, CANJNI.LM_API_POS_T_SET,
        		CANJNI.LM_API_ICTRL_T_EN, CANJNI.LM_API_ICTRL_T_SET
        };

        byte i;
        for (i = 0; i < kTrustedMessages.length; i++) {
            if ((kFullMessageIDMask & messageID) == kTrustedMessages[i])
            {
                // Make sure the data will still fit after adjusting for the token.
                if (dataSize > kMaxMessageDataSize - 2) {
                    throw new RuntimeException("CAN message has too much data.");
                }

                ByteBuffer trustedBuffer = ByteBuffer.allocateDirect(dataSize+2);
                trustedBuffer.put(0, (byte)0);
                trustedBuffer.put(1, (byte)0);

                byte j;
                for (j = 0; j < dataSize; j++) {
                    trustedBuffer.put(j+2, data[j]);
                }


        		ByteBuffer status = ByteBuffer.allocateDirect(4);
        		// set the byte order
        		status.order(ByteOrder.LITTLE_ENDIAN);
        		status.asIntBuffer().put(0,dataSize+2);


        		CANJNI.FRCNetworkCommunicationJaguarCANDriverSendMessage(messageID, trustedBuffer, status.asIntBuffer());
                return;
            }
        }

        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
		status.asIntBuffer().put(0,dataSize);

		ByteBuffer dataBuffer = null;
		if( dataSize > 0)
		{
			dataBuffer = ByteBuffer.allocateDirect(dataSize);
			dataBuffer.put(data);
		}

		CANJNI.FRCNetworkCommunicationJaguarCANDriverSendMessage(messageID, dataBuffer, status.asIntBuffer());

		CANExceptionFactory.checkStatus(status.asIntBuffer().get(0), messageID);
    }

    /**
     * Receive a message from the CAN bus through the CAN driver in FRC_NetworkCommunication
     *
     * @param messageID The messageID to read from the CAN bus
     * @param data The up to 8 bytes of data that was received with the message
     * @param timeout Specify how long to wait for a message (in seconds)
     */
    protected static byte receiveMessage(int messageID, byte[] data, double timeout) throws CANTimeoutException {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);

		ByteBuffer messageIDBuffer = ByteBuffer.allocateDirect(4);
		messageIDBuffer.order(ByteOrder.LITTLE_ENDIAN);
		messageIDBuffer.asIntBuffer().put(0,messageID);

		ByteBuffer dataBuffer = CANJNI.FRCNetworkCommunicationJaguarCANDriverReceiveMessage(
				messageIDBuffer.asIntBuffer(), (int) (timeout*1000), status.asIntBuffer());

		CANExceptionFactory.checkStatus(status.asIntBuffer().get(0), messageID);

        byte returnValue = 0;

        if( dataBuffer != null )
        {
        	returnValue = (byte)dataBuffer.capacity();
        	for(int index = 0; index < returnValue; index++)
        	{
        		data[index] = dataBuffer.get(index);
        	}
        }

        return returnValue;
    }

    protected static byte receiveMessage(int messageID, byte[] data) throws CANTimeoutException {
        //return receiveMessage(messageID, data, 0.01);
        return receiveMessage(messageID, data, 0.1);
    }

    /**
     * Execute a transaction with a Jaguar that sets some property.
     *
     * Jaguar always acks when it receives a message.  If we don't wait for an ack,
     * the message object in the Jaguar could get overwritten before it is handled.
     *
     * @param messageID The messageID to be used on the CAN bus (device number is added internally)
     * @param data The up to 8 bytes of data to be sent with the message
     * @param dataSize Specify how much of the data in "data" to send
     */
    protected byte setTransaction(int messageID, byte[] data, byte dataSize) throws CANTimeoutException {
        int ackMessageID = CANJNI.LM_API_ACK | m_deviceNumber;

        // Make sure we don't have more than one transaction with the same Jaguar outstanding.
        synchronized (m_transactionMutex) {
            // Throw away any stale acks.
            try {
                receiveMessage(ackMessageID, kNoData, 0.0);
            } catch(CANTimeoutException e) {}
            // Send the message with the data.
            sendMessage(messageID | m_deviceNumber, data, dataSize);
            // Wait for an ack.
            dataSize = receiveMessage(ackMessageID, kNoData);
        }
        return dataSize;
    }

    /**
     * Execute a transaction with a Jaguar that gets some property.
     *
     * Jaguar always generates a message with the same message ID when replying.
     *
     * @param messageID The messageID to read from the CAN bus (device number is added internally)
     * @param data The up to 8 bytes of data that was received with the message
     * @return Indicates how much data was received
     */
    protected byte getTransaction(int messageID, byte[] data) throws CANTimeoutException {
        int targetedMessageID = messageID | m_deviceNumber;
        byte dataSize = 0;
        // Make sure we don't have more than one transaction with the same Jaguar outstanding.
        synchronized (m_transactionMutex) {
            // Send the message requesting data.
            sendMessage(targetedMessageID, kNoData, 0);
            // Caller may have set bit31 for remote frame transmission so clear invalid bits[31-29]
            targetedMessageID &= 0x1FFFFFFF;
            // Wait for the data.
            dataSize = receiveMessage(targetedMessageID, data);
        }
        return dataSize;
    }

    /**
     * Set the reference source device for speed controller mode.
     *
     * Choose encoder as the source of speed feedback when in speed control mode.
     *
     * @param reference Specify a SpeedReference.
     */
    public void setSpeedReference(SpeedReference reference) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];

        dataBuffer[0] = reference.value;
        setTransaction(CANJNI.LM_API_SPD_REF, dataBuffer, (byte) 1);
    }

    /**
     * Get the reference source device for speed controller mode.
     *
     * @return A SpeedReference indicating the currently selected reference device for speed controller mode.
     */
    public SpeedReference getSpeedReference() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        dataSize = getTransaction(CANJNI.LM_API_SPD_REF, dataBuffer);
        if (dataSize == 1) {
            switch (dataBuffer[0]) {
            case SpeedReference.kEncoder_val:
                return SpeedReference.kEncoder;
            case SpeedReference.kInvEncoder_val:
                return SpeedReference.kInvEncoder;
            case SpeedReference.kQuadEncoder_val:
                return SpeedReference.kQuadEncoder;
            case SpeedReference.kNone_val:
                return SpeedReference.kNone;
            }
        }
        return SpeedReference.kNone;
    }

    /**
     * Set the reference source device for position controller mode.
     *
     * Choose between using and encoder and using a potentiometer
     * as the source of position feedback when in position control mode.
     *
     * @param reference Specify a PositionReference.
     */
    public void setPositionReference(PositionReference reference) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];

        dataBuffer[0] = reference.value;
        setTransaction(CANJNI.LM_API_POS_REF, dataBuffer, (byte) 1);
    }

    /**
     * Get the reference source device for position controller mode.
     *
     * @return A PositionReference indicating the currently selected reference device for position controller mode.
     */
    public PositionReference getPositionReference() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        dataSize = getTransaction(CANJNI.LM_API_POS_REF, dataBuffer);
        if (dataSize == 1) {
            switch (dataBuffer[0]) {
            case PositionReference.kPotentiometer_val:
                return PositionReference.kPotentiometer;
            case PositionReference.kQuadEncoder_val:
                return PositionReference.kQuadEncoder;
            case PositionReference.kNone_val:
                return PositionReference.kNone;
            }
        }
        return PositionReference.kNone;
    }

    /**
     * Set the P, I, and D constants for the closed loop modes.
     *
     * @param p The proportional gain of the Jaguar's PID controller.
     * @param i The integral gain of the Jaguar's PID controller.
     * @param d The differential gain of the Jaguar's PID controller.
     */
    public void setPID(double p, double i, double d) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
        case ControlMode.kVoltage_val:
            // TODO: Error, Not Valid
            break;
        case ControlMode.kSpeed_val:
            dataSize = packFXP16_16(dataBuffer, p);
            setTransaction(CANJNI.LM_API_SPD_PC, dataBuffer, dataSize);
            dataSize = packFXP16_16(dataBuffer, i);
            setTransaction(CANJNI.LM_API_SPD_IC, dataBuffer, dataSize);
            dataSize = packFXP16_16(dataBuffer, d);
            setTransaction(CANJNI.LM_API_SPD_DC, dataBuffer, dataSize);
            break;
        case ControlMode.kPosition_val:
            dataSize = packFXP16_16(dataBuffer, p);
            setTransaction(CANJNI.LM_API_POS_PC, dataBuffer, dataSize);
            dataSize = packFXP16_16(dataBuffer, i);
            setTransaction(CANJNI.LM_API_POS_IC, dataBuffer, dataSize);
            dataSize = packFXP16_16(dataBuffer, d);
            setTransaction(CANJNI.LM_API_POS_DC, dataBuffer, dataSize);
            break;
        case ControlMode.kCurrent_val:
            dataSize = packFXP16_16(dataBuffer, p);
            setTransaction(CANJNI.LM_API_ICTRL_PC, dataBuffer, dataSize);
            dataSize = packFXP16_16(dataBuffer, i);
            setTransaction(CANJNI.LM_API_ICTRL_IC, dataBuffer, dataSize);
            dataSize = packFXP16_16(dataBuffer, d);
            setTransaction(CANJNI.LM_API_ICTRL_DC, dataBuffer, dataSize);
            break;
        }
    }

    /**
     * Get the Proportional gain of the controller.
     *
     * @return The proportional gain.
     */
    public double getP() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
        case ControlMode.kVoltage_val:
            // TODO: Error, Not Valid
            break;
        case ControlMode.kSpeed_val:
            dataSize = getTransaction(CANJNI.LM_API_SPD_PC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kPosition_val:
            dataSize = getTransaction(CANJNI.LM_API_POS_PC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kCurrent_val:
            dataSize = getTransaction(CANJNI.LM_API_ICTRL_PC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        }
        return 0.0;
    }

    /**
     * Get the Intregral gain of the controller.
     *
     * @return The integral gain.
     */
    public double getI() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
        case ControlMode.kVoltage_val:
            // TODO: Error, Not Valid
            break;
        case ControlMode.kSpeed_val:
            dataSize = getTransaction(CANJNI.LM_API_SPD_IC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kPosition_val:
            dataSize = getTransaction(CANJNI.LM_API_POS_IC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kCurrent_val:
            dataSize = getTransaction(CANJNI.LM_API_ICTRL_IC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        }
        return 0.0;
    }

    /**
     * Get the Differential gain of the controller.
     *
     * @return The differential gain.
     */
    public double getD() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
        case ControlMode.kVoltage_val:
            // TODO: Error, Not Valid
            break;
        case ControlMode.kSpeed_val:
            dataSize = getTransaction(CANJNI.LM_API_SPD_DC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kPosition_val:
            dataSize = getTransaction(CANJNI.LM_API_POS_DC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        case ControlMode.kCurrent_val:
            dataSize = getTransaction(CANJNI.LM_API_ICTRL_DC, dataBuffer);
            if (dataSize == 4) {
                return unpackFXP16_16(dataBuffer);
            }
            break;
        }
        return 0.0;
    }

    /**
     * Enable the closed loop controller.
     *
     * Start actually controlling the output based on the feedback.
     */
    public void enableControl() throws CANTimeoutException {
        enableControl(0.0);
    }

    /**
     * Enable the closed loop controller.
     *
     * Start actually controlling the output based on the feedback.
     * If starting a position controller with an encoder reference,
     * use the encoderInitialPosition parameter to initialize the
     * encoder state.
     * @param encoderInitialPosition Encoder position to set if position with encoder reference.  Ignored otherwise.
     */
    public void enableControl(double encoderInitialPosition) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
            setTransaction(CANJNI.LM_API_VOLT_T_EN, dataBuffer, dataSize);
            break;
        case ControlMode.kSpeed_val:
            setTransaction(CANJNI.LM_API_SPD_T_EN, dataBuffer, dataSize);
            break;
        case ControlMode.kPosition_val:
            dataSize = packFXP16_16(dataBuffer, encoderInitialPosition);
            setTransaction(CANJNI.LM_API_POS_T_EN, dataBuffer, dataSize);
            break;
        case ControlMode.kCurrent_val:
            setTransaction(CANJNI.LM_API_ICTRL_T_EN, dataBuffer, dataSize);
            break;
        case ControlMode.kVoltage_val:
            setTransaction(CANJNI.LM_API_VCOMP_T_EN, dataBuffer, dataSize);
            break;
        }
    }

    /**
     * Disable the closed loop controller.
     *
     * Stop driving the output based on the feedback.
     */
    public void disableControl() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
            setTransaction(CANJNI.LM_API_VOLT_DIS, dataBuffer, dataSize);
            break;
        case ControlMode.kSpeed_val:
            setTransaction(CANJNI.LM_API_SPD_DIS, dataBuffer, dataSize);
            break;
        case ControlMode.kPosition_val:
            setTransaction(CANJNI.LM_API_POS_DIS, dataBuffer, dataSize);
            break;
        case ControlMode.kCurrent_val:
            setTransaction(CANJNI.LM_API_ICTRL_DIS, dataBuffer, dataSize);
            break;
        case ControlMode.kVoltage_val:
            setTransaction(CANJNI.LM_API_VCOMP_DIS, dataBuffer, dataSize);
            break;
        }
    }

    /**
     * Change the control mode of this Jaguar object.
     *
     * After changing modes, configure any PID constants or other settings needed
     * and then enableControl() to actually change the mode on the Jaguar.
     *
     * @param controlMode The new mode.
     */
    public void changeControlMode(ControlMode controlMode) throws CANTimeoutException {
        // Disable the previous mode
        disableControl();

        // Update the local mode
        m_controlMode = controlMode;

		// XXX: Resource Reporting Fixes
//        UsageReporting.report(UsageReporting.kResourceType_CANJaguar, m_deviceNumber, m_controlMode.value);
    }

    /**
     * Get the active control mode from the Jaguar.
     *
     * Ask the Jag what mode it is in.
     *
     * @return ControlMode that the Jag is in.
     */
    public ControlMode getControlMode() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_CMODE, dataBuffer);
        if (dataSize == 1) {
            switch (dataBuffer[0]) {
            case ControlMode.kPercentVbus_val:
                return ControlMode.kPercentVbus;
            case ControlMode.kCurrent_val:
                return ControlMode.kCurrent;
            case ControlMode.kSpeed_val:
                return ControlMode.kSpeed;
            case ControlMode.kPosition_val:
                return ControlMode.kPosition;
            case ControlMode.kVoltage_val:
                return ControlMode.kVoltage;
            }
        }
        return ControlMode.kPercentVbus;
    }

    /**
     * Get the voltage at the battery input terminals of the Jaguar.
     *
     * @return The bus voltage in Volts.
     */
    public double getBusVoltage() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_VOLTBUS, dataBuffer);
        if (dataSize == 2) {
            return unpackFXP8_8(dataBuffer);
        }
        return 0.0;
    }

    /**
     * Get the voltage being output from the motor terminals of the Jaguar.
     *
     * @return The output voltage in Volts.
     */
    public double getOutputVoltage() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize = 0;

        // Read the volt out which is in Volts units.
        dataSize = getTransaction(CANJNI.LM_API_STATUS_VOUT, dataBuffer);
        if (dataSize == 2) {
            return unpackFXP8_8(dataBuffer);
        }
        return 0.0;
    }

    /**
     * Get the current through the motor terminals of the Jaguar.
     *
     * @return The output current in Amps.
     */
    public double getOutputCurrent() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_CURRENT, dataBuffer);
        if (dataSize == 2) {
            return unpackFXP8_8(dataBuffer);
        }
        return 0.0;

    }

    /**
     * Get the internal temperature of the Jaguar.
     *
     * @return The temperature of the Jaguar in degrees Celsius.
     */
    public double getTemperature() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_TEMP, dataBuffer);
        if (dataSize == 2) {
            return unpackFXP8_8(dataBuffer);
        }
        return 0.0;
    }

    /**
     * Get the position of the encoder or potentiometer.
     *
     * @return The position of the motor based on the configured feedback.
     */
    public double getPosition() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_POS, dataBuffer);
        if (dataSize == 4) {
            return unpackFXP16_16(dataBuffer);
        }
        return 0.0;
    }

    /**
     * Get the speed of the encoder.
     *
     * @return The speed of the motor in RPM based on the configured feedback.
     */
    public double getSpeed() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_SPD, dataBuffer);
        if (dataSize == 4) {
            return unpackFXP16_16(dataBuffer);
        }
        return 0.0;
    }

    /**
     * Get the status of the forward limit switch.
     *
     * @return The motor is allowed to turn in the forward direction when true.
     */
    public boolean getForwardLimitOK() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_LIMIT, dataBuffer);
        if (dataSize == 1) {
            return (dataBuffer[0] & Limits.kForwardLimit_val) != 0;
        }
        return false;
    }

    /**
     * Get the status of the reverse limit switch.
     *
     * @return The motor is allowed to turn in the reverse direction when true.
     */
    public boolean getReverseLimitOK() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_LIMIT, dataBuffer);
        if (dataSize == 1) {
            return (dataBuffer[0] & Limits.kReverseLimit_val) != 0;
        }
        return false;
    }

    /**
     * Get the status of any faults the Jaguar has detected.
     *
     * @return A bit-mask of faults defined by the "Faults" enum class.
     */
    public short getFaults() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_FAULT, dataBuffer);
        if (dataSize == 2) {
            return (short)unpackINT16(dataBuffer);
        }
        return 0;
    }

    /**
     * Check if the Jaguar's power has been cycled since this was last called.
     *
     * This should return true the first time called after a Jaguar power up,
     * and false after that.
     *
     * @return The Jaguar was power cycled since the last call to this function.
     */
    public boolean getPowerCycled() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_STATUS_POWER, dataBuffer);
        if (dataSize == 1) {
            boolean powerCycled = dataBuffer[0] != 0;

            // Clear the power cycled bit now that we've accessed it
            if (powerCycled) {
                dataBuffer[0] = 1;
                setTransaction(CANJNI.LM_API_STATUS_POWER, dataBuffer, (byte) 1);
            }

            return powerCycled;
        }
        return false;
    }

    /**
     * Set the maximum voltage change rate.
     *
     * When in percent voltage output mode, the rate at which the voltage changes can
     * be limited to reduce current spikes.  Set this to 0.0 to disable rate limiting.
     *
     * @param rampRate The maximum rate of voltage change in Percent Voltage mode in V/s.
     */
    public void setVoltageRampRate(double rampRate) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        switch (m_controlMode.value) {
        case ControlMode.kPercentVbus_val:
            dataSize = packPercentage(dataBuffer, rampRate / (m_maxOutputVoltage * kControllerRate));
            setTransaction(CANJNI.LM_API_VOLT_SET_RAMP, dataBuffer, dataSize);
            break;
        case ControlMode.kVoltage_val:
            dataSize = packFXP8_8(dataBuffer, rampRate / kControllerRate);
            setTransaction(CANJNI.LM_API_VCOMP_IN_RAMP, dataBuffer, dataSize);
            break;
        default:
            return;
        }
    }

    /**
     * Get the version of the firmware running on the Jaguar.
     *
     * @return The firmware version.  0 if the device did not respond.
     */
    public int getFirmwareVersion() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        // Set the MSB to tell the 2CAN that this is a remote message.
        dataSize = getTransaction(0x80000000 | CANJNI.CAN_MSGID_API_FIRMVER, dataBuffer);
        if (dataSize == 4) {
            return (int)unpackINT32(dataBuffer);
        }
        return 0;
    }

    /**
     * Get the version of the Jaguar hardware.
     *
     * @return The hardware version. 1: Jaguar,  2: Black Jaguar
     */
    public byte getHardwareVersion() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = getTransaction(CANJNI.LM_API_HWVER, dataBuffer);
        if (dataSize == 1 + 1) {
            if (dataBuffer[0] == m_deviceNumber) {
                return dataBuffer[1];
            }
        }
        // Assume Gray Jag if there is no response
        return CANJNI.LM_HWVER_JAG_1_0;
    }

    /**
     * Configure what the controller does to the H-Bridge when neutral (not driving the output).
     *
     * This allows you to override the jumper configuration for brake or coast.
     *
     * @param mode Select to use the jumper setting or to override it to coast or brake.
     */
    public void configNeutralMode(NeutralMode mode) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];

        dataBuffer[0] = mode.value;
        setTransaction(CANJNI.LM_API_CFG_BRAKE_COAST, dataBuffer, (byte) 1);
    }

    /**
     * Configure how many codes per revolution are generated by your encoder.
     *
     * @param codesPerRev The number of counts per revolution in 1X mode.
     */
    public void configEncoderCodesPerRev(int codesPerRev) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = packINT16(dataBuffer, (short)codesPerRev);
        setTransaction(CANJNI.LM_API_CFG_ENC_LINES, dataBuffer, dataSize);
    }

    /**
     * Configure the number of turns on the potentiometer.
     *
     * There is no special support for continuous turn potentiometers.
     * Only integer numbers of turns are supported.
     *
     * @param turns The number of turns of the potentiometer
     */
    public void configPotentiometerTurns(int turns) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = packINT16(dataBuffer, (short)turns);
        setTransaction(CANJNI.LM_API_CFG_POT_TURNS, dataBuffer, dataSize);
    }

    /**
     * Configure Soft Position Limits when in Position Controller mode.
     *
     * When controlling position, you can add additional limits on top of the limit switch inputs
     * that are based on the position feedback.  If the position limit is reached or the
     * switch is opened, that direction will be disabled.
     *
     * @param forwardLimitPosition The position that if exceeded will disable the forward direction.
     * @param reverseLimitPosition The position that if exceeded will disable the reverse direction.
     */
    public void configSoftPositionLimits(double forwardLimitPosition, double reverseLimitPosition) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        dataSize = packFXP16_16(dataBuffer, forwardLimitPosition);
        dataBuffer[dataSize++] = (forwardLimitPosition > reverseLimitPosition) ? (byte) 1 : (byte) 0;
        setTransaction(CANJNI.LM_API_CFG_LIMIT_FWD, dataBuffer, dataSize);

        dataSize = packFXP16_16(dataBuffer, reverseLimitPosition);
        dataBuffer[dataSize++] = forwardLimitPosition <= reverseLimitPosition ? (byte) 1 : (byte) 0;
        setTransaction(CANJNI.LM_API_CFG_LIMIT_REV, dataBuffer, dataSize);

        dataBuffer[0] = LimitMode.kSoftPositionLimit_val;
        setTransaction(CANJNI.LM_API_CFG_LIMIT_MODE, dataBuffer, (byte) 1);
    }

    /**
     * Disable Soft Position Limits if previously enabled.
     *
     * Soft Position Limits are disabled by default.
     */
    public void disableSoftPositionLimits() throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];

        dataBuffer[0] = LimitMode.kSwitchInputsOnly_val;
        setTransaction(CANJNI.LM_API_CFG_LIMIT_MODE, dataBuffer, (byte) 1);
    }

    /**
     * Configure the maximum voltage that the Jaguar will ever output.
     *
     * This can be used to limit the maximum output voltage in all modes so that
     * motors which cannot withstand full bus voltage can be used safely.
     *
     * @param voltage The maximum voltage output by the Jaguar.
     */
    public void configMaxOutputVoltage(double voltage) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        m_maxOutputVoltage = voltage;
        dataSize = packFXP8_8(dataBuffer, voltage);
        setTransaction(CANJNI.LM_API_CFG_MAX_VOUT, dataBuffer, dataSize);
    }

    /**
     * Configure how long the Jaguar waits in the case of a fault before resuming operation.
     *
     * Faults include over temerature, over current, and bus under voltage.
     * The default is 3.0 seconds, but can be reduced to as low as 0.5 seconds.
     *
     * @param faultTime The time to wait before resuming operation, in seconds.
     */
    public void configFaultTime(double faultTime) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];
        byte dataSize;

        // Message takes ms
        dataSize = packINT16(dataBuffer, (short) (faultTime * 1000.0));
        setTransaction(CANJNI.LM_API_CFG_FAULT_TIME, dataBuffer, dataSize);
    }

    /**
     * Update all the motors that have pending sets in the syncGroup.
     *
     * @param syncGroup A bitmask of groups to generate synchronous output.
     */
    public static void updateSyncGroup(byte syncGroup) throws CANTimeoutException {
        byte[] dataBuffer = new byte[8];

        dataBuffer[0] = syncGroup;
        sendMessage(CANJNI.CAN_MSGID_API_SYNC, dataBuffer, 1);
    }


    public void setExpiration(double timeout) {
        m_safetyHelper.setExpiration(timeout);
    }

    public double getExpiration() {
        return m_safetyHelper.getExpiration();
    }

    public boolean isAlive() {
        return m_safetyHelper.isAlive();
    }

    public boolean isSafetyEnabled() {
        return m_safetyHelper.isSafetyEnabled();
    }

    public void setSafetyEnabled(boolean enabled) {
        m_safetyHelper.setSafetyEnabled(enabled);
    }

    public String getDescription() {
        return "CANJaguar ID "+m_deviceNumber;
    }

    /**
     * Common interface for stopping a motor.
     *
     * @deprecated Use disableControl instead.
     */
    public void stopMotor() {
        try {
            disableControl();
        } catch (CANTimeoutException e) {}
    }

    /*
     * Live Window code, only does anything if live window is activated.
     */
    public String getSmartDashboardType() {
        return "Speed Controller";
    }
    private ITable m_table;
    private ITableListener m_table_listener;

    /**
     * {@inheritDoc}
     */
    public void initTable(ITable subtable) {
        m_table = subtable;
        updateTable();
    }

    /**
     * {@inheritDoc}
     */
    public void updateTable() {
        if (m_table != null) {
            m_table.putNumber("Value", get());
        }
    }

    /**
     * {@inheritDoc}
     */
    public ITable getTable() {
        return m_table;
    }

    /**
     * {@inheritDoc}
     */
    public void startLiveWindowMode() {
        set(0); // Stop for safety
        m_table_listener = new ITableListener() {
            public void valueChanged(ITable itable, String key, Object value, boolean bln) {
                set(((Double) value).doubleValue());
            }
        };
        m_table.addTableListener("Value", m_table_listener, true);
    }

    /**
     * {@inheritDoc}
     */
    public void stopLiveWindowMode() {
        set(0); // Stop for safety
        // TODO: Broken, should only remove the listener from "Value" only.
        m_table.removeTableListener(m_table_listener);
    }
}
