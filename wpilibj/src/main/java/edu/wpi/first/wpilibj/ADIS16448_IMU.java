// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 Analog Devices Inc. All Rights Reserved.           */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*                                                                            */
/* Modified by Juan Chong - frcsupport@analog.com                             */
/*----------------------------------------------------------------------------*/

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

/** This class is for the ADIS16448 IMU that connects to the RoboRIO MXP port. */
@SuppressWarnings({"PMD.RedundantFieldInitializer", "PMD.ImmutableField"})
public class ADIS16448_IMU implements AutoCloseable, Sendable {
  // ADIS16448 Register Map Declaration
  private static final int FLASH_CNT = 0x00; // Flash memory write count

  private static final int XGYRO_OUT = 0x04; // X-axis gyroscope output
  private static final int YGYRO_OUT = 0x06; // Y-axis gyroscope output
  private static final int ZGYRO_OUT = 0x08; // Z-axis gyroscope output
  private static final int XACCL_OUT = 0x0A; // X-axis accelerometer output
  private static final int YACCL_OUT = 0x0C; // Y-axis accelerometer output
  private static final int ZACCL_OUT = 0x0E; // Z-axis accelerometer output
  private static final int XMAGN_OUT = 0x10; // X-axis magnetometer output
  private static final int YMAGN_OUT = 0x12; // Y-axis magnetometer output
  private static final int ZMAGN_OUT = 0x14; // Z-axis magnetometer output
  private static final int BARO_OUT = 0x16; // Barometer pressure measurement, high word
  private static final int TEMP_OUT = 0x18; // Temperature output
  private static final int XGYRO_OFF = 0x1A; // X-axis gyroscope bias offset factor
  private static final int YGYRO_OFF = 0x1C; // Y-axis gyroscope bias offset factor
  private static final int ZGYRO_OFF = 0x1E; // Z-axis gyroscope bias offset factor
  private static final int XACCL_OFF = 0x20; // X-axis acceleration bias offset factor
  private static final int YACCL_OFF = 0x22; // Y-axis acceleration bias offset factor
  private static final int ZACCL_OFF = 0x24; // Z-axis acceleration bias offset factor
  private static final int XMAGN_HIC = 0x26; // X-axis magnetometer, hard iron factor
  private static final int YMAGN_HIC = 0x28; // Y-axis magnetometer, hard iron factor
  private static final int ZMAGN_HIC = 0x2A; // Z-axis magnetometer, hard iron factor
  private static final int XMAGN_SIC = 0x2C; // X-axis magnetometer, soft iron factor
  private static final int YMAGN_SIC = 0x2E; // Y-axis magnetometer, soft iron factor
  private static final int ZMAGN_SIC = 0x30; // Z-axis magnetometer, soft iron factor
  private static final int GPIO_CTRL = 0x32; // GPIO control
  private static final int MSC_CTRL = 0x34; // MISC control
  private static final int SMPL_PRD = 0x36; // Sample clock/Decimation filter control
  private static final int SENS_AVG = 0x38; // Digital filter control
  private static final int SEQ_CNT = 0x3A; // MAGN_OUT and BARO_OUT counter
  private static final int DIAG_STAT = 0x3C; // System status
  private static final int GLOB_CMD = 0x3E; // System command
  private static final int ALM_MAG1 = 0x40; // Alarm 1 amplitude threshold
  private static final int ALM_MAG2 = 0x42; // Alarm 2 amplitude threshold
  private static final int ALM_SMPL1 = 0x44; // Alarm 1 sample size
  private static final int ALM_SMPL2 = 0x46; // Alarm 2 sample size
  private static final int ALM_CTRL = 0x48; // Alarm control
  private static final int LOT_ID1 = 0x52; // Lot identification number
  private static final int LOT_ID2 = 0x54; // Lot identification number
  private static final int PROD_ID = 0x56; // Product identifier
  private static final int SERIAL_NUM = 0x58; // Lot-specific serial number

  /** ADIS16448 calibration times. */
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

