// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package com.qualcomm.hardware.gobilda;

import static com.qualcomm.robotcore.util.TypeConversion.byteArrayToInt;
import static com.qualcomm.robotcore.util.TypeConversion.intToByteArray;

import com.qualcomm.hardware.lynx.LynxI2cDeviceSynch;
import com.qualcomm.robotcore.hardware.I2cAddr;
import com.qualcomm.robotcore.hardware.I2cDeviceSynchDevice;
import com.qualcomm.robotcore.hardware.I2cDeviceSynchSimple;
import com.qualcomm.robotcore.hardware.configuration.annotations.DeviceProperties;
import com.qualcomm.robotcore.hardware.configuration.annotations.I2cDeviceType;

import org.firstinspires.ftc.robotcore.external.navigation.AngleUnit;
import org.firstinspires.ftc.robotcore.external.navigation.DistanceUnit;
import org.firstinspires.ftc.robotcore.external.navigation.Pose2D;
import org.firstinspires.ftc.robotcore.external.navigation.Quaternion;
import org.firstinspires.ftc.robotcore.external.navigation.UnnormalizedAngleUnit;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.stream.Stream;


@I2cDeviceType
@DeviceProperties(
        name = "goBILDA® Pinpoint Odometry Computer",
        xmlTag = "goBILDAPinpoint",
        description = "goBILDA® Pinpoint Odometry Computer (IMU Sensor Fusion for 2 Wheel Odometry)"
)

public class GoBildaPinpointDriver extends I2cDeviceSynchDevice<I2cDeviceSynchSimple> {
    private int deviceID = 0;
    private int deviceVersion = 0;
    private int deviceStatus = 0;
    private int loopTime = 0;
    private int xEncoderValue = 0;
    private int yEncoderValue = 0;
    private float xPosition = 0;
    private float yPosition = 0;
    private float hOrientation = 0;
    private float xVelocity = 0;
    private float yVelocity = 0;
    private float hVelocity = 0;
    private float mmPerTick = 0;
    private float xPodOffset = 0;
    private float yPodOffset = 0;
    private float yawScalar = 0;
    private float quaternionW = 0;
    private float quaternionX = 0;
    private float quaternionY = 0;
    private float quaternionZ = 0;
    private float pitch = 0;
    private float roll = 0;

    private Register[] bulkReadScope = {
            Register.DEVICE_STATUS,
            Register.LOOP_TIME,
            Register.X_ENCODER_VALUE,
            Register.Y_ENCODER_VALUE,
            Register.X_POSITION,
            Register.Y_POSITION,
            Register.H_ORIENTATION,
            Register.X_VELOCITY,
            Register.Y_VELOCITY,
            Register.H_VELOCITY
    };

    public enum ErrorDetectionType {
        NONE,
        CRC,
        LOCAL_TEST,
    }

    private ErrorDetectionType errorDetectionType = ErrorDetectionType.LOCAL_TEST;


    private static final float goBILDA_SWINGARM_POD = 13.26291192f; //ticks-per-mm for the goBILDA Swingarm Pod
    private static final float goBILDA_4_BAR_POD = 19.89436789f; //ticks-per-mm for the goBILDA 4-Bar Pod

    private final int CRC_SIZE = 1;

    private final byte CRC_INITIAL_VALUE = (byte) 0x90;
    private final byte CRC_POLYNOMIAL_VALUE = (byte) 0x31;
    private final byte CRC_FINAL_XOR_VALUE = (byte) 0x00;

    //i2c address of the device
    private static final byte DEFAULT_ADDRESS = 0x31;

    public GoBildaPinpointDriver(I2cDeviceSynchSimple deviceClient, boolean deviceClientIsOwned) {
        super(deviceClient, deviceClientIsOwned);

        this.deviceClient.setI2cAddress(I2cAddr.create7bit(DEFAULT_ADDRESS));
        super.registerArmingStateCallback(false);
    }


    @Override
    public Manufacturer getManufacturer() {
        return Manufacturer.GoBilda;
    }

    @Override
    protected synchronized boolean doInitialize() {
        ((LynxI2cDeviceSynch) (deviceClient)).setBusSpeed(LynxI2cDeviceSynch.BusSpeed.FAST_400K);
        return true;
    }

    @Override
    public String getDeviceName() {
        return "goBILDA® Pinpoint Odometry Computer";
    }

    /**
     * Captures the length of each type of register used on the device. Aside from BULK_READ all registers are 4 bytes long
     */
    private enum RegisterType {
        INT32(4),
        FLOAT(4),
        GENERIC(4),
        BULK(40);

        private final int length;

        RegisterType(int length) {
            this.length = length;
        }
    }

    //Register map of the i2c device
    public enum Register {
        DEVICE_ID(1, RegisterType.INT32),
        DEVICE_VERSION(2, RegisterType.INT32),
        DEVICE_STATUS(3, RegisterType.INT32),
        DEVICE_CONTROL(4, RegisterType.INT32),
        LOOP_TIME(5, RegisterType.INT32),
        X_ENCODER_VALUE(6, RegisterType.INT32),
        Y_ENCODER_VALUE(7, RegisterType.INT32),
        X_POSITION(8, RegisterType.FLOAT),
        Y_POSITION(9, RegisterType.FLOAT),
        H_ORIENTATION(10, RegisterType.FLOAT),
        X_VELOCITY(11, RegisterType.FLOAT),
        Y_VELOCITY(12, RegisterType.FLOAT),
        H_VELOCITY(13, RegisterType.FLOAT),
        MM_PER_TICK(14, RegisterType.FLOAT),
        X_POD_OFFSET(15, RegisterType.FLOAT),
        Y_POD_OFFSET(16, RegisterType.FLOAT),
        YAW_SCALAR(17, RegisterType.FLOAT),
        BULK_READ(18, RegisterType.BULK),
        QUATERNION_W(19, RegisterType.FLOAT),
        QUATERNION_X(20, RegisterType.FLOAT),
        QUATERNION_Y(21, RegisterType.FLOAT),
        QUATERNION_Z(22, RegisterType.FLOAT),
        PITCH(23, RegisterType.FLOAT),
        ROLL(24, RegisterType.FLOAT),
        SET_BULK_READ(25, RegisterType.INT32);

