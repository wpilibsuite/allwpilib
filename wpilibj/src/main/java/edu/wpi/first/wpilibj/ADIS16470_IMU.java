// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;

// CHECKSTYLE.OFF: TypeName
// CHECKSTYLE.OFF: MemberName
// CHECKSTYLE.OFF: LocalVariableName
// CHECKSTYLE.OFF: ParameterName
// CHECKSTYLE.OFF: EmptyCatchBlock

/** This class is for the ADIS16470 IMU that connects to the RoboRIO SPI port. */
@SuppressWarnings("PMD.RedundantFieldInitializer")
public class ADIS16470_IMU implements AutoCloseable, Sendable {
  /* ADIS16470 Register Map Declaration */
  private static final int FLASH_CNT = 0x00; // Flash memory write count
  private static final int DIAG_STAT = 0x02; // Diagnostic and operational status
  private static final int X_GYRO_LOW = 0x04; // X-axis gyroscope output, lower word
  private static final int X_GYRO_OUT = 0x06; // X-axis gyroscope output, upper word
  private static final int Y_GYRO_LOW = 0x08; // Y-axis gyroscope output, lower word
  private static final int Y_GYRO_OUT = 0x0A; // Y-axis gyroscope output, upper word
  private static final int Z_GYRO_LOW = 0x0C; // Z-axis gyroscope output, lower word
  private static final int Z_GYRO_OUT = 0x0E; // Z-axis gyroscope output, upper word
  private static final int X_ACCL_LOW = 0x10; // X-axis accelerometer output, lower word
  private static final int X_ACCL_OUT = 0x12; // X-axis accelerometer output, upper word
  private static final int Y_ACCL_LOW = 0x14; // Y-axis accelerometer output, lower word
  private static final int Y_ACCL_OUT = 0x16; // Y-axis accelerometer output, upper word
  private static final int Z_ACCL_LOW = 0x18; // Z-axis accelerometer output, lower word
  private static final int Z_ACCL_OUT = 0x1A; // Z-axis accelerometer output, upper word
  private static final int TEMP_OUT = 0x1C; // Temperature output (internal, not calibrated)
  private static final int TIME_STAMP = 0x1E; // PPS mode time stamp
  private static final int X_DELTANG_LOW = 0x24; // X-axis delta angle output, lower word
  private static final int X_DELTANG_OUT = 0x26; // X-axis delta angle output, upper word
  private static final int Y_DELTANG_LOW = 0x28; // Y-axis delta angle output, lower word
  private static final int Y_DELTANG_OUT = 0x2A; // Y-axis delta angle output, upper word
  private static final int Z_DELTANG_LOW = 0x2C; // Z-axis delta angle output, lower word
  private static final int Z_DELTANG_OUT = 0x2E; // Z-axis delta angle output, upper word
  private static final int X_DELTVEL_LOW = 0x30; // X-axis delta velocity output, lower word
  private static final int X_DELTVEL_OUT = 0x32; // X-axis delta velocity output, upper word
  private static final int Y_DELTVEL_LOW = 0x34; // Y-axis delta velocity output, lower word
  private static final int Y_DELTVEL_OUT = 0x36; // Y-axis delta velocity output, upper word
  private static final int Z_DELTVEL_LOW = 0x38; // Z-axis delta velocity output, lower word
  private static final int Z_DELTVEL_OUT = 0x3A; // Z-axis delta velocity output, upper word
  private static final int XG_BIAS_LOW =
      0x40; // X-axis gyroscope bias offset correction, lower word
  private static final int XG_BIAS_HIGH =
      0x42; // X-axis gyroscope bias offset correction, upper word
  private static final int YG_BIAS_LOW =
      0x44; // Y-axis gyroscope bias offset correction, lower word
  private static final int YG_BIAS_HIGH =
      0x46; // Y-axis gyroscope bias offset correction, upper word
  private static final int ZG_BIAS_LOW =
      0x48; // Z-axis gyroscope bias offset correction, lower word
  private static final int ZG_BIAS_HIGH =
      0x4A; // Z-axis gyroscope bias offset correction, upper word
  private static final int XA_BIAS_LOW =
      0x4C; // X-axis accelerometer bias offset correction, lower word
  private static final int XA_BIAS_HIGH =
      0x4E; // X-axis accelerometer bias offset correction, upper word
  private static final int YA_BIAS_LOW =
      0x50; // Y-axis accelerometer bias offset correction, lower word
  private static final int YA_BIAS_HIGH =
      0x52; // Y-axis accelerometer bias offset correction, upper word
  private static final int ZA_BIAS_LOW =
      0x54; // Z-axis accelerometer bias offset correction, lower word
  private static final int ZA_BIAS_HIGH =
      0x56; // Z-axis accelerometer bias offset correction, upper word
  private static final int FILT_CTRL = 0x5C; // Filter control
  private static final int MSC_CTRL = 0x60; // Miscellaneous control
  private static final int UP_SCALE = 0x62; // Clock scale factor, PPS mode
  private static final int DEC_RATE = 0x64; // Decimation rate control (output data rate)
  private static final int NULL_CNFG = 0x66; // Auto-null configuration control
  private static final int GLOB_CMD = 0x68; // Global commands
  private static final int FIRM_REV = 0x6C; // Firmware revision
  private static final int FIRM_DM = 0x6E; // Firmware revision date, month and day
  private static final int FIRM_Y = 0x70; // Firmware revision date, year
  private static final int PROD_ID = 0x72; // Product identification
  private static final int SERIAL_NUM = 0x74; // Serial number (relative to assembly lot)
  private static final int USER_SCR1 = 0x76; // User scratch register 1
  private static final int USER_SCR2 = 0x78; // User scratch register 2
  private static final int USER_SCR3 = 0x7A; // User scratch register 3
  private static final int FLSHCNT_LOW = 0x7C; // Flash update count, lower word
  private static final int FLSHCNT_HIGH = 0x7E; // Flash update count, upper word