  /** IMU axes. */
  public enum IMUAxis {
    /** The IMU's X axis. */
    kX,
    /** The IMU's Y axis. */
    kY,
    /** The IMU's Z axis. */
    kZ
  }

  private static final double kGrav = 9.81;

  // User-specified yaw axis
  private IMUAxis m_yaw_axis;

  // Offset data storage
  private double[] m_offset_data_gyro_rate_x;
  private double[] m_offset_data_gyro_rate_y;
  private double[] m_offset_data_gyro_rate_z;

  // Instant raw output variables
  private double m_gyro_rate_x = 0.0;
  private double m_gyro_rate_y = 0.0;
  private double m_gyro_rate_z = 0.0;
  private double m_accel_x = 0.0;
  private double m_accel_y = 0.0;
  private double m_accel_z = 0.0;
  private double m_mag_x = 0.0;
  private double m_mag_y = 0.0;
  private double m_mag_z = 0.0;
  private double m_baro = 0.0;
  private double m_temp = 0.0;

  // IMU gyro offset variables
  private double m_gyro_rate_offset_x = 0.0;
  private double m_gyro_rate_offset_y = 0.0;
  private double m_gyro_rate_offset_z = 0.0;
  private int m_avg_size = 0;
  private int m_accum_count = 0;

  // Integrated gyro angle variables
  private double m_integ_gyro_angle_x = 0.0;
  private double m_integ_gyro_angle_y = 0.0;
  private double m_integ_gyro_angle_z = 0.0;

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
  private CalibrationTime m_calibration_time = CalibrationTime._32ms;
  private volatile boolean m_first_run = true;
  private volatile boolean m_thread_idle = false;
  private boolean m_auto_configured = false;
  private boolean m_start_up_mode = true;

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

  // CRC-16 Look-Up Table
  private int[] m_adiscrc = {
    0x0000, 0x17CE, 0x0FDF, 0x1811, 0x1FBE, 0x0870, 0x1061, 0x07AF,
    0x1F3F, 0x08F1, 0x10E0, 0x072E, 0x0081, 0x174F, 0x0F5E, 0x1890,
    0x1E3D, 0x09F3, 0x11E2, 0x062C, 0x0183, 0x164D, 0x0E5C, 0x1992,
    0x0102, 0x16CC, 0x0EDD, 0x1913, 0x1EBC, 0x0972, 0x1163, 0x06AD,
    0x1C39, 0x0BF7, 0x13E6, 0x0428, 0x0387, 0x1449, 0x0C58, 0x1B96,
    0x0306, 0x14C8, 0x0CD9, 0x1B17, 0x1CB8, 0x0B76, 0x1367, 0x04A9,
    0x0204, 0x15CA, 0x0DDB, 0x1A15, 0x1DBA, 0x0A74, 0x1265, 0x05AB,
    0x1D3B, 0x0AF5, 0x12E4, 0x052A, 0x0285, 0x154B, 0x0D5A, 0x1A94,
    0x1831, 0x0FFF, 0x17EE, 0x0020, 0x078F, 0x1041, 0x0850, 0x1F9E,
    0x070E, 0x10C0, 0x08D1, 0x1F1F, 0x18B0, 0x0F7E, 0x176F, 0x00A1,
    0x060C, 0x11C2, 0x09D3, 0x1E1D, 0x19B2, 0x0E7C, 0x166D, 0x01A3,
    0x1933, 0x0EFD, 0x16EC, 0x0122, 0x068D, 0x1143, 0x0952, 0x1E9C,
    0x0408, 0x13C6, 0x0BD7, 0x1C19, 0x1BB6, 0x0C78, 0x1469, 0x03A7,
    0x1B37, 0x0CF9, 0x14E8, 0x0326, 0x0489, 0x1347, 0x0B56, 0x1C98,
    0x1A35, 0x0DFB, 0x15EA, 0x0224, 0x058B, 0x1245, 0x0A54, 0x1D9A,
    0x050A, 0x12C4, 0x0AD5, 0x1D1B, 0x1AB4, 0x0D7A, 0x156B, 0x02A5,
    0x1021, 0x07EF, 0x1FFE, 0x0830, 0x0F9F, 0x1851, 0x0040, 0x178E,
    0x0F1E, 0x18D0, 0x00C1, 0x170F, 0x10A0, 0x076E, 0x1F7F, 0x08B1,
    0x0E1C, 0x19D2, 0x01C3, 0x160D, 0x11A2, 0x066C, 0x1E7D, 0x09B3,
    0x1123, 0x06ED, 0x1EFC, 0x0932, 0x0E9D, 0x1953, 0x0142, 0x168C,
    0x0C18, 0x1BD6, 0x03C7, 0x1409, 0x13A6, 0x0468, 0x1C79, 0x0BB7,
    0x1327, 0x04E9, 0x1CF8, 0x0B36, 0x0C99, 0x1B57, 0x0346, 0x1488,
    0x1225, 0x05EB, 0x1DFA, 0x0A34, 0x0D9B, 0x1A55, 0x0244, 0x158A,
    0x0D1A, 0x1AD4, 0x02C5, 0x150B, 0x12A4, 0x056A, 0x1D7B, 0x0AB5,
    0x0810, 0x1FDE, 0x07CF, 0x1001, 0x17AE, 0x0060, 0x1871, 0x0FBF,
    0x172F, 0x00E1, 0x18F0, 0x0F3E, 0x0891, 0x1F5F, 0x074E, 0x1080,
    0x162D, 0x01E3, 0x19F2, 0x0E3C, 0x0993, 0x1E5D, 0x064C, 0x1182,
    0x0912, 0x1EDC, 0x06CD, 0x1103, 0x16AC, 0x0162, 0x1973, 0x0EBD,
    0x1429, 0x03E7, 0x1BF6, 0x0C38, 0x0B97, 0x1C59, 0x0448, 0x1386,
    0x0B16, 0x1CD8, 0x04C9, 0x1307, 0x14A8, 0x0366, 0x1B77, 0x0CB9,
    0x0A14, 0x1DDA, 0x05CB, 0x1205, 0x15AA, 0x0264, 0x1A75, 0x0DBB,
    0x152B, 0x02E5, 0x1AF4, 0x0D3A, 0x0A95, 0x1D5B, 0x054A, 0x1284
  };