        private final int bVal;
        private final RegisterType registerType;

        Register(int bVal, RegisterType registerType) {
            this.bVal = bVal;
            this.registerType = registerType;
        }
    }

    //Device Status enum that captures the current fault condition of the device
    public enum DeviceStatus {
        NOT_READY(0),
        READY(1),
        CALIBRATING(1 << 1),
        FAULT_X_POD_NOT_DETECTED(1 << 2),
        FAULT_Y_POD_NOT_DETECTED(1 << 3),
        FAULT_NO_PODS_DETECTED(1 << 2 | 1 << 3),
        FAULT_IMU_RUNAWAY(1 << 4),
        FAULT_BAD_READ(1 << 5);

        private final int status;

        DeviceStatus(int status) {
            this.status = status;
        }
    }

    //enum that captures the direction the encoders are set to
    public enum EncoderDirection {
        FORWARD,
        REVERSED
    }

    //enum that captures the kind of goBILDA odometry pods, if goBILDA pods are used
    public enum GoBildaOdometryPods {
        goBILDA_SWINGARM_POD,
        goBILDA_4_BAR_POD
    }

    //enum that captures a limited scope of read data. More options may be added in future update
    public enum ReadData {
        ONLY_UPDATE_HEADING,
    }

    //Device Control enum that has the values to set
    private enum DeviceControl {
        RECALIBRATE_IMU(1 << 0),
        RESET_POS_AND_IMU(1 << 1),
        SET_X_ENCODER_REVERSED(1 << 4),
        SET_X_ENCODER_FORWARD(1 << 5),
        SET_Y_ENCODER_REVERSED(1 << 2),
        SET_Y_ENCODER_FORWARD(1 << 3);

        public final int value;

        DeviceControl(int value) {
            this.value = value;
        }
    }


    /**
     * Writes an int to the i2c device
     *
     * @param reg the register to write the int to
     * @param i   the integer to write to the register
     */
    private void writeInt(final Register reg, int i) {
        deviceClient.write(reg.bVal, intToByteArray(i, ByteOrder.LITTLE_ENDIAN));
    }