  // Weight between the previous and current gyro angles represents 1 second for the timestamp,
  // this is the point at which we ignore the previous angle because it is too old to be of use.
  // The IMU timestamp conversion is 1 = 49.02us, the value 1_000_000 is the number of microseconds
  // we average over.
  private static final double AVERAGE_RATE_SCALING_FACTOR = 49.02 / 1_000_000;

  private static final byte[] m_autospi_allAngle_packet = {
    X_DELTANG_OUT,
    FLASH_CNT,
    X_DELTANG_LOW,
    FLASH_CNT,
    Y_DELTANG_OUT,
    FLASH_CNT,
    Y_DELTANG_LOW,
    FLASH_CNT,
    Z_DELTANG_OUT,
    FLASH_CNT,
    Z_DELTANG_LOW,
    FLASH_CNT,
    X_GYRO_OUT,
    FLASH_CNT,
    Y_GYRO_OUT,
    FLASH_CNT,
    Z_GYRO_OUT,
    FLASH_CNT,
    X_ACCL_OUT,
    FLASH_CNT,
    Y_ACCL_OUT,
    FLASH_CNT,
    Z_ACCL_OUT,
    FLASH_CNT
  };

  /** ADIS16470 calibration times. */
  public enum CalibrationTime {
    /** 32 ms calibration time. */
    _32ms(0),
    /** 64 ms calibration time. */
    _64ms(1),
    /** 128 ms calibration time. */
    _128ms(2),
    /** 256 ms calibration time. */
    _256ms(3),
    /** 512 ms calibration time. */
    _512ms(4),
    /** 1 s calibration time. */
    _1s(5),
    /** 2 s calibration time. */
    _2s(6),
    /** 4 s calibration time. */
    _4s(7),
    /** 8 s calibration time. */
    _8s(8),
    /** 16 s calibration time. */
    _16s(9),
    /** 32 s calibration time. */
    _32s(10),
    /** 64 s calibration time. */
    _64s(11);

    private final int value;

    CalibrationTime(int value) {
      this.value = value;
    }
  }

  /**
   * IMU axes.
   *
   * <p>kX, kY, and kZ refer to the IMU's X, Y, and Z axes respectively. kYaw, kPitch, and kRoll are
   * configured by the user to refer to an X, Y, or Z axis.
   */
  public enum IMUAxis {
    /** The IMU's X axis. */
    kX,
    /** The IMU's Y axis. */
    kY,
    /** The IMU's Z axis. */
    kZ,
    /** The user-configured yaw axis. */
    kYaw,
    /** The user-configured pitch axis. */
    kPitch,
    /** The user-configured roll axis. */
    kRoll,
  }

  // Static Constants
  private static final double kDeltaAngleSf = 2160.0 / 2147483648.0; // 2160 / (2^31)
  private static final double kGrav = 9.81;

  // User-specified axes
  private IMUAxis m_yaw_axis;
  private IMUAxis m_pitch_axis;
  private IMUAxis m_roll_axis;

  // Instant raw outputs
  private double m_gyro_rate_x = 0.0;
  private double m_gyro_rate_y = 0.0;
  private double m_gyro_rate_z = 0.0;
  private double m_accel_x = 0.0;
  private double m_accel_y = 0.0;
  private double m_accel_z = 0.0;

  // Integrated gyro angle
  private double m_integ_angle_x = 0.0;
  private double m_integ_angle_y = 0.0;
  private double m_integ_angle_z = 0.0;

  // Complementary filter variables
  private double m_dt = 0.0;
  private double m_alpha = 0.0;
  private static final double kTau = 1.0;
  private double m_compAngleX = 0.0;
  private double m_compAngleY = 0.0;
  private double m_accelAngleX = 0.0;
  private double m_accelAngleY = 0.0;

  // State variables
  private volatile boolean m_thread_active = false;
  private int m_calibration_time = 0;
  private volatile boolean m_first_run = true;
  private volatile boolean m_thread_idle = false;
  private boolean m_auto_configured = false;
  private double m_scaled_sample_rate = 2500.0;