  /** Creates a new ADIS16448_IMU object. */
  public ADIS16448_IMU() {
    this(IMUAxis.kZ, SPI.Port.kMXP, CalibrationTime._512ms);
  }

  /**
   * Creates a new ADIS16448_IMU object.
   *
   * @param yaw_axis The axis that measures the yaw
   * @param port The SPI Port the gyro is plugged into
   * @param cal_time Calibration time
   */
  @SuppressWarnings("this-escape")
  public ADIS16448_IMU(final IMUAxis yaw_axis, SPI.Port port, CalibrationTime cal_time) {
    m_yaw_axis = yaw_axis;
    m_spi_port = port;

    m_acquire_task = new Thread(this::acquire);

    m_simDevice = SimDevice.create("Gyro:ADIS16448", port.value);
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
      var reset_out = new DigitalOutput(18); // Drive MXP DIO8 low
      Timer.delay(0.01);
      reset_out.close();
      m_reset_in = new DigitalInput(18); // Set MXP DIO8 high
      Timer.delay(0.25); // Wait for reset to complete

      configCalTime(cal_time);

      if (!switchToStandardSPI()) {
        return;
      }
      boolean needsFlash = false;
      // Set IMU internal decimation to 1 (ODR = 819.2 SPS / (1 + 1) = 409.6Hz), BW = 204.8Hz
      if (readRegister(SMPL_PRD) != 0x0001) {
        writeRegister(SMPL_PRD, 0x0001);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16448: SMPL_PRD register configuration inconsistent! Scheduling flash update.",
            false);
      }