    /**
     * Reads an int from a register of the i2c device
     *
     * @param reg the register to read from
     * @return returns an int that contains the value stored in the read register
     */
    private int readInt(Register reg) {
        return byteArrayToInt(deviceClient.read(reg.bVal, 4), ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Converts a byte array to a float value
     *
     * @param byteArray byte array to transform
     * @param byteOrder order of byte array to convert
     * @return the float value stored by the byte array
     */
    private float byteArrayToFloat(byte[] byteArray, ByteOrder byteOrder) {
        return ByteBuffer.wrap(byteArray).order(byteOrder).getFloat();
    }

    /**
     * Reads a float from a register
     *
     * @param reg the register to read
     * @return the float value stored in that register
     */
    private float readFloat(Register reg) {
        return byteArrayToFloat(deviceClient.read(reg.bVal, 4), ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Converts a float to a byte array
     *
     * @param value the float array to convert
     * @return the byte array converted from the float
     */
    private byte[] floatToByteArray(float value, ByteOrder byteOrder) {
        return ByteBuffer.allocate(4).order(byteOrder).putFloat(value).array();
    }

    /**
     * Writes a byte array to a register on the i2c device
     *
     * @param reg   the register to write to
     * @param bytes the byte array to write
     */
    private void writeByteArray(Register reg, byte[] bytes) {
        deviceClient.write(reg.bVal, bytes);
    }

    /**
     * Writes a float to a register on the i2c device
     *
     * @param reg the register to write to
     * @param f   the float to write
     */
    private void writeFloat(Register reg, float f) {
        byte[] bytes = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putFloat(f).array();
        deviceClient.write(reg.bVal, bytes);
    }

    /**
     * Looks up the DeviceStatus enum corresponding with an int value
     *
     * @param s int to lookup
     * @return the Odometry Computer state
     */
    private DeviceStatus lookupStatus(int s) {
        if ((s & DeviceStatus.CALIBRATING.status) != 0) {
            return DeviceStatus.CALIBRATING;
        }
        boolean xPodDetected = (s & DeviceStatus.FAULT_X_POD_NOT_DETECTED.status) == 0;
        boolean yPodDetected = (s & DeviceStatus.FAULT_Y_POD_NOT_DETECTED.status) == 0;

        if (!xPodDetected && !yPodDetected) {
            return DeviceStatus.FAULT_NO_PODS_DETECTED;
        }
        if (!xPodDetected) {
            return DeviceStatus.FAULT_X_POD_NOT_DETECTED;
        }
        if (!yPodDetected) {
            return DeviceStatus.FAULT_Y_POD_NOT_DETECTED;
        }
        if ((s & DeviceStatus.FAULT_IMU_RUNAWAY.status) != 0) {
            return DeviceStatus.FAULT_IMU_RUNAWAY;
        }
        if ((s & DeviceStatus.READY.status) != 0) {
            return DeviceStatus.READY;
        }
        if ((s & DeviceStatus.FAULT_BAD_READ.status) != 0) {
            return DeviceStatus.FAULT_BAD_READ;
        } else {
            return DeviceStatus.NOT_READY;
        }
    }

    /**
     * Checks to see if the register passed in is in the bulkReadScope
     *
     * @param register register to check
     * @return true if not in bulk read.
     */
    private boolean registerNotInBulkRead(Register register) {
        return !Arrays.asList(bulkReadScope).contains(register);
    }

    /**
     * Saves either an int or a float to the private variable associated to a register.
     *
     * @param register to save data from
     * @param dataI    the integer to write, if applicable
     * @param dataF    the float to write, if applicable
     */
    private void saveData(Register register, int dataI, float dataF) {
        final int positionThreshold = 5000; //more than one FTC field in mm
        final int headingThreshold = 120; //About 20 full rotations in Radians
        final int velocityThreshold = 10000; //10k mm/sec is faster than an FTC robot should be going...
        final int headingVelocityThreshold = 120; //About 20 rotations per second

        switch (register) {
            case DEVICE_ID:
                deviceID = dataI;
                break;
            case DEVICE_VERSION:
                deviceVersion = dataI;
                break;
            case DEVICE_STATUS:
                deviceStatus = dataI;
                break;
            case LOOP_TIME:
                loopTime = dataI;
                break;
            case X_ENCODER_VALUE:
                xEncoderValue = dataI;
                break;
            case Y_ENCODER_VALUE:
                yEncoderValue = dataI;
                break;
            case X_POSITION:
                if (errorDetectionType == ErrorDetectionType.LOCAL_TEST) {
                    dataF = isPositionCorrupt(xPosition, dataF, positionThreshold, false);
                }
                xPosition = dataF;
                break;
            case Y_POSITION:
                if (errorDetectionType == ErrorDetectionType.LOCAL_TEST) {
                    dataF = isPositionCorrupt(yPosition, dataF, positionThreshold, false);
                }
                yPosition = dataF;
                break;
            case H_ORIENTATION:
                if (errorDetectionType == ErrorDetectionType.LOCAL_TEST) {
                    dataF = isPositionCorrupt(hOrientation, dataF, headingThreshold, false);
                }
                hOrientation = dataF;
                break;
            case X_VELOCITY:
                if (errorDetectionType == ErrorDetectionType.LOCAL_TEST) {
                    dataF = isVelocityCorrupt(xVelocity, dataF, velocityThreshold, false);
                }
                xVelocity = dataF;
                break;
            case Y_VELOCITY:
                if (errorDetectionType == ErrorDetectionType.LOCAL_TEST) {
                    dataF = isVelocityCorrupt(yVelocity, dataF, velocityThreshold, false);
                }
                yVelocity = dataF;
                break;
            case H_VELOCITY:
                if (errorDetectionType == ErrorDetectionType.LOCAL_TEST) {
                    dataF = isVelocityCorrupt(hVelocity, dataF, headingVelocityThreshold, false);
                }
                hVelocity = dataF;
                break;
            case MM_PER_TICK:
                mmPerTick = dataF;
                break;
            case X_POD_OFFSET:
                xPodOffset = dataF;
                break;
            case Y_POD_OFFSET:
                yPodOffset = dataF;
                break;
            case YAW_SCALAR:
                yawScalar = dataF;
                break;
            case QUATERNION_W:
                quaternionW = dataF;
                break;
            case QUATERNION_X:
                quaternionX = dataF;
                break;
            case QUATERNION_Y:
                quaternionY = dataF;
                break;
            case QUATERNION_Z:
                quaternionZ = dataF;
                break;
            case PITCH:
                pitch = dataF;
                break;
            case ROLL:
                roll = dataF;
                break;
        }
    }

    /**
     * checks a given byteArray[] for a valid CRC data signature by comparing a calculated CRC to the one received in the read.
     *
     * @param byteArray    data to validate.
     * @param registerType The kind of register validated. Can be FLOAT, INT32, or BULK.
     * @return true if CRC validates the data. False otherwise.
     */

    private boolean checkCRC(byte[] byteArray, RegisterType registerType) {
        if (registerType == RegisterType.BULK) {
            int readLength = bulkReadScope.length * RegisterType.GENERIC.length;
            if (computeCRC8(Arrays.copyOfRange(byteArray, 0, readLength)) == byteArray[(readLength + CRC_SIZE) - 1]) {
                return true;
            } else {
                deviceStatus = DeviceStatus.FAULT_BAD_READ.status;
                return false;
            }
        }

        if (byteArray.length > RegisterType.GENERIC.length) {
            if (computeCRC8(Arrays.copyOfRange(byteArray, 0, RegisterType.GENERIC.length)) == byteArray[(RegisterType.GENERIC.length + CRC_SIZE) - 1]) {
                return true;
            } else {
                deviceStatus = DeviceStatus.FAULT_BAD_READ.status;
                return false;
            }
        }
        return false;
    }

    /**
     * Computes the correct CRC8 for a byteArray.
     *
     * @param byteArray data to check
     * @return byte to compare against received CRC.
     */
    private byte computeCRC8(byte[] byteArray) {
        byte crc = CRC_INITIAL_VALUE;

        for (byte b : byteArray) {
            crc ^= b;
            for (int i = 0; i < 8; i++) {
                if ((crc & 0x80) != 0) {
                    crc = (byte) ((crc << 1) ^ CRC_POLYNOMIAL_VALUE);
                } else {
                    crc <<= 1;
                }
            }
        }
        return (byte) (crc ^ CRC_FINAL_XOR_VALUE);
    }

    /**
     * Confirm that the number received is a number, and does not include a change above the threshold
     *
     * @param oldValue   the reading from the previous cycle
     * @param newValue   the new reading
     * @param threshold  the maximum change between this reading and the previous one
     * @param bulkUpdate true if we are updating the loopTime variable. If not it should be false.
     * @return newValue if the position is good, oldValue otherwise
     */
    private Float isPositionCorrupt(float oldValue, float newValue, int threshold, boolean bulkUpdate) {
        boolean noData = bulkUpdate && (loopTime < 1);

        boolean isCorrupt = noData || Float.isNaN(newValue) || Math.abs(newValue - oldValue) > threshold;

        if (!isCorrupt) {
            return newValue;
        }

        deviceStatus = DeviceStatus.FAULT_BAD_READ.status;
        return oldValue;
    }

    /**
     * Confirm that the number received is a number, and does not include a change above the threshold
     *
     * @param oldValue  the reading from the previous cycle
     * @param newValue  the new reading
     * @param threshold the velocity allowed to be reported
     * @return newValue if the velocity is good, oldValue otherwise
     */
    private Float isVelocityCorrupt(float oldValue, float newValue, int threshold, boolean bulkUpdate) {
        boolean noData = bulkUpdate && (loopTime <= 1);
        boolean isCorrupt = noData || Float.isNaN(newValue) || Math.abs(newValue) > threshold;

        if (!isCorrupt) {
            return newValue;
        }

        deviceStatus = DeviceStatus.FAULT_BAD_READ.status;
        return oldValue;
    }

    /**
     * Reads the BULK_READ register depending on how the bulkReadScope is configured and saves data to local variables.
     * if CRC is enabled, and a bad CRC is detected on the BulkRead, then no data will be saved.
     */
    private void flexBulkRead() {
        byte[] bArr;

        if (errorDetectionType == ErrorDetectionType.CRC) {
            bArr = deviceClient.read(Register.BULK_READ.bVal, (bulkReadScope.length * RegisterType.GENERIC.length) + CRC_SIZE);
            if (!checkCRC(bArr, RegisterType.BULK)) {
                return;
            }
        } else {
            bArr = deviceClient.read(Register.BULK_READ.bVal, (bulkReadScope.length * RegisterType.GENERIC.length));
        }

        for (int i = 0; i < bulkReadScope.length; i++) {
            int index = i * RegisterType.GENERIC.length;
            switch (bulkReadScope[i].registerType) {
                case INT32:
                    int dataI = byteArrayToInt(Arrays.copyOfRange(bArr, index, index + bulkReadScope[i].registerType.length), ByteOrder.LITTLE_ENDIAN);
                    saveData(bulkReadScope[i], dataI, 0);
                    break;
                case FLOAT:
                    float dataF = byteArrayToFloat(Arrays.copyOfRange(bArr, index, index + bulkReadScope[i].registerType.length), ByteOrder.LITTLE_ENDIAN);
                    saveData(bulkReadScope[i], 0, dataF);
                    break;
            }
        }
    }

    /**
     * For devices with version 1 or version 2 firmware, this reads a fixed length BULK_READ register. <br>
     * A warning is thrown if CRC is requested as CRC was not enabled on V1 or V2 devices.
     */
    private void fixedBulkRead() {
        final int positionThreshold = 5000; //more than one FTC field in mm
        final int headingThreshold = 120; //About 20 full rotations in Radians
        final int velocityThreshold = 10000; //10k mm/sec is faster than an FTC robot should be going...
        final int headingVelocityThreshold = 120; //About 20 rotations per second

        float oldPosX = xPosition;
        float oldPosY = yPosition;
        float oldPosH = hOrientation;
        float oldVelX = xVelocity;
        float oldVelY = yVelocity;
        float oldVelH = hVelocity;

        byte[] bArr = deviceClient.read(Register.BULK_READ.bVal, 40);
        deviceStatus = byteArrayToInt(Arrays.copyOfRange(bArr, 0, 4), ByteOrder.LITTLE_ENDIAN);
        loopTime = byteArrayToInt(Arrays.copyOfRange(bArr, 4, 8), ByteOrder.LITTLE_ENDIAN);
        xEncoderValue = byteArrayToInt(Arrays.copyOfRange(bArr, 8, 12), ByteOrder.LITTLE_ENDIAN);
        yEncoderValue = byteArrayToInt(Arrays.copyOfRange(bArr, 12, 16), ByteOrder.LITTLE_ENDIAN);
        xPosition = byteArrayToFloat(Arrays.copyOfRange(bArr, 16, 20), ByteOrder.LITTLE_ENDIAN);
        yPosition = byteArrayToFloat(Arrays.copyOfRange(bArr, 20, 24), ByteOrder.LITTLE_ENDIAN);
        hOrientation = byteArrayToFloat(Arrays.copyOfRange(bArr, 24, 28), ByteOrder.LITTLE_ENDIAN);
        xVelocity = byteArrayToFloat(Arrays.copyOfRange(bArr, 28, 32), ByteOrder.LITTLE_ENDIAN);
        yVelocity = byteArrayToFloat(Arrays.copyOfRange(bArr, 32, 36), ByteOrder.LITTLE_ENDIAN);
        hVelocity = byteArrayToFloat(Arrays.copyOfRange(bArr, 36, 40), ByteOrder.LITTLE_ENDIAN);

        switch (errorDetectionType) {
            case CRC:
                throw new RuntimeException("CRC Error Handling Not Supported by this Firmware");
            case LOCAL_TEST:
                /*
                 * Check to see if any of the floats we have received from the device are NaN or are too large
                 * if they are, we return the previously read value and alert the user via the DeviceStatus Enum.
                 */
                xPosition = isPositionCorrupt(oldPosX, xPosition, positionThreshold, true);
                yPosition = isPositionCorrupt(oldPosY, yPosition, positionThreshold, true);
                hOrientation = isPositionCorrupt(oldPosH, hOrientation, headingThreshold, true);
                xVelocity = isVelocityCorrupt(oldVelX, xVelocity, velocityThreshold, true);
                yVelocity = isVelocityCorrupt(oldVelY, yVelocity, velocityThreshold, true);
                hVelocity = isVelocityCorrupt(oldVelH, hVelocity, headingVelocityThreshold, true);
                break;
        }
    }

    /**
     * Reads a register and saves the data found there to the local variable. Uses either CRC or Local error detection.
     *
     * @param register register to read
     */
    private void readRegister(Register register) {
        boolean checkCRC = (errorDetectionType == ErrorDetectionType.CRC);

        byte[] temp = deviceClient.read(register.bVal, RegisterType.GENERIC.length + CRC_SIZE);

        switch (register.registerType) {
            case INT32:
                if (checkCRC(temp, RegisterType.INT32) || !checkCRC) {
                    saveData(register, byteArrayToInt(Arrays.copyOfRange(temp, 0, RegisterType.INT32.length), ByteOrder.LITTLE_ENDIAN), 0);
                } else {
                    deviceStatus = DeviceStatus.FAULT_BAD_READ.status;
                }
                break;
            case FLOAT:
                if (checkCRC(temp, RegisterType.FLOAT) || !checkCRC) {
                    saveData(register, 0, byteArrayToFloat(Arrays.copyOfRange(temp, 0, RegisterType.FLOAT.length), ByteOrder.LITTLE_ENDIAN));
                } else {
                    deviceStatus = DeviceStatus.FAULT_BAD_READ.status;
                }
                break;
            case BULK:
                update();
                break;
        }
    }

    /**
     * Call this once per loop to read new data from the Odometry Computer. Data will only update once this is called.
     */
    public void update() {
        if (deviceVersion == 0) {
            getDeviceVersion();  // Makes sure device version has been read
        }

        if (deviceVersion == 1 || deviceVersion == 2) {
            fixedBulkRead();
        }
        if (deviceVersion >= 3) {
            flexBulkRead();
        }
    }

    /**
     * Call this once per loop to read new data from the Odometry Computer. This is an override of the update() function
     * which allows a narrower range of data to be read from the device for faster read times. Currently ONLY_UPDATE_HEADING
     * is supported.
     *
     * @param data GoBildaPinpointDriver.ReadData.ONLY_UPDATE_HEADING
     */
    public void update(ReadData data) {
        if (data == ReadData.ONLY_UPDATE_HEADING) {
            final int headingThreshold = 120;

            float oldPosH = hOrientation;

            hOrientation = byteArrayToFloat(deviceClient.read(Register.H_ORIENTATION.bVal, 4), ByteOrder.LITTLE_ENDIAN);

            hOrientation = isPositionCorrupt(oldPosH, hOrientation, headingThreshold, false);

            if (deviceStatus == DeviceStatus.FAULT_BAD_READ.status) {
                deviceStatus = DeviceStatus.READY.status;
            }
        }
    }

    /**
     * Only supported on V3 firmware and above. This configures the registers that are read in bulk
     * when .update() is called. Use this to minimize read times based on your unique application.
     *
     * @param registers An array of registers, add registers that you need data from frequently.
     */
    public void setBulkReadScope(Register... registers) {
        if (deviceVersion == 0) {
            deviceVersion = readInt(Register.DEVICE_VERSION);
        }
        if (deviceVersion == 1 || deviceVersion == 2) {
            throw new RuntimeException(".setBulkReadScope is not supported by this device firmware.");
        }
        if (deviceVersion >= 3) {
            bulkReadScope = registers.clone();

            Stream<Register> reg = Arrays.stream(registers).distinct();
            ArrayList<Byte> arrayList = new ArrayList<>(registers.length);

            Iterator<Register> iter = reg.iterator();
            while (iter.hasNext()) {
                arrayList.add((byte) iter.next().bVal);
            }

            byte[] arr = new byte[arrayList.size()];
            for (int i = 0; i < arrayList.size(); i++) {
                arr[i] = arrayList.get(i);
            }
            writeByteArray(Register.SET_BULK_READ, arr); //write all registers sequentially
        }
    }

    /**
     * The kind of error correction used on the I²C communication from the device. <br><br>
     * NONE: This does not check the data, and passes it directly to the user. <br>
     * CRC: This uses CRC8 error detection to catch incorrect reads. - Only supported by devices with V3 firmware or newer.<br>
     * LOCAL_TEST: "Controller only" validation that ensures that the data is !NAN, is not all zeros, and is a reasonable number.
     * This is faster than CRC but may not catch every erroneous read.<br>
     */
    public void setErrorDetectionType(ErrorDetectionType e) {
        errorDetectionType = e;
    }

    /**
     * Sets the odometry pod positions relative to the point that the odometry computer tracks around.<br><br>
     * The most common tracking position is the center of the robot. <br> <br>
     * The X pod offset refers to how far sideways from the tracking point the X (forward) odometry pod is. Left of the center is a positive number, right of center is a negative number. <br>
     * the Y pod offset refers to how far forwards from the tracking point the Y (strafe) odometry pod is. forward of center is a positive number, backwards is a negative number.<br>
     *
     * @param xOffset      how sideways from the center of the robot is the X (forward) pod? Left increases
     * @param yOffset      how far forward from the center of the robot is the Y (Strafe) pod? forward increases
     * @param distanceUnit the unit of distance used for offsets.
     */
    public void setOffsets(double xOffset, double yOffset, DistanceUnit distanceUnit) {
        writeFloat(Register.X_POD_OFFSET, (float) distanceUnit.toMm(xOffset));
        writeFloat(Register.Y_POD_OFFSET, (float) distanceUnit.toMm(yOffset));
    }

    /**
     * Recalibrates the Odometry Computer's internal IMU. <br><br>
     * <strong> Robot MUST be stationary </strong> <br><br>
     * Device takes a large number of samples, and uses those as the gyroscope zero-offset. This takes approximately 0.25 seconds.
     */
    public void recalibrateIMU() {
        writeInt(Register.DEVICE_CONTROL, DeviceControl.RECALIBRATE_IMU.value);
    }

    /**
     * Resets the current position to 0,0,0 and recalibrates the Odometry Computer's internal IMU. <br><br>
     * <strong> Robot MUST be stationary </strong> <br><br>
     * Device takes a large number of samples, and uses those as the gyroscope zero-offset. This takes approximately 0.25 seconds.
     */
    public void resetPosAndIMU() {
        writeInt(Register.DEVICE_CONTROL, DeviceControl.RESET_POS_AND_IMU.value);
    }

    /**
     * Can reverse the direction of each encoder.
     *
     * @param xEncoder FORWARD or REVERSED, X (forward) pod should increase when the robot is moving forward
     * @param yEncoder FORWARD or REVERSED, Y (strafe) pod should increase when the robot is moving left
     */
    public void setEncoderDirections(EncoderDirection xEncoder, EncoderDirection yEncoder) {
        if (xEncoder == EncoderDirection.FORWARD) {
            writeInt(Register.DEVICE_CONTROL, DeviceControl.SET_X_ENCODER_FORWARD.value);
        }
        if (xEncoder == EncoderDirection.REVERSED) {
            writeInt(Register.DEVICE_CONTROL, DeviceControl.SET_X_ENCODER_REVERSED.value);
        }

        if (yEncoder == EncoderDirection.FORWARD) {
            writeInt(Register.DEVICE_CONTROL, DeviceControl.SET_Y_ENCODER_FORWARD.value);
        }
        if (yEncoder == EncoderDirection.REVERSED) {
            writeInt(Register.DEVICE_CONTROL, DeviceControl.SET_Y_ENCODER_REVERSED.value);
        }
    }

    /**
     * This allows you to set the encoder resolution by the type of GoBilda pod you are using.
     * If you aren't using a GoBilda pod, use setEncoderResolution(double) instead. <br><br>
     *
     * @param pods goBILDA_SWINGARM_POD or goBILDA_4_BAR_POD
     */
    public void setEncoderResolution(GoBildaOdometryPods pods) {
        if (pods == GoBildaOdometryPods.goBILDA_SWINGARM_POD) {
            setEncoderResolution(goBILDA_SWINGARM_POD, DistanceUnit.MM);
        }
        if (pods == GoBildaOdometryPods.goBILDA_4_BAR_POD) {
            setEncoderResolution(goBILDA_4_BAR_POD, DistanceUnit.MM);
        }
    }

    /**
     * Sets the encoder resolution in ticks per mm of the odometry pods. <br>
     * You can find this number by dividing the counts-per-revolution of your encoder by the circumference of the wheel.
     *
     * @param ticksPerUnit should be somewhere between 10 ticks/mm and 100 ticks/mm a goBILDA Swingarm pod is ~13.26291192
     * @param distanceUnit unit used for distance
     */
    public void setEncoderResolution(double ticksPerUnit, DistanceUnit distanceUnit) {
        double resolution = 1.0 / distanceUnit.toMm(1.0 / ticksPerUnit);
        writeByteArray(Register.MM_PER_TICK, (floatToByteArray((float) resolution, ByteOrder.LITTLE_ENDIAN)));
    }

    /**
     * Tuning this value should be unnecessary.<br>
     * The goBILDA Odometry Computer has a per-device tuned yaw offset already applied when you receive it.<br><br>
     * This is a scalar that is applied to the gyro's yaw value. Increasing it will mean it will report more than one degree for every degree the sensor fusion algorithm measures. <br><br>
     * You can tune this variable by rotating the robot a large amount (10 full turns is a good starting place) and comparing the amount that the robot rotated to the amount measured.
     * Rotating the robot exactly 10 times should measure 3600°. If it measures more or less, divide moved amount by the measured amount and apply that value to the Yaw Offset.<br><br>
     * If you find that to get an accurate heading number you need to apply a scalar of more than 1.05, or less than 0.95, your device may be bad. Please reach out to tech@gobilda.com
     *
     * @param yawScalar A scalar for the robot's heading.
     */
    public void setYawScalar(double yawScalar) {
        writeByteArray(Register.YAW_SCALAR, (floatToByteArray((float) yawScalar, ByteOrder.LITTLE_ENDIAN)));
    }

    /**
     * Send a position that the Pinpoint should use to track your robot relative to. You can use this to
     * update the estimated position of your robot with new external sensor data, or to run a robot
     * in field coordinates. <br><br>
     * This overrides the current position. <br><br>
     * <strong>Using this feature to track your robot's position in field coordinates:</strong> <br>
     * When you start your code, send a Pose2D that describes the starting position on the field of your robot. <br>
     * Say you're on the red alliance, your robot is against the wall and closer to the audience side,
     * and the front of your robot is pointing towards the center of the field.
     * You can send a setPosition with something like -600mm x, -1200mm Y, and 90 degrees. The pinpoint would then always
     * keep track of how far away from the center of the field you are. <br><br>
     * <strong>Using this feature to update your position with additional sensors: </strong><br>
     * Some robots have a secondary way to locate their robot on the field. This is commonly
     * Apriltag localization in FTC, but it can also be something like a distance sensor.
     * Often these external sensors are absolute (meaning they measure something about the field)
     * so their data is very accurate. But they can be slower to read, or you may need to be in a very specific
     * position on the field to use them. In that case, spend most of your time relying on the Pinpoint
     * to determine your location. Then when you pull a new position from your secondary sensor,
     * send a setPosition command with the new position. The Pinpoint will then track your movement
     * relative to that new, more accurate position.
     *
     * @param pos a Pose2D describing the robot's new position.
     */
    public void setPosition(Pose2D pos) {
        setPosX(pos.getX(DistanceUnit.MM), DistanceUnit.MM);
        setPosY(pos.getY(DistanceUnit.MM), DistanceUnit.MM);
        setHeading(pos.getHeading(AngleUnit.RADIANS), AngleUnit.RADIANS);
    }

    /**
     * Send a X position that the Pinpoint should use to track your robot relative to.
     * You can use this to update the estimated position of your robot with new external
     * sensor data, or to run a robot in field coordinates.
     *
     * @param posX         the new X position you'd like the Pinpoint to track your robot relive to.
     * @param distanceUnit the unit for posX
     */
    public void setPosX(double posX, DistanceUnit distanceUnit) {
        writeByteArray(Register.X_POSITION, (floatToByteArray((float) distanceUnit.toMm(posX), ByteOrder.LITTLE_ENDIAN)));
    }

    /**
     * Send a Y position that the Pinpoint should use to track your robot relative to.
     * You can use this to update the estimated position of your robot with new external
     * sensor data, or to run a robot in field coordinates.
     *
     * @param posY         the new Y position you'd like the Pinpoint to track your robot relive to.
     * @param distanceUnit the unit for posY
     */
    public void setPosY(double posY, DistanceUnit distanceUnit) {
        writeByteArray(Register.Y_POSITION, (floatToByteArray((float) distanceUnit.toMm(posY), ByteOrder.LITTLE_ENDIAN)));
    }

    /**
     * Send a heading that the Pinpoint should use to track your robot relative to.
     * You can use this to update the estimated position of your robot with new external
     * sensor data, or to run a robot in field coordinates.
     *
     * @param heading   the new heading you'd like the Pinpoint to track your robot relive to.
     * @param angleUnit Radians or Degrees
     */
    public void setHeading(double heading, AngleUnit angleUnit) {
        writeByteArray(Register.H_ORIENTATION, (floatToByteArray((float) angleUnit.toRadians(heading), ByteOrder.LITTLE_ENDIAN)));
    }

    /**
     * Checks the deviceID of the Odometry Computer. Should return 1.
     *
     * @return 1 if device is functional.
     */
    public int getDeviceID() {
        if (registerNotInBulkRead(Register.DEVICE_ID)) {
            readRegister(Register.DEVICE_ID);
        }
        return deviceID;
    }

    /**
     * @return the firmware version of the Odometry Computer
     */
    public int getDeviceVersion() {
        if (registerNotInBulkRead(Register.DEVICE_VERSION)) {
            readRegister(Register.DEVICE_VERSION);
        }
        return deviceVersion;
    }

    /**
     * @return a scalar that the IMU measured heading is multiplied by.
     */
    public float getYawScalar() {
        if (registerNotInBulkRead(Register.YAW_SCALAR)) {
            readRegister(Register.YAW_SCALAR);
        }
        return yawScalar;
    }

    /**
     * Device Status stores any faults the Odometry Computer may be experiencing. These faults include:
     *
     * @return one of the following states:<br>
     * NOT_READY - The device is currently powering up. And has not initialized yet. RED LED<br>
     * READY - The device is currently functioning as normal. GREEN LED<br>
     * CALIBRATING - The device is currently recalibrating the gyro. RED LED<br>
     * FAULT_NO_PODS_DETECTED - the device does not detect any pods plugged in. PURPLE LED <br>
     * FAULT_X_POD_NOT_DETECTED - The device does not detect an X pod plugged in. BLUE LED <br>
     * FAULT_Y_POD_NOT_DETECTED - The device does not detect a Y pod plugged in. ORANGE LED <br>
     * FAULT_BAD_READ - The Java code has detected a bad I²C read, the result reported is a
     * duplicate of the last good read.
     */
    public DeviceStatus getDeviceStatus() {
        return lookupStatus(deviceStatus);
    }

    /**
     * Checks the Odometry Computer's most recent loop time.<br><br>
     * If values less than 500, or more than 1100 are commonly seen here, there may be something wrong with your device. Please reach out to tech@gobilda.com
     *
     * @return loop time in microseconds (1/1,000,000 seconds)
     */
    public int getLoopTime() {
        return loopTime;
    }

    /**
     * Checks the Odometry Computer's most recent loop frequency.<br><br>
     * If values less than 900, or more than 2000 are commonly seen here, there may be something wrong with your device. Please reach out to tech@gobilda.com
     *
     * @return Pinpoint Frequency in Hz (loops per second),
     */
    public double getFrequency() {
        if (loopTime != 0) {
            return 1000000.0 / loopTime;
        } else {
            return 0;
        }
    }

    /**
     * @return the raw value of the X (forward) encoder in ticks
     */
    public int getEncoderX() {
        if (registerNotInBulkRead(Register.X_ENCODER_VALUE)) {
            readRegister(Register.X_ENCODER_VALUE);
        }
        return xEncoderValue;
    }

    /**
     * @return the raw value of the Y (strafe) encoder in ticks
     */
    public int getEncoderY() {
        if (registerNotInBulkRead(Register.Y_ENCODER_VALUE)) {
            readRegister(Register.Y_ENCODER_VALUE);
        }
        return yEncoderValue;
    }

    /**
     * @param distanceUnit the unit that the estimated position will return in
     * @return the estimated X (forward) position of the robot in specified unit
     */
    public double getPosX(DistanceUnit distanceUnit) {
        if (registerNotInBulkRead(Register.X_POSITION)) {
            readRegister(Register.X_POSITION);
        }
        return distanceUnit.fromMm(xPosition);
    }

    /**
     * @param distanceUnit the unit that the estimated position will return in
     * @return the estimated Y (Strafe) position of the robot in specified unit
     */
    public double getPosY(DistanceUnit distanceUnit) {
        if (registerNotInBulkRead(Register.Y_POSITION)) {
            readRegister(Register.Y_POSITION);
        }
        return distanceUnit.fromMm(yPosition);
    }

    /**
     * @return the normalized estimated H (heading) position of the robot in specified unit
     * normalized heading is wrapped from -180°, to 180°.
     */
    public double getHeading(AngleUnit angleUnit) {
        if (registerNotInBulkRead(Register.H_ORIENTATION)) {
            readRegister(Register.H_ORIENTATION);
        }
        return angleUnit.fromRadians(hOrientation);
    }

    /**
     * @return the unnormalized estimated H (heading) position of the robot in specified unit
     * unnormalized heading is not constrained from -180° to 180°. It will continue counting
     * multiple rotations.
     */
    public double getHeading(UnnormalizedAngleUnit unnormalizedAngleUnit) {
        if (registerNotInBulkRead(Register.H_ORIENTATION)) {
            readRegister(Register.H_ORIENTATION);
        }
        return unnormalizedAngleUnit.fromRadians(hOrientation);
    }

    /**
     * @return the estimated X (forward) velocity of the robot in specified unit/sec
     */
    public double getVelX(DistanceUnit distanceUnit) {
        if (registerNotInBulkRead(Register.X_VELOCITY)) {
            readRegister(Register.X_VELOCITY);
        }
        return distanceUnit.fromMm(xVelocity);
    }

    /**
     * @return the estimated Y (strafe) velocity of the robot in specified unit/sec
     */
    public double getVelY(DistanceUnit distanceUnit) {
        if (registerNotInBulkRead(Register.Y_VELOCITY)) {
            readRegister(Register.Y_VELOCITY);
        }
        return distanceUnit.fromMm(yVelocity);
    }

    /**
     * @return the estimated H (heading) velocity of the robot in specified unit/sec
     */
    public double getHeadingVelocity(UnnormalizedAngleUnit unnormalizedAngleUnit) {
        if (registerNotInBulkRead(Register.H_VELOCITY)) {
            readRegister(Register.H_VELOCITY);
        }
        return unnormalizedAngleUnit.fromRadians(hVelocity);
    }

    /**
     * <strong> This uses its own I2C read, avoid calling this every loop. </strong>
     *
     * @return the user-set offset for the X (forward) pod in specified unit
     */
    public float getXOffset(DistanceUnit distanceUnit) {
        if (registerNotInBulkRead(Register.X_POD_OFFSET)) {
            readRegister(Register.X_POD_OFFSET);
        }
        return (float) distanceUnit.fromMm(xPodOffset);
    }

    /**
     * <strong> This uses its own I2C read, avoid calling this every loop. </strong>
     *
     * @return the user-set offset for the Y (strafe) pod
     */
    public float getYOffset(DistanceUnit distanceUnit) {
        if (registerNotInBulkRead(Register.Y_POD_OFFSET)) {
            readRegister(Register.Y_POD_OFFSET);
        }
        return (float) distanceUnit.fromMm(yPodOffset);
    }

    /**
     * @return a Pose2D containing the estimated position of the robot
     */
    public Pose2D getPosition() {
        if (registerNotInBulkRead(Register.X_POSITION)) {
            readRegister(Register.X_POSITION);
        }
        if (registerNotInBulkRead(Register.Y_POSITION)) {
            readRegister(Register.Y_POSITION);
        }
        if (registerNotInBulkRead(Register.H_ORIENTATION)) {
            readRegister(Register.H_ORIENTATION);
        }
        return new Pose2D(DistanceUnit.MM,
                xPosition,
                yPosition,
                AngleUnit.RADIANS,
                AngleUnit.normalizeRadians(hOrientation));
    }

    /**
     * @return a Quaternion which describes the 3d orientation of the device.
     */
    public Quaternion getQuaternion() {
        if (deviceVersion == 0) {
            readInt(Register.DEVICE_VERSION);
        }
        if (deviceVersion == 1 || deviceVersion == 2) {
            throw new RuntimeException("Quaternion output is not supported on this device firmware");
        } else {
            if (registerNotInBulkRead(Register.QUATERNION_W)) {
                readRegister(Register.QUATERNION_W);
            }
            if (registerNotInBulkRead(Register.QUATERNION_X)) {
                readRegister(Register.QUATERNION_X);
            }
            if (registerNotInBulkRead(Register.QUATERNION_Y)) {
                readRegister(Register.QUATERNION_Y);
            }
            if (registerNotInBulkRead(Register.QUATERNION_Z)) {
                readRegister(Register.QUATERNION_Z);
            }
            return new Quaternion(quaternionW, quaternionX, quaternionY, quaternionZ, 0);
        }
    }

    /**
     * @return the current pitch of the device in the specified AngleUnit.
     */

    public double getPitch(AngleUnit angleUnit) {
        if (deviceVersion == 0) {
            readInt(Register.DEVICE_VERSION);
        }
        if (deviceVersion == 1 || deviceVersion == 2) {
            throw new RuntimeException("IMU Pitch output is not supported on this device firmware");
        } else {
            if (registerNotInBulkRead(Register.PITCH)) {
                pitch = readFloat(Register.PITCH);
            }
            return angleUnit.fromRadians(pitch);
        }
    }

    /**
     * @return the current roll of the device in the specified AngleUnit.
     */
    public double getRoll(AngleUnit angleUnit) {
        if (deviceVersion == 0) {
            readInt(Register.DEVICE_VERSION);
        }
        if (deviceVersion == 1 || deviceVersion == 2) {
            throw new RuntimeException("IMU Roll output is not supported on this device firmware");
        } else {
            if (registerNotInBulkRead(Register.ROLL)) {
                roll = readFloat(Register.ROLL);
            }
            return angleUnit.fromRadians(roll);
        }
    }
}