  // Resources
  private SPI m_spi;
  private SPI.Port m_spi_port;
  private DigitalInput m_auto_interrupt;
  private DigitalInput m_reset_in;
  private DigitalOutput m_status_led;
  private Thread m_acquire_task;
  private boolean m_connected;

  private SimDevice m_simDevice;
  private SimBoolean m_simConnected;
  private SimDouble m_simGyroAngleX;
  private SimDouble m_simGyroAngleY;
  private SimDouble m_simGyroAngleZ;
  private SimDouble m_simGyroRateX;
  private SimDouble m_simGyroRateY;
  private SimDouble m_simGyroRateZ;
  private SimDouble m_simAccelX;
  private SimDouble m_simAccelY;
  private SimDouble m_simAccelZ;

  /**
   * Creates a new ADIS16740 IMU object.
   *
   * <p>The default setup is the onboard SPI port with a calibration time of 1 second. Yaw, pitch,
   * and roll are kZ, kX, and kY respectively.
   */
  public ADIS16470_IMU() {
    this(IMUAxis.kZ, IMUAxis.kX, IMUAxis.kY, SPI.Port.kOnboardCS0, CalibrationTime._1s);
  }

  /**
   * Creates a new ADIS16740 IMU object.
   *
   * <p>The default setup is the onboard SPI port with a calibration time of 1 second.
   *
   * <p><b><i>Input axes limited to kX, kY and kZ. Specifying kYaw, kPitch,or kRoll will result in
   * an error.</i></b>
   *
   * @param yaw_axis The axis that measures the yaw
   * @param pitch_axis The axis that measures the pitch
   * @param roll_axis The axis that measures the roll
   */
  public ADIS16470_IMU(IMUAxis yaw_axis, IMUAxis pitch_axis, IMUAxis roll_axis) {
    this(yaw_axis, pitch_axis, roll_axis, SPI.Port.kOnboardCS0, CalibrationTime._1s);
  }

  /**
   * Creates a new ADIS16740 IMU object.
   *
   * <p><b><i>Input axes limited to kX, kY and kZ. Specifying kYaw, kPitch,or kRoll will result in
   * an error.</i></b>
   *
   * @param yaw_axis The axis that measures the yaw
   * @param pitch_axis The axis that measures the pitch
   * @param roll_axis The axis that measures the roll
   * @param port The SPI Port the gyro is plugged into
   * @param cal_time Calibration time
   */
  @SuppressWarnings("this-escape")
  public ADIS16470_IMU(
      IMUAxis yaw_axis,
      IMUAxis pitch_axis,
      IMUAxis roll_axis,
      SPI.Port port,
      CalibrationTime cal_time) {
    if (yaw_axis == IMUAxis.kYaw
        || yaw_axis == IMUAxis.kPitch
        || yaw_axis == IMUAxis.kRoll
        || pitch_axis == IMUAxis.kYaw
        || pitch_axis == IMUAxis.kPitch
        || pitch_axis == IMUAxis.kRoll
        || roll_axis == IMUAxis.kYaw
        || roll_axis == IMUAxis.kPitch
        || roll_axis == IMUAxis.kRoll) {
      DriverStation.reportError(
          "ADIS16470 constructor only allows IMUAxis.kX, IMUAxis.kY or IMUAxis.kZ as arguments.",
          false);
      DriverStation.reportError(
          "Constructing ADIS with default axes. (IMUAxis.kZ is defined as Yaw)", false);
      yaw_axis = IMUAxis.kZ;
      pitch_axis = IMUAxis.kY;
      roll_axis = IMUAxis.kX;
    }

    m_yaw_axis = yaw_axis;
    m_pitch_axis = pitch_axis;
    m_roll_axis = roll_axis;

    m_calibration_time = cal_time.value;
    m_spi_port = port;

    m_acquire_task = new Thread(this::acquire);

    m_simDevice = SimDevice.create("Gyro:ADIS16470", port.value);
    if (m_simDevice != null) {
      m_simConnected = m_simDevice.createBoolean("connected", SimDevice.Direction.kInput, true);
      m_simGyroAngleX = m_simDevice.createDouble("gyro_angle_x", SimDevice.Direction.kInput, 0.0);
      m_simGyroAngleY = m_simDevice.createDouble("gyro_angle_y", SimDevice.Direction.kInput, 0.0);
      m_simGyroAngleZ = m_simDevice.createDouble("gyro_angle_z", SimDevice.Direction.kInput, 0.0);
      m_simGyroRateX = m_simDevice.createDouble("gyro_rate_x", SimDevice.Direction.kInput, 0.0);
      m_simGyroRateY = m_simDevice.createDouble("gyro_rate_y", SimDevice.Direction.kInput, 0.0);
      m_simGyroRateZ = m_simDevice.createDouble("gyro_rate_z", SimDevice.Direction.kInput, 0.0);
      m_simAccelX = m_simDevice.createDouble("accel_x", SimDevice.Direction.kInput, 0.0);
      m_simAccelY = m_simDevice.createDouble("accel_y", SimDevice.Direction.kInput, 0.0);
      m_simAccelZ = m_simDevice.createDouble("accel_z", SimDevice.Direction.kInput, 0.0);
    }

    if (m_simDevice == null) {
      // Force the IMU reset pin to toggle on startup (doesn't require DS enable)
      // Relies on the RIO hardware by default configuring an output as low
      // and configuring an input as high Z. The 10k pull-up resistor internal to the
      // IMU then forces the reset line high for normal operation.
      var reset_out = new DigitalOutput(27); // Drive SPI CS2 (IMU RST) low
      Timer.delay(0.01);
      reset_out.close();
      m_reset_in = new DigitalInput(27); // Set SPI CS2 (IMU RST) high
      Timer.delay(0.25); // Wait for reset to complete

      if (!switchToStandardSPI()) {
        return;
      }
      boolean needsFlash = false;
      // Set IMU internal decimation to 4 (ODR = 2000 SPS / (4 + 1) = 400Hz), BW = 200Hz
      if (readRegister(DEC_RATE) != 0x0004) {
        writeRegister(DEC_RATE, 0x0004);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16470: DEC_RATE register configuration inconsistent! Scheduling flash update.",
            false);
      }

      // Set data ready polarity (HIGH = Good Data), Disable gSense Compensation and PoP
      if (readRegister(MSC_CTRL) != 0x0001) {
        writeRegister(MSC_CTRL, 0x0001);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16470: MSC_CTRL register configuration inconsistent! Scheduling flash update.",
            false);
      }