      // Set data ready polarity (LOW = Good Data) on DIO1 (PWM0 on MXP)
      if (readRegister(MSC_CTRL) != 0x0016) {
        writeRegister(MSC_CTRL, 0x0016);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16448: MSC_CTRL register configuration inconsistent! Scheduling flash update.",
            false);
      }

      // Disable IMU internal Bartlett filter (204Hz BW is sufficient) and set IMU scale factor
      if (readRegister(SENS_AVG) != 0x0400) {
        writeRegister(SENS_AVG, 0x0400);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16448: SENS_AVG register configuration inconsistent! Scheduling flash update.",
            false);
      }
      // Clear offset registers
      if (readRegister(XGYRO_OFF) != 0x0000) {
        writeRegister(XGYRO_OFF, 0x0000);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16448: XGYRO_OFF register configuration inconsistent! Scheduling flash update.",
            false);
      }

      if (readRegister(YGYRO_OFF) != 0x0000) {
        writeRegister(YGYRO_OFF, 0x0000);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16448: YGYRO_OFF register configuration inconsistent! Scheduling flash update.",
            false);
      }

      if (readRegister(ZGYRO_OFF) != 0x0000) {
        writeRegister(ZGYRO_OFF, 0x0000);
        needsFlash = true;
        DriverStation.reportWarning(
            "ADIS16448: ZGYRO_OFF register configuration inconsistent! Scheduling flash update.",
            false);
      }

      // If any registers on the IMU don't match the config, trigger a flash update
      if (needsFlash) {
        DriverStation.reportWarning(
            "ADIS16448: Register configuration changed! Starting IMU flash update.", false);
        writeRegister(GLOB_CMD, 0x0008);
        // Wait long enough for the flash update to finish (75ms minimum as per the datasheet)
        Timer.delay(0.5);
        DriverStation.reportWarning("ADIS16448: Flash update finished!", false);
      } else {
        DriverStation.reportWarning(
            "ADIS16448: Flash and RAM configuration consistent. No flash update required!", false);
      }

      if (!switchToAutoSPI()) {
        return;
      }
      // Notify DS that IMU calibration delay is active
      DriverStation.reportWarning("ADIS16448: Starting initial calibration delay.", false);
      // Wait for whatever time the user set as the start-up delay
      try {
        Thread.sleep((long) (m_calibration_time.value * 1.2 * 1000));
      } catch (InterruptedException e) {
      }
      // Execute calibration routine and reset accumulated offsets
      calibrate();
      // Indicate to the acquire loop that we're done starting up
      m_start_up_mode = false;
      // Let the user know the IMU was initialized successfully
      DriverStation.reportWarning("ADIS16448 IMU Successfully Initialized!", false);
      // Drive MXP PWM5 (IMU ready LED) low (active low)
      m_status_led = new DigitalOutput(19);
    }

    // Report usage and post data to DS
    HAL.report(tResourceType.kResourceType_ADIS16448, 0);
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
          // Dequeue 200 at a time, or the remainder of the buffer if less than 200
          m_spi.readAutoReceivedData(trashBuffer, Math.min(200, data_count), 0);
          // Update remaining buffer count
          data_count = m_spi.readAutoReceivedData(trashBuffer, 0, 0);
        }
        System.out.println("Paused auto SPI successfully.");
      }
    }
    if (m_spi == null) {
      m_spi = new SPI(m_spi_port);
      m_spi.setClockRate(1000000);
      m_spi.setMode(SPI.Mode.kMode3);
      m_spi.setChipSelectActiveLow();
    }
    readRegister(PROD_ID); // Dummy read
    // Validate the product ID
    int prodId = readRegister(PROD_ID);
    if (prodId != 16448) {
      DriverStation.reportError("Could not find ADIS16448; got product ID " + prodId, false);
      close();
      return false;
    }
    return true;
  }

  boolean switchToAutoSPI() {
    // No SPI port has been set up. Go set one up first.
    if (m_spi == null && !switchToStandardSPI()) {
      DriverStation.reportError("Failed to start/restart auto SPI", false);
      return false;
    }
    // Only set up the interrupt if needed.
    if (m_auto_interrupt == null) {
      m_auto_interrupt = new DigitalInput(10); // MXP DIO0
    }
    // The auto SPI controller gets angry if you try to set up two instances on one
    // bus.
    if (!m_auto_configured) {
      m_spi.initAuto(8200);
      m_auto_configured = true;
    }
    // Set auto SPI packet data and size
    m_spi.setAutoTransmitData(new byte[] {GLOB_CMD}, 27);
    // Configure auto stall time
    m_spi.configureAutoStall(100, 1000, 255);
    // Kick off auto SPI (Note: Device configuration impossible after auto SPI is
    // activated)
    m_spi.startAutoTrigger(m_auto_interrupt, true, false);
    // Check to see if the acquire thread is running. If not, kick one off.
    if (!m_acquire_task.isAlive()) {
      m_first_run = true;
      m_thread_active = true;
      m_acquire_task.start();
      System.out.println("New IMU Processing thread activated!");
    } else {
      // The thread was running, re-init run variables and start it up again.
      m_first_run = true;
      m_thread_active = true;
      System.out.println("Old IMU Processing thread re-activated!");
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

    // Check max
    if (decimationRate > 9) {
      DriverStation.reportError(
          "Attempted to write an invalid decimation value. Capping at 9", false);
      decimationRate = 9;
    }
    if (decimationRate < 0) {
      DriverStation.reportError(
          "Attempted to write an invalid decimation value. Capping at 0", false);
      decimationRate = 0;
    }

    // Shift decimation setting to correct position and select internal sync
    int writeValue = (decimationRate << 8) | 0x1;

    // Apply to IMU
    writeRegister(SMPL_PRD, writeValue);

    // Perform read back to verify write
    int readbackValue = readRegister(SMPL_PRD);

    // Throw error for invalid write
    if (readbackValue != writeValue) {
      DriverStation.reportError("ADIS16448 SMPL_PRD write failed.", false);
    }

    if (!switchToAutoSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure auto SPI.", false);
      return 2;
    }
    return 0;
  }

  /**
   * Configures calibration time.
   *
   * @param new_cal_time New calibration time
   * @return 1 if the new calibration time is the same as the current one else 0
   */
  public final int configCalTime(CalibrationTime new_cal_time) {
    if (m_calibration_time == new_cal_time) {
      return 1;
    } else {
      m_calibration_time = new_cal_time;
      m_avg_size = m_calibration_time.value * 819;
      initOffsetBuffer(m_avg_size);
      return 0;
    }
  }

  private void initOffsetBuffer(int size) {
    // Avoid exceptions in the case of bad arguments
    if (size < 1) {
      size = 1;
    }
    // Set average size to size (correct bad values)
    m_avg_size = size;
    synchronized (this) {
      // Resize vector
      m_offset_data_gyro_rate_x = new double[size];
      m_offset_data_gyro_rate_y = new double[size];
      m_offset_data_gyro_rate_z = new double[size];
      // Set accumulate count to 0
      m_accum_count = 0;
    }
  }

  /**
   * Calibrate the gyro. It's important to make sure that the robot is not moving while the
   * calibration is in progress, this is typically done when the robot is first turned on while it's
   * sitting at rest before the match starts.
   */
  public void calibrate() {
    synchronized (this) {
      int gyroAverageSize = Math.min(m_accum_count, m_avg_size);
      double accum_gyro_rate_x = 0.0;
      double accum_gyro_rate_y = 0.0;
      double accum_gyro_rate_z = 0.0;
      for (int i = 0; i < gyroAverageSize; i++) {
        accum_gyro_rate_x += m_offset_data_gyro_rate_x[i];
        accum_gyro_rate_y += m_offset_data_gyro_rate_y[i];
        accum_gyro_rate_z += m_offset_data_gyro_rate_z[i];
      }
      m_gyro_rate_offset_x = accum_gyro_rate_x / gyroAverageSize;
      m_gyro_rate_offset_y = accum_gyro_rate_y / gyroAverageSize;
      m_gyro_rate_offset_z = accum_gyro_rate_z / gyroAverageSize;
      reset();
    }
  }

  /**
   * Sets the yaw axis.
   *
   * @param yaw_axis The new yaw axis to use
   * @return 1 if the new yaw axis is the same as the current one else 0.
   */
  public int setYawAxis(IMUAxis yaw_axis) {
    if (m_yaw_axis == yaw_axis) {
      return 1;
    }
    m_yaw_axis = yaw_axis;
    reset();
    return 0;
  }

  private int readRegister(final int reg) {
    final byte[] buf = {(byte) (reg & 0x7f), 0};

    m_spi.write(buf, 2);
    m_spi.read(false, buf, 2);

    return toUShort(buf[0], buf[1]);
  }

  private void writeRegister(final int reg, final int val) {
    final byte[] buf = new byte[2];
    // low byte
    buf[0] = (byte) (0x80 | reg);
    buf[1] = (byte) (val & 0xff);
    m_spi.write(buf, 2);
    // high byte
    buf[0] = (byte) (0x81 | reg);
    buf[1] = (byte) (val >> 8);
    m_spi.write(buf, 2);
  }

  /**
   * Reset the gyro.
   *
   * <p>Resets the gyro accumulations to a heading of zero. This can be used if there is significant
   * drift in the gyro and it needs to be recalibrated after running.
   */
  public void reset() {
    synchronized (this) {
      m_integ_gyro_angle_x = 0.0;
      m_integ_gyro_angle_y = 0.0;
      m_integ_gyro_angle_z = 0.0;
    }
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
    final int dataset_len = 29; // 18 data points + timestamp
    final int BUFFER_SIZE = 4000;

    // Set up buffers and variables
    int[] buffer = new int[BUFFER_SIZE];
    double previous_timestamp = 0.0;
    double compAngleX = 0.0;
    double compAngleY = 0.0;
    while (true) {
      // Wait for data
      try {
        Thread.sleep(5);
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
        if (data_to_read > BUFFER_SIZE) {
          DriverStation.reportWarning(
              "ADIS16448 data processing thread overrun has occurred!", false);
          data_to_read = BUFFER_SIZE - (BUFFER_SIZE % dataset_len);
        }
        // Read data from DMA buffer (only complete sets)
        m_spi.readAutoReceivedData(buffer, data_to_read, 0);

        // Could be multiple data sets in the buffer. Handle each one.
        for (int i = 0; i < data_to_read; i += dataset_len) {
          // Calculate CRC-16 on each data packet
          int calc_crc = 0xFFFF; // Starting word
          // Cycle through XYZ GYRO, XYZ ACCEL, XYZ MAG, BARO, TEMP (Ignore Status & CRC)
          for (int k = 5; k < 27; k += 2) {
            // Process LSB
            calc_crc = (calc_crc >>> 8) ^ m_adiscrc[(calc_crc & 0xFF) ^ buffer[i + k + 1]];
            // Process MSB
            calc_crc = (calc_crc >>> 8) ^ m_adiscrc[(calc_crc & 0xFF) ^ buffer[i + k]];
          }
          // Complement
          calc_crc = ~calc_crc & 0xFFFF;
          // Flip LSB & MSB
          calc_crc = ((calc_crc << 8) | (calc_crc >> 8)) & 0xFFFF;
          // Extract DUT CRC from data buffer
          int imu_crc = toUShort(buffer[i + 27], buffer[i + 28]);

          if (calc_crc == imu_crc) {
            // Timestamp is at buffer[i]
            m_dt = (buffer[i] - previous_timestamp) / 1000000.0;

            // Scale sensor data
            double gyro_rate_x = toShort(buffer[i + 5], buffer[i + 6]) * 0.04;
            double gyro_rate_y = toShort(buffer[i + 7], buffer[i + 8]) * 0.04;
            double gyro_rate_z = toShort(buffer[i + 9], buffer[i + 10]) * 0.04;
            double accel_x = toShort(buffer[i + 11], buffer[i + 12]) * 0.833;
            double accel_y = toShort(buffer[i + 13], buffer[i + 14]) * 0.833;
            double accel_z = toShort(buffer[i + 15], buffer[i + 16]) * 0.833;
            double mag_x = toShort(buffer[i + 17], buffer[i + 18]) * 0.1429;
            double mag_y = toShort(buffer[i + 19], buffer[i + 20]) * 0.1429;
            double mag_z = toShort(buffer[i + 21], buffer[i + 22]) * 0.1429;
            double baro = toShort(buffer[i + 23], buffer[i + 24]) * 0.02;
            double temp = toShort(buffer[i + 25], buffer[i + 26]) * 0.07386 + 31.0;

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
            // Calculate alpha for use with the complementary filter
            m_alpha = kTau / (kTau + m_dt);
            // Run inclinometer calculations
            double accelAngleX = Math.atan2(-accel_x_si, Math.hypot(accel_y_si, -accel_z_si));
            double accelAngleY = Math.atan2(accel_y_si, Math.hypot(-accel_x_si, -accel_z_si));
            // Calculate complementary filter
            if (m_first_run) {
              compAngleX = accelAngleX;
              compAngleY = accelAngleY;
            } else {
              accelAngleX = formatAccelRange(accelAngleX, -accel_z_si);
              accelAngleY = formatAccelRange(accelAngleY, -accel_z_si);
              compAngleX = compFilterProcess(compAngleX, accelAngleX, -gyro_rate_y_si);
              compAngleY = compFilterProcess(compAngleY, accelAngleY, -gyro_rate_x_si);
            }

            // Update global variables and state
            synchronized (this) {
              // Ignore first, integrated sample
              if (m_first_run) {
                reset();
              } else {
                // Accumulate gyro for offset calibration
                // Add to buffer
                int bufferAvgIndex = m_accum_count % m_avg_size;
                m_offset_data_gyro_rate_x[bufferAvgIndex] = gyro_rate_x;
                m_offset_data_gyro_rate_y[bufferAvgIndex] = gyro_rate_y;
                m_offset_data_gyro_rate_z[bufferAvgIndex] = gyro_rate_z;
                // Increment counter
                m_accum_count++;
              }
              if (!m_start_up_mode) {
                m_gyro_rate_x = gyro_rate_x;
                m_gyro_rate_y = gyro_rate_y;
                m_gyro_rate_z = gyro_rate_z;
                m_accel_x = accel_x;
                m_accel_y = accel_y;
                m_accel_z = accel_z;
                m_mag_x = mag_x;
                m_mag_y = mag_y;
                m_mag_z = mag_z;
                m_baro = baro;
                m_temp = temp;
                m_compAngleX = Math.toDegrees(compAngleX);
                m_compAngleY = Math.toDegrees(compAngleY);
                m_accelAngleX = Math.toDegrees(accelAngleX);
                m_accelAngleY = Math.toDegrees(accelAngleY);
                // Accumulate gyro for angle integration and publish to global variables
                m_integ_gyro_angle_x += (gyro_rate_x - m_gyro_rate_offset_x) * m_dt;
                m_integ_gyro_angle_y += (gyro_rate_y - m_gyro_rate_offset_y) * m_dt;
                m_integ_gyro_angle_z += (gyro_rate_z - m_gyro_rate_offset_z) * m_dt;
              }
            }
            m_first_run = false;
          }
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
   * Returns the yaw axis angle in degrees (CCW positive).
   *
   * @return Yaw axis angle in degrees (CCW positive).
   */
  public synchronized double getAngle() {
    return switch (m_yaw_axis) {
      case kX -> getGyroAngleX();
      case kY -> getGyroAngleY();
      case kZ -> getGyroAngleZ();
    };
  }

  /**
   * Returns the yaw axis angular rate in degrees per second (CCW positive).
   *
   * @return Yaw axis angular rate in degrees per second (CCW positive).
   */
  public synchronized double getRate() {
    return switch (m_yaw_axis) {
      case kX -> getGyroRateX();
      case kY -> getGyroRateY();
      case kZ -> getGyroRateZ();
    };
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
   * Returns the accumulated gyro angle in the X axis in degrees.
   *
   * @return The accumulated gyro angle in the X axis in degrees.
   */
  public synchronized double getGyroAngleX() {
    if (m_simGyroAngleX != null) {
      return m_simGyroAngleX.get();
    }
    return m_integ_gyro_angle_x;
  }

  /**
   * Returns the accumulated gyro angle in the Y axis in degrees.
   *
   * @return The accumulated gyro angle in the Y axis in degrees.
   */
  public synchronized double getGyroAngleY() {
    if (m_simGyroAngleY != null) {
      return m_simGyroAngleY.get();
    }
    return m_integ_gyro_angle_y;
  }

  /**
   * Returns the accumulated gyro angle in the Z axis in degrees.
   *
   * @return The accumulated gyro angle in the Z axis in degrees.
   */
  public synchronized double getGyroAngleZ() {
    if (m_simGyroAngleZ != null) {
      return m_simGyroAngleZ.get();
    }
    return m_integ_gyro_angle_z;
  }

  /**
   * Returns the gyro angular rate in the X axis in degrees per second.
   *
   * @return The gyro angular rate in the X axis in degrees per second.
   */
  public synchronized double getGyroRateX() {
    if (m_simGyroRateX != null) {
      return m_simGyroRateX.get();
    }
    return m_gyro_rate_x;
  }

  /**
   * Returns the gyro angular rate in the Y axis in degrees per second.
   *
   * @return The gyro angular rate in the Y axis in degrees per second.
   */
  public synchronized double getGyroRateY() {
    if (m_simGyroRateY != null) {
      return m_simGyroRateY.get();
    }
    return m_gyro_rate_y;
  }

  /**
   * Returns the gyro angular rate in the Z axis in degrees per second.
   *
   * @return The gyro angular rate in the Z axis in degrees per second.
   */
  public synchronized double getGyroRateZ() {
    if (m_simGyroRateZ != null) {
      return m_simGyroRateZ.get();
    }
    return m_gyro_rate_z;
  }

  /**
   * Returns the acceleration in the X axis in meters per second squared.
   *
   * @return The acceleration in the X axis in meters per second squared.
   */
  public synchronized double getAccelX() {
    if (m_simAccelX != null) {
      return m_simAccelX.get();
    }
    return m_accel_x * kGrav;
  }

  /**
   * Returns the acceleration in the Y axis in meters per second squared.
   *
   * @return The acceleration in the Y axis in meters per second squared.
   */
  public synchronized double getAccelY() {
    if (m_simAccelY != null) {
      return m_simAccelY.get();
    }
    return m_accel_y * kGrav;
  }

  /**
   * Returns the acceleration in the Z axis in meters per second squared.
   *
   * @return The acceleration in the Z axis in meters per second squared.
   */
  public synchronized double getAccelZ() {
    if (m_simAccelZ != null) {
      return m_simAccelZ.get();
    }
    return m_accel_z * kGrav;
  }

  /**
   * Returns the magnetic field strength in the X axis in Tesla.
   *
   * @return The magnetic field strength in the X axis in Tesla.
   */
  public synchronized double getMagneticFieldX() {
    // mG to T
    return m_mag_x * 1e-7;
  }

  /**
   * Returns the magnetic field strength in the Y axis in Tesla.
   *
   * @return The magnetic field strength in the Y axis in Tesla.
   */
  public synchronized double getMagneticFieldY() {
    // mG to T
    return m_mag_y * 1e-7;
  }

  /**
   * Returns the magnetic field strength in the Z axis in Tesla.
   *
   * @return The magnetic field strength in the Z axis in Tesla.
   */
  public synchronized double getMagneticFieldZ() {
    // mG to T
    return m_mag_z * 1e-7;
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
   * Returns the barometric pressure in PSI.
   *
   * @return The barometric pressure in PSI.
   */
  public synchronized double getBarometricPressure() {
    // mbar to PSI conversion
    return m_baro * 0.0145;
  }

  /**
   * Returns the temperature in degrees Celsius.
   *
   * @return The temperature in degrees Celsius.
   */
  public synchronized double getTemperature() {
    return m_temp;
  }

  /**
   * Get the SPI port number.
   *
   * @return The SPI port number.
   */
  public int getPort() {
    return m_spi_port.value;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Gyro");
    builder.addDoubleProperty("Value", this::getAngle, null);
  }
}