      // Disable IMU internal Bartlett filter (200Hz bandwidth is sufficient)
      if (readRegister(FILT_CTRL) != 0x0000) {
        writeRegister(FILT_CTRL, 0x0000);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16470: FILT_CTRL register configuration inconsistent! Scheduling flash update.",
            false);
      }

      // If any registers on the IMU don't match the config, trigger a flash update
      if (needsFlash) {
        DriverStation.reportWarning(
            "ADIS16470: Register configuration changed! Starting IMU flash update.", false);
        writeRegister(GLOB_CMD, 0x0008);
        // Wait long enough for the flash update to finish (72ms minimum as per the datasheet)
        Timer.delay(0.3);
        DriverStation.reportWarning("ADIS16470: Flash update finished!", false);
      } else {
        DriverStation.reportWarning(
            "ADIS16470: Flash and RAM configuration consistent. No flash update required!", false);
      }

      // Configure continuous bias calibration time based on user setting
      writeRegister(NULL_CNFG, m_calibration_time | 0x0700);

      // Notify DS that IMU calibration delay is active
      DriverStation.reportWarning("ADIS16470: Starting initial calibration delay.", false);

      // Wait for samples to accumulate internal to the IMU (110% of user-defined
      // time)
      try {
        Thread.sleep((long) (Math.pow(2.0, m_calibration_time) / 2000 * 64 * 1.1 * 1000));
      } catch (InterruptedException e) {
      }

      // Write offset calibration command to IMU
      writeRegister(GLOB_CMD, 0x0001);

      if (!switchToAutoSPI()) {
        return;
      }

      // Let the user know the IMU was initialized successfully
      DriverStation.reportWarning("ADIS16470 IMU Successfully Initialized!", false);

      // Drive "Ready" LED low
      m_status_led = new DigitalOutput(28); // Set SPI CS3 (IMU Ready LED) low
    }

    // Report usage and post data to DS
    HAL.report(tResourceType.kResourceType_ADIS16470, 0);
    m_connected = true;
  }

  /**
   * Checks the connection status of the IMU.
   *
   * @return True if the IMU is connected, false otherwise.
   */
  public boolean isConnected() {
    if (m_simConnected != null) {
      return m_simConnected.get();
    }
    return m_connected;
  }

  private static int toUShort(int upper, int lower) {
    return ((upper & 0xFF) << 8) + (lower & 0xFF);
  }

  private static int toShort(int upper, int lower) {
    return (short) (((upper & 0xFF) << 8) + (lower & 0xFF));
  }

  private static int toInt(int msb, int byte2, int byte3, int lsb) {
    return (msb & 0xFF) << 24 | (byte2 & 0xFF) << 16 | (byte3 & 0xFF) << 8 | (lsb & 0xFF);
  }

  /**
   * Switch to standard SPI mode.
   *
   * @return True if successful, false otherwise.
   */
  private boolean switchToStandardSPI() {
    // Check to see whether the acquire thread is active. If so, wait for it to stop
    // producing data.
    if (m_thread_active) {
      m_thread_active = false;
      while (!m_thread_idle) {
        try {
          Thread.sleep(10);
        } catch (InterruptedException e) {
        }
      }
      System.out.println("Paused the IMU processing thread successfully!");
      // Maybe we're in auto SPI mode? If so, kill auto SPI, and then SPI.
      if (m_spi != null && m_auto_configured) {
        m_spi.stopAuto();
        // We need to get rid of all the garbage left in the auto SPI buffer after
        // stopping it.
        // Sometimes data magically reappears, so we have to check the buffer size a
        // couple of times to be sure we got it all. Yuck.
        int[] trashBuffer = new int[200];
        try {
          Thread.sleep(100);
        } catch (InterruptedException e) {
        }
        int data_count = m_spi.readAutoReceivedData(trashBuffer, 0, 0);
        while (data_count > 0) {
          m_spi.readAutoReceivedData(trashBuffer, Math.min(data_count, 200), 0);
          data_count = m_spi.readAutoReceivedData(trashBuffer, 0, 0);
        }
        System.out.println("Paused auto SPI successfully.");
      }
    }
    if (m_spi == null) {
      m_spi = new SPI(m_spi_port);
      m_spi.setClockRate(2000000);
      m_spi.setMode(SPI.Mode.kMode3);
      m_spi.setChipSelectActiveLow();
    }
    readRegister(PROD_ID); // Dummy read
    // Validate the product ID
    int prodId = readRegister(PROD_ID);
    if (prodId != 16982 && prodId != 16470) {
      DriverStation.reportError("Could not find an ADIS16470; got product ID " + prodId, false);
      close();
      return false;
    }
    return true;
  }

  /**
   * Switch to auto SPI mode.
   *
   * @return True if successful, false otherwise.
   */
  boolean switchToAutoSPI() {
    // No SPI port has been set up. Go set one up first.
    if (m_spi == null && !switchToStandardSPI()) {
      DriverStation.reportError("Failed to start/restart auto SPI", false);
      return false;
    }
    // Only set up the interrupt if needed.
    if (m_auto_interrupt == null) {
      // Configure interrupt on SPI CS1
      m_auto_interrupt = new DigitalInput(26);
    }
    // The auto SPI controller gets angry if you try to set up two instances on one
    // bus.
    if (!m_auto_configured) {
      m_spi.initAuto(8200);
      m_auto_configured = true;
    }
    // Do we need to change auto SPI settings?
    m_spi.setAutoTransmitData(m_autospi_allAngle_packet, 2);
    // Configure auto stall time
    m_spi.configureAutoStall(5, 1000, 1);
    // Kick off auto SPI (Note: Device configuration impossible after auto SPI is
    // activated)
    // DR High = Data good (data capture should be triggered on the rising edge)
    m_spi.startAutoTrigger(m_auto_interrupt, true, false);
    // Check to see if the acquire thread is running. If not, kick one off.
    if (!m_acquire_task.isAlive()) {
      m_first_run = true;
      m_thread_active = true;
      m_acquire_task.start();
      System.out.println("Processing thread activated!");
    } else {
      // The thread was running, re-init run variables and start it up again.
      m_first_run = true;
      m_thread_active = true;
      System.out.println("Processing thread activated!");
    }
    // Looks like the thread didn't start for some reason. Abort.
    if (!m_acquire_task.isAlive()) {
      DriverStation.reportError("Failed to start/restart the acquire() thread.", false);
      close();
      return false;
    }
    return true;
  }

  /**
   * Configures calibration time.
   *
   * @param new_cal_time New calibration time
   * @return 0 if success, 1 if no change, 2 if error.
   */
  public int configCalTime(CalibrationTime new_cal_time) {
    if (m_calibration_time == new_cal_time.value) {
      return 1;
    }
    if (!switchToStandardSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure standard SPI.", false);
      return 2;
    }
    m_calibration_time = new_cal_time.value;
    writeRegister(NULL_CNFG, m_calibration_time | 0x700);
    if (!switchToAutoSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure auto SPI.", false);
      return 2;
    }
    return 0;
  }

  /**
   * Configures the decimation rate of the IMU.
   *
   * @param decimationRate The new decimation value.
   * @return 0 if success, 1 if no change, 2 if error.
   */
  public int configDecRate(int decimationRate) {
    // Switches the active SPI port to standard SPI mode, writes a new value to the DECIMATE
    // register in the IMU, adjusts the sample scale factor, and re-enables auto SPI.
    if (!switchToStandardSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure standard SPI.", false);
      return 2;
    }
    if (decimationRate > 1999) {
      DriverStation.reportError("Attempted to write an invalid decimation value.", false);
      decimationRate = 1999;
    }
    m_scaled_sample_rate = (decimationRate + 1.0) / 2000.0 * 1000000.0;
    writeRegister(DEC_RATE, decimationRate);
    System.out.println("Decimation register: " + readRegister(DEC_RATE));
    if (!switchToAutoSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure auto SPI.", false);
      return 2;
    }
    return 0;
  }

  /**
   * Calibrate the gyro. It's important to make sure that the robot is not moving while the
   * calibration is in progress, this is typically done when the robot is first turned on while it's
   * sitting at rest before the match starts.
   */
  public void calibrate() {
    if (!switchToStandardSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure standard SPI.", false);
    }
    writeRegister(GLOB_CMD, 0x0001);
    if (!switchToAutoSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure auto SPI.", false);
    }
  }

  private int readRegister(int reg) {
    final byte[] buf = {(byte) (reg & 0x7f), 0};

    m_spi.write(buf, 2);
    m_spi.read(false, buf, 2);

    return toUShort(buf[0], buf[1]);
  }

  private void writeRegister(int reg, int val) {
    // low byte
    final byte[] buf = {(byte) (0x80 | reg), (byte) (val & 0xff)};
    m_spi.write(buf, 2);
    // high byte
    buf[0] = (byte) (0x81 | reg);
    buf[1] = (byte) (val >> 8);
    m_spi.write(buf, 2);
  }

  /** Delete (free) the spi port used for the IMU. */
  @Override
  public void close() {
    if (m_thread_active) {
      m_thread_active = false;
      try {
        if (m_acquire_task != null) {
          m_acquire_task.join();
          m_acquire_task = null;
        }
      } catch (InterruptedException e) {
      }
      if (m_spi != null) {
        if (m_auto_configured) {
          m_spi.stopAuto();
        }
        m_spi.close();
        m_auto_configured = false;
        if (m_auto_interrupt != null) {
          m_auto_interrupt.close();
          m_auto_interrupt = null;
        }
        m_spi = null;
      }
    }
    if (m_simDevice != null) {
      m_simDevice.close();
      m_simDevice = null;
    }
    System.out.println("Finished cleaning up after the IMU driver.");
  }

  private void acquire() {
    // Set data packet length
    final int dataset_len = 27; // 26 data points + timestamp
    final int BUFFER_SIZE = 4000;

    // Set up buffers and variables
    int[] buffer = new int[BUFFER_SIZE];
    double previous_timestamp = 0.0;
    double compAngleX = 0.0;
    double compAngleY = 0.0;
    while (true) {
      // Wait for data
      try {
        Thread.sleep(10);
      } catch (InterruptedException e) {
      }

      if (m_thread_active) {
        m_thread_idle = false;

        // Read number of bytes currently stored in the buffer
        int data_count = m_spi.readAutoReceivedData(buffer, 0, 0);
        // Check if frame is incomplete
        int data_remainder = data_count % dataset_len;
        // Remove incomplete data from read count
        int data_to_read = data_count - data_remainder;
        // Want to cap the data to read in a single read at the buffer size
        if (data_to_read > BUFFER_SIZE) {
          DriverStation.reportWarning(
              "ADIS16470 data processing thread overrun has occurred!", false);
          data_to_read = BUFFER_SIZE - (BUFFER_SIZE % dataset_len);
        }
        // Read data from DMA buffer (only complete sets)
        m_spi.readAutoReceivedData(buffer, data_to_read, 0);

        // Could be multiple data sets in the buffer. Handle each one.
        for (int i = 0; i < data_to_read; i += dataset_len) {
          // Timestamp is at buffer[i]
          m_dt = (buffer[i] - previous_timestamp) / 1000000.0;
          // Get delta angle value for all 3 axes and scale by the elapsed time
          // (based on timestamp)
          double elapsed_time = m_scaled_sample_rate / (buffer[i] - previous_timestamp);
          double delta_angle_x =
              toInt(buffer[i + 3], buffer[i + 4], buffer[i + 5], buffer[i + 6])
                  * kDeltaAngleSf
                  / elapsed_time;
          double delta_angle_y =
              toInt(buffer[i + 7], buffer[i + 8], buffer[i + 9], buffer[i + 10])
                  * kDeltaAngleSf
                  / elapsed_time;
          double delta_angle_z =
              toInt(buffer[i + 11], buffer[i + 12], buffer[i + 13], buffer[i + 14])
                  * kDeltaAngleSf
                  / elapsed_time;

          double gyro_rate_x = toShort(buffer[i + 15], buffer[i + 16]) / 10.0;
          double gyro_rate_y = toShort(buffer[i + 17], buffer[i + 18]) / 10.0;
          double gyro_rate_z = toShort(buffer[i + 19], buffer[i + 20]) / 10.0;

          double accel_x = toShort(buffer[i + 21], buffer[i + 22]) / 800.0;
          double accel_y = toShort(buffer[i + 23], buffer[i + 24]) / 800.0;
          double accel_z = toShort(buffer[i + 25], buffer[i + 26]) / 800.0;

          // Convert scaled sensor data to SI units (for tilt calculations)
          // TODO: Should the unit outputs be selectable?
          double gyro_rate_x_si = Math.toRadians(gyro_rate_x);
          double gyro_rate_y_si = Math.toRadians(gyro_rate_y);
          // double gyro_rate_z_si = Math.toRadians(gyro_rate_z);
          double accel_x_si = accel_x * kGrav;
          double accel_y_si = accel_y * kGrav;
          double accel_z_si = accel_z * kGrav;

          // Store timestamp for next iteration
          previous_timestamp = buffer[i];

          m_alpha = kTau / (kTau + m_dt);

          // Run inclinometer calculations
          double accelAngleX = Math.atan2(accel_x_si, Math.hypot(accel_y_si, accel_z_si));
          double accelAngleY = Math.atan2(accel_y_si, Math.hypot(accel_x_si, accel_z_si));
          if (m_first_run) {
            compAngleX = accelAngleX;
            compAngleY = accelAngleY;
          } else {
            accelAngleX = formatAccelRange(accelAngleX, accel_z_si);
            accelAngleY = formatAccelRange(accelAngleY, accel_z_si);
            compAngleX = compFilterProcess(compAngleX, accelAngleX, -gyro_rate_y_si);
            compAngleY = compFilterProcess(compAngleY, accelAngleY, gyro_rate_x_si);
          }

          synchronized (this) {
            // Push data to global variables
            if (m_first_run) {
              // Don't accumulate first run. previous_timestamp will be "very" old and the
              // integration will end up way off
              reset();
            } else {
              m_integ_angle_x += delta_angle_x;
              m_integ_angle_y += delta_angle_y;
              m_integ_angle_z += delta_angle_z;
            }
            m_gyro_rate_x = gyro_rate_x;
            m_gyro_rate_y = gyro_rate_y;
            m_gyro_rate_z = gyro_rate_z;
            m_accel_x = accel_x;
            m_accel_y = accel_y;
            m_accel_z = accel_z;
            m_compAngleX = Math.toDegrees(compAngleX);
            m_compAngleY = Math.toDegrees(compAngleY);
            m_accelAngleX = Math.toDegrees(accelAngleX);
            m_accelAngleY = Math.toDegrees(accelAngleY);
          }
          m_first_run = false;
        }
      } else {
        m_thread_idle = true;
        previous_timestamp = 0.0;
        compAngleX = 0.0;
        compAngleY = 0.0;
      }
    }
  }

  private double formatFastConverge(double compAngle, double accAngle) {
    if (compAngle > accAngle + Math.PI) {
      compAngle = compAngle - 2.0 * Math.PI;
    } else if (accAngle > compAngle + Math.PI) {
      compAngle = compAngle + 2.0 * Math.PI;
    }
    return compAngle;
  }

  private double formatAccelRange(double accelAngle, double accelZ) {
    if (accelZ < 0.0) {
      accelAngle = Math.PI - accelAngle;
    } else if (accelZ > 0.0 && accelAngle < 0.0) {
      accelAngle = 2.0 * Math.PI + accelAngle;
    }
    return accelAngle;
  }

  private double compFilterProcess(double compAngle, double accelAngle, double omega) {
    compAngle = formatFastConverge(compAngle, accelAngle);
    compAngle = m_alpha * (compAngle + omega * m_dt) + (1.0 - m_alpha) * accelAngle;
    return MathUtil.angleModulus(compAngle);
  }

  /**
   * Reset the gyro.
   *
   * <p>Resets the gyro accumulations to a heading of zero. This can be used if there is significant
   * drift in the gyro and it needs to be recalibrated after running.
   */
  public void reset() {
    synchronized (this) {
      m_integ_angle_x = 0.0;
      m_integ_angle_y = 0.0;
      m_integ_angle_z = 0.0;
    }
  }

  /**
   * Allow the designated gyro angle to be set to a given value. This may happen with unread values
   * in the buffer, it is suggested that the IMU is not moving when this method is run.
   *
   * @param axis IMUAxis that will be changed
   * @param angle A double in degrees (CCW positive)
   */
  public void setGyroAngle(IMUAxis axis, double angle) {
    axis =
        switch (axis) {
          case kYaw -> m_yaw_axis;
          case kPitch -> m_pitch_axis;
          case kRoll -> m_roll_axis;
          default -> axis;
        };

    switch (axis) {
      case kX -> setGyroAngleX(angle);
      case kY -> setGyroAngleY(angle);
      case kZ -> setGyroAngleZ(angle);
      default -> {
        // NOP
      }
    }
  }

  /**
   * Allow the gyro angle X to be set to a given value. This may happen with unread values in the
   * buffer, it is suggested that the IMU is not moving when this method is run.
   *
   * @param angle A double in degrees (CCW positive)
   */
  public void setGyroAngleX(double angle) {
    synchronized (this) {
      m_integ_angle_x = angle;
    }
  }

  /**
   * Allow the gyro angle Y to be set to a given value. This may happen with unread values in the
   * buffer, it is suggested that the IMU is not moving when this method is run.
   *
   * @param angle A double in degrees (CCW positive)
   */
  public void setGyroAngleY(double angle) {
    synchronized (this) {
      m_integ_angle_y = angle;
    }
  }

  /**
   * Allow the gyro angle Z to be set to a given value. This may happen with unread values in the
   * buffer, it is suggested that the IMU is not moving when this method is run.
   *
   * @param angle A double in degrees (CCW positive)
   */
  public void setGyroAngleZ(double angle) {
    synchronized (this) {
      m_integ_angle_z = angle;
    }
  }

  /**
   * Returns the axis angle in degrees (CCW positive).
   *
   * @param axis The IMUAxis whose angle to return.
   * @return The axis angle in degrees (CCW positive).
   */
  public synchronized double getAngle(IMUAxis axis) {
    axis =
        switch (axis) {
          case kYaw -> m_yaw_axis;
          case kPitch -> m_pitch_axis;
          case kRoll -> m_roll_axis;
          default -> axis;
        };

    return switch (axis) {
      case kX -> {
        if (m_simGyroAngleX != null) {
          yield m_simGyroAngleX.get();
        }
        yield m_integ_angle_x;
      }
      case kY -> {
        if (m_simGyroAngleY != null) {
          yield m_simGyroAngleY.get();
        }
        yield m_integ_angle_y;
      }
      case kZ -> {
        if (m_simGyroAngleZ != null) {
          yield m_simGyroAngleZ.get();
        }
        yield m_integ_angle_z;
      }
      default -> 0.0;
    };
  }

  /**
   * Returns the Yaw axis angle in degrees (CCW positive).
   *
   * @return The Yaw axis angle in degrees (CCW positive).
   */
  public synchronized double getAngle() {
    return getAngle(m_yaw_axis);
  }

  /**
   * Returns the axis angular rate in degrees per second (CCW positive).
   *
   * @param axis The IMUAxis whose rate to return.
   * @return Axis angular rate in degrees per second (CCW positive).
   */
  public synchronized double getRate(IMUAxis axis) {
    axis =
        switch (axis) {
          case kYaw -> m_yaw_axis;
          case kPitch -> m_pitch_axis;
          case kRoll -> m_roll_axis;
          default -> axis;
        };

    return switch (axis) {
      case kX -> {
        if (m_simGyroRateX != null) {
          yield m_simGyroRateX.get();
        }
        yield m_gyro_rate_x;
      }
      case kY -> {
        if (m_simGyroRateY != null) {
          yield m_simGyroRateY.get();
        }
        yield m_gyro_rate_y;
      }
      case kZ -> {
        if (m_simGyroRateZ != null) {
          yield m_simGyroRateZ.get();
        }
        yield m_gyro_rate_z;
      }
      default -> 0.0;
    };
  }

  /**
   * Returns the Yaw axis angular rate in degrees per second (CCW positive).
   *
   * @return Yaw axis angular rate in degrees per second (CCW positive).
   */
  public synchronized double getRate() {
    return getRate(m_yaw_axis);
  }

  /**
   * Returns which axis, kX, kY, or kZ, is set to the yaw axis.
   *
   * @return IMUAxis Yaw Axis
   */
  public IMUAxis getYawAxis() {
    return m_yaw_axis;
  }

  /**
   * Returns which axis, kX, kY, or kZ, is set to the pitch axis.
   *
   * @return IMUAxis Pitch Axis
   */
  public IMUAxis getPitchAxis() {
    return m_pitch_axis;
  }

  /**
   * Returns which axis, kX, kY, or kZ, is set to the roll axis.
   *
   * @return IMUAxis Roll Axis
   */
  public IMUAxis getRollAxis() {
    return m_roll_axis;
  }

  /**
   * Returns the acceleration in the X axis in meters per second squared.
   *
   * @return The acceleration in the X axis in meters per second squared.
   */
  public synchronized double getAccelX() {
    return m_accel_x * kGrav;
  }

  /**
   * Returns the acceleration in the Y axis in meters per second squared.
   *
   * @return The acceleration in the Y axis in meters per second squared.
   */
  public synchronized double getAccelY() {
    return m_accel_y * kGrav;
  }

  /**
   * Returns the acceleration in the Z axis in meters per second squared.
   *
   * @return The acceleration in the Z axis in meters per second squared.
   */
  public synchronized double getAccelZ() {
    return m_accel_z * kGrav;
  }

  /**
   * Returns the complementary angle around the X axis computed from accelerometer and gyro rate
   * measurements.
   *
   * @return The X-axis complementary angle in degrees.
   */
  public synchronized double getXComplementaryAngle() {
    return m_compAngleX;
  }

  /**
   * Returns the complementary angle around the Y axis computed from accelerometer and gyro rate
   * measurements.
   *
   * @return The Y-axis complementary angle in degrees.
   */
  public synchronized double getYComplementaryAngle() {
    return m_compAngleY;
  }

  /**
   * Returns the X-axis filtered acceleration angle in degrees.
   *
   * @return The X-axis filtered acceleration angle in degrees.
   */
  public synchronized double getXFilteredAccelAngle() {
    return m_accelAngleX;
  }

  /**
   * Returns the Y-axis filtered acceleration angle in degrees.
   *
   * @return The Y-axis filtered acceleration angle in degrees.
   */
  public synchronized double getYFilteredAccelAngle() {
    return m_accelAngleY;
  }

  /**
   * Gets the SPI port number.
   *
   * @return The SPI port number.
   */
  public int getPort() {
    return m_spi_port.value;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Gyro");
    builder.addDoubleProperty("Value", () -> getAngle(m_yaw_axis), null);
  }
}
