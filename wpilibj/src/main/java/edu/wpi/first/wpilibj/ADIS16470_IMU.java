// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

// import java.lang.FdLibm.Pow;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.networktables.NTSendable;
import edu.wpi.first.networktables.NTSendableBuilder;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

// CHECKSTYLE.OFF: TypeName
// CHECKSTYLE.OFF: MemberName
// CHECKSTYLE.OFF: SummaryJavadoc
// CHECKSTYLE.OFF: UnnecessaryParentheses
// CHECKSTYLE.OFF: OverloadMethodsDeclarationOrder
// CHECKSTYLE.OFF: NonEmptyAtclauseDescription
// CHECKSTYLE.OFF: MissingOverride
// CHECKSTYLE.OFF: AtclauseOrder
// CHECKSTYLE.OFF: LocalVariableName
// CHECKSTYLE.OFF: RedundantModifier
// CHECKSTYLE.OFF: AbbreviationAsWordInName
// CHECKSTYLE.OFF: ParameterName
// CHECKSTYLE.OFF: EmptyCatchBlock
// CHECKSTYLE.OFF: MissingJavadocMethod
// CHECKSTYLE.OFF: MissingSwitchDefault
// CHECKSTYLE.OFF: VariableDeclarationUsageDistance
// CHECKSTYLE.OFF: ArrayTypeStyle

/** This class is for the ADIS16470 IMU that connects to the RoboRIO SPI port. */
@SuppressWarnings({
  "unused",
  "PMD.RedundantFieldInitializer",
  "PMD.ImmutableField",
  "PMD.SingularField",
  "PMD.CollapsibleIfStatements",
  "PMD.MissingOverride",
  "PMD.EmptyIfStmt",
  "PMD.EmptyStatementNotInLoop"
})
public class ADIS16470_IMU implements AutoCloseable, NTSendable {
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

  private static final byte[] m_autospi_x_packet = {
    X_DELTANG_OUT,
    FLASH_CNT,
    X_DELTANG_LOW,
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

  private static final byte[] m_autospi_y_packet = {
    Y_DELTANG_OUT,
    FLASH_CNT,
    Y_DELTANG_LOW,
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

  private static final byte[] m_autospi_z_packet = {
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

  public enum CalibrationTime {
    _32ms(0),
    _64ms(1),
    _128ms(2),
    _256ms(3),
    _512ms(4),
    _1s(5),
    _2s(6),
    _4s(7),
    _8s(8),
    _16s(9),
    _32s(10),
    _64s(11);

    private int value;

    private CalibrationTime(int value) {
      this.value = value;
    }
  }

  public enum IMUAxis {
    kX,
    kY,
    kZ
  }

  // Static Constants
  private static final double delta_angle_sf = 2160.0 / 2147483648.0; /* 2160 / (2^31) */
  private static final double rad_to_deg = 57.2957795;
  private static final double deg_to_rad = 0.0174532;
  private static final double grav = 9.81;

  // User-specified yaw axis
  private IMUAxis m_yaw_axis;

  // Instant raw outputs
  private double m_gyro_rate_x = 0.0;
  private double m_gyro_rate_y = 0.0;
  private double m_gyro_rate_z = 0.0;
  private double m_accel_x = 0.0;
  private double m_accel_y = 0.0;
  private double m_accel_z = 0.0;

  // Integrated gyro angle
  private double m_integ_angle = 0.0;

  // Complementary filter variables
  private double m_dt = 0.0;
  private double m_alpha = 0.0;
  private double m_tau = 1.0;
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
  private DigitalOutput m_reset_out;
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

  private static class AcquireTask implements Runnable {
    private ADIS16470_IMU imu;

    public AcquireTask(ADIS16470_IMU imu) {
      this.imu = imu;
    }

    @Override
    public void run() {
      imu.acquire();
    }
  }

  public ADIS16470_IMU() {
    this(IMUAxis.kZ, SPI.Port.kOnboardCS0, CalibrationTime._4s);
  }

  /**
   * @param yaw_axis The axis that measures the yaw
   * @param port The SPI Port the gyro is plugged into
   * @param cal_time Calibration time
   */
  public ADIS16470_IMU(IMUAxis yaw_axis, SPI.Port port, CalibrationTime cal_time) {
    m_yaw_axis = yaw_axis;
    m_calibration_time = cal_time.value;
    m_spi_port = port;

    m_acquire_task = new Thread(new AcquireTask(this));

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
      m_reset_out = new DigitalOutput(27); // Drive SPI CS2 (IMU RST) low
      Timer.delay(0.01); // Wait 10ms
      m_reset_out.close();
      m_reset_in = new DigitalInput(27); // Set SPI CS2 (IMU RST) high
      Timer.delay(0.25); // Wait 250ms for reset to complete

      if (!switchToStandardSPI()) {
        return;
      }

      // Set IMU internal decimation to 4 (output data rate of 2000 SPS / (4 + 1) =
      // 400Hz)
      writeRegister(DEC_RATE, 4);

      // Set data ready polarity (HIGH = Good Data), Disable gSense Compensation and
      // PoP
      writeRegister(MSC_CTRL, 1);

      // Configure IMU internal Bartlett filter
      writeRegister(FILT_CTRL, 0);

      // Configure continuous bias calibration time based on user setting
      writeRegister(NULL_CNFG, (m_calibration_time | 0x0700));

      // Notify DS that IMU calibration delay is active
      DriverStation.reportWarning(
          "ADIS16470 IMU Detected. Starting initial calibration delay.", false);

      // Wait for samples to accumulate internal to the IMU (110% of user-defined
      // time)
      try {
        Thread.sleep((long) (Math.pow(2.0, m_calibration_time) / 2000 * 64 * 1.1 * 1000));
      } catch (InterruptedException e) {
      }

      // Write offset calibration command to IMU
      writeRegister(GLOB_CMD, 0x0001);

      // Configure and enable auto SPI
      if (!switchToAutoSPI()) {
        return;
      }

      // Let the user know the IMU was initiallized successfully
      DriverStation.reportWarning("ADIS16470 IMU Successfully Initialized!", false);

      // Drive "Ready" LED low
      m_status_led = new DigitalOutput(28); // Set SPI CS3 (IMU Ready LED) low
    }

    // Report usage and post data to DS
    HAL.report(tResourceType.kResourceType_ADIS16470, 0);
    m_connected = true;
  }

  public boolean isConnected() {
    if (m_simConnected != null) {
      return m_simConnected.get();
    }
    return m_connected;
  }

  /**
   * @param buf
   * @return
   */
  private static int toUShort(ByteBuffer buf) {
    return (buf.getShort(0)) & 0xFFFF;
  }

  /**
   * @param sint
   * @return
   */
  private static long toULong(int sint) {
    return sint & 0x00000000FFFFFFFFL;
  }

  /**
   * @param buf
   * @return
   */
  private static int toShort(int... buf) {
    return (short) (((buf[0] & 0xFF) << 8) + ((buf[1] & 0xFF) << 0));
  }

  /**
   * @param buf
   * @return
   */
  private static int toInt(int... buf) {
    return (buf[0] & 0xFF) << 24 | (buf[1] & 0xFF) << 16 | (buf[2] & 0xFF) << 8 | (buf[3] & 0xFF);
  }

  /**
   * Switch to standard SPI mode.
   *
   * @return
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
        // couple of times
        // to be sure we got it all. Yuck.
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
    // There doesn't seem to be a SPI port active. Let's try to set one up
    if (m_spi == null) {
      System.out.println("Setting up a new SPI port.");
      m_spi = new SPI(m_spi_port);
      m_spi.setClockRate(2000000);
      m_spi.setMode(SPI.Mode.kMode3);
      m_spi.setChipSelectActiveLow();
      readRegister(PROD_ID); // Dummy read

      // Validate the product ID
      if (readRegister(PROD_ID) != 16982) {
        DriverStation.reportError("Could not find ADIS16470", false);
        close();
        return false;
      }
      return true;
    } else {
      // Maybe the SPI port is active, but not in auto SPI mode? Try to read the
      // product ID.
      readRegister(PROD_ID); // dummy read
      if (readRegister(PROD_ID) != 16982) {
        DriverStation.reportError("Could not find an ADIS16470", false);
        close();
        return false;
      } else {
        return true;
      }
    }
  }

  /** @return */
  boolean switchToAutoSPI() {
    // No SPI port has been set up. Go set one up first.
    if (m_spi == null) {
      if (!switchToStandardSPI()) {
        DriverStation.reportError("Failed to start/restart auto SPI", false);
        return false;
      }
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
    switch (m_yaw_axis) {
      case kX:
        m_spi.setAutoTransmitData(m_autospi_x_packet, 2);
        break;
      case kY:
        m_spi.setAutoTransmitData(m_autospi_y_packet, 2);
        break;
      default:
        m_spi.setAutoTransmitData(m_autospi_z_packet, 2);
        break;
    }
    // Configure auto stall time
    m_spi.configureAutoStall(5, 1000, 1);
    // Kick off auto SPI (Note: Device configration impossible after auto SPI is
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
   * Configures calibration time
   *
   * @param new_cal_time New calibration time
   * @return 1 if the new calibration time is the same as the current one else 0
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
    writeRegister(NULL_CNFG, (m_calibration_time | 0x700));
    if (!switchToAutoSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure auto SPI.", false);
      return 2;
    }
    return 0;
  }

  public int configDecRate(int reg) {
    int m_reg = reg;
    if (!switchToStandardSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure standard SPI.", false);
      return 2;
    }
    if (m_reg > 1999) {
      DriverStation.reportError("Attemted to write an invalid deimation value.", false);
      m_reg = 1999;
    }
    m_scaled_sample_rate = (((m_reg + 1.0) / 2000.0) * 1000000.0);
    writeRegister(DEC_RATE, m_reg);
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
    ;
  }

  /**
   * Sets the yaw axis
   *
   * @param yaw_axis The new yaw axis to use
   * @return 1 if the new yaw axis is the same as the current one, 2 if the switch to Standard SPI
   *     failed, else 0.
   */
  public int setYawAxis(IMUAxis yaw_axis) {
    if (m_yaw_axis == yaw_axis) {
      return 1;
    }
    if (!switchToStandardSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure standard SPI.", false);
      return 2;
    }
    m_yaw_axis = yaw_axis;
    if (!switchToAutoSPI()) {
      DriverStation.reportError("Failed to configure/reconfigure auto SPI.", false);
    }
    return 0;
  }

  /**
   * @param reg
   * @return
   */
  private int readRegister(int reg) {
    ByteBuffer buf = ByteBuffer.allocateDirect(2);
    buf.order(ByteOrder.BIG_ENDIAN);
    buf.put(0, (byte) (reg & 0x7f));
    buf.put(1, (byte) 0);

    m_spi.write(buf, 2);
    m_spi.read(false, buf, 2);

    return toUShort(buf);
  }

  /**
   * @param reg
   * @param val
   */
  private void writeRegister(int reg, int val) {
    ByteBuffer buf = ByteBuffer.allocateDirect(2);
    // low byte
    buf.put(0, (byte) ((0x80 | reg)));
    buf.put(1, (byte) (val & 0xff));
    m_spi.write(buf, 2);
    // high byte
    buf.put(0, (byte) (0x81 | reg));
    buf.put(1, (byte) (val >> 8));
    m_spi.write(buf, 2);
  }

  public void reset() {
    synchronized (this) {
      m_integ_angle = 0.0;
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
    System.out.println("Finished cleaning up after the IMU driver.");
  }

  /** */
  private void acquire() {
    // Set data packet length
    final int dataset_len = 19; // 18 data points + timestamp
    final int BUFFER_SIZE = 4000;

    // Set up buffers and variables
    int[] buffer = new int[BUFFER_SIZE];
    int data_count = 0;
    int data_remainder = 0;
    int data_to_read = 0;
    double previous_timestamp = 0.0;
    double delta_angle = 0.0;
    double gyro_rate_x = 0.0;
    double gyro_rate_y = 0.0;
    double gyro_rate_z = 0.0;
    double accel_x = 0.0;
    double accel_y = 0.0;
    double accel_z = 0.0;
    double gyro_rate_x_si = 0.0;
    double gyro_rate_y_si = 0.0;
    double gyro_rate_z_si = 0.0;
    double accel_x_si = 0.0;
    double accel_y_si = 0.0;
    double accel_z_si = 0.0;
    double compAngleX = 0.0;
    double compAngleY = 0.0;
    double accelAngleX = 0.0;
    double accelAngleY = 0.0;

    while (true) {
      // Sleep loop for 10ms
      try {
        Thread.sleep(10);
      } catch (InterruptedException e) {
      }

      if (m_thread_active) {
        m_thread_idle = false;

        data_count =
            m_spi.readAutoReceivedData(
                buffer, 0, 0); // Read number of bytes currently stored in the
        // buffer
        data_remainder =
            data_count % dataset_len; // Check if frame is incomplete. Add 1 because of timestamp
        data_to_read = data_count - data_remainder; // Remove incomplete data from read count
        /* Want to cap the data to read in a single read at the buffer size */
        if (data_to_read > BUFFER_SIZE) {
          DriverStation.reportWarning(
              "ADIS16470 data processing thread overrun has occurred!", false);
          data_to_read = BUFFER_SIZE - (BUFFER_SIZE % dataset_len);
        }
        m_spi.readAutoReceivedData(
            buffer, data_to_read, 0); // Read data from DMA buffer (only complete sets)

        // Could be multiple data sets in the buffer. Handle each one.
        for (int i = 0; i < data_to_read; i += dataset_len) {
          // Timestamp is at buffer[i]
          m_dt = ((double) buffer[i] - previous_timestamp) / 1000000.0;

          /*
           * System.out.println(((toInt(buffer[i + 3], buffer[i + 4], buffer[i + 5],
           * buffer[i + 6]))*delta_angle_sf) / ((10000.0 / (buffer[i] -
           * previous_timestamp)) / 100.0));
           * // DEBUG: Plot Sub-Array Data in Terminal
           * for (int j = 0; j < data_to_read; j++) {
           * System.out.print(buffer[j]);
           * System.out.print(" ,");
           * }
           * System.out.println(" ");
           * //System.out.println(((toInt(buffer[i + 3], buffer[i + 4], buffer[i + 5],
           * buffer[i + 6]))*delta_angle_sf) / ((10000.0 / (buffer[i] -
           * previous_timestamp)) / 100.0) + "," + buffer[3] + "," + buffer[4] + "," +
           * buffer[5] + "," + buffer[6]
           * /*toShort(buffer[7], buffer[8]) + "," +
           * toShort(buffer[9], buffer[10]) + "," +
           * toShort(buffer[11], buffer[12]) + "," +
           * toShort(buffer[13], buffer[14]) + "," +
           * toShort(buffer[15], buffer[16]) + ","
           * + toShort(buffer[17], buffer[18]));
           */

          /*
           * Get delta angle value for selected yaw axis and scale by the elapsed time
           * (based on timestamp)
           */
          delta_angle =
              (toInt(buffer[i + 3], buffer[i + 4], buffer[i + 5], buffer[i + 6]) * delta_angle_sf)
                  / (m_scaled_sample_rate / (buffer[i] - previous_timestamp));
          gyro_rate_x = (toShort(buffer[i + 7], buffer[i + 8]) / 10.0);
          gyro_rate_y = (toShort(buffer[i + 9], buffer[i + 10]) / 10.0);
          gyro_rate_z = (toShort(buffer[i + 11], buffer[i + 12]) / 10.0);
          accel_x = (toShort(buffer[i + 13], buffer[i + 14]) / 800.0);
          accel_y = (toShort(buffer[i + 15], buffer[i + 16]) / 800.0);
          accel_z = (toShort(buffer[i + 17], buffer[i + 18]) / 800.0);

          // Convert scaled sensor data to SI units (for tilt calculations)
          // TODO: Should the unit outputs be selectable?
          gyro_rate_x_si = gyro_rate_x * deg_to_rad;
          gyro_rate_y_si = gyro_rate_y * deg_to_rad;
          gyro_rate_z_si = gyro_rate_z * deg_to_rad;
          accel_x_si = accel_x * grav;
          accel_y_si = accel_y * grav;
          accel_z_si = accel_z * grav;

          // Store timestamp for next iteration
          previous_timestamp = buffer[i];

          m_alpha = m_tau / (m_tau + m_dt);

          if (m_first_run) {
            // Set up inclinometer calculations for first run
            accelAngleX =
                Math.atan2(
                    accel_x_si, Math.sqrt((accel_y_si * accel_y_si) + (accel_z_si * accel_z_si)));
            accelAngleY =
                Math.atan2(
                    accel_y_si, Math.sqrt((accel_x_si * accel_x_si) + (accel_z_si * accel_z_si)));
            compAngleX = accelAngleX;
            compAngleY = accelAngleY;
          } else {
            // Run inclinometer calculations
            accelAngleX =
                Math.atan2(
                    accel_x_si, Math.sqrt((accel_y_si * accel_y_si) + (accel_z_si * accel_z_si)));
            accelAngleY =
                Math.atan2(
                    accel_y_si, Math.sqrt((accel_x_si * accel_x_si) + (accel_z_si * accel_z_si)));
            accelAngleX = formatAccelRange(accelAngleX, accel_z_si);
            accelAngleY = formatAccelRange(accelAngleY, accel_z_si);
            compAngleX = compFilterProcess(compAngleX, accelAngleX, -gyro_rate_y_si);
            compAngleY = compFilterProcess(compAngleY, accelAngleY, gyro_rate_x_si);
          }

          synchronized (this) {
            /* Push data to global variables */
            if (m_first_run) {
              /*
               * Don't accumulate first run. previous_timestamp will be "very" old and the
               * integration will end up way off
               */
              m_integ_angle = 0.0;
            } else {
              m_integ_angle += delta_angle;
            }
            m_gyro_rate_x = gyro_rate_x;
            m_gyro_rate_y = gyro_rate_y;
            m_gyro_rate_z = gyro_rate_z;
            m_accel_x = accel_x;
            m_accel_y = accel_y;
            m_accel_z = accel_z;
            m_compAngleX = compAngleX * rad_to_deg;
            m_compAngleY = compAngleY * rad_to_deg;
            m_accelAngleX = accelAngleX * rad_to_deg;
            m_accelAngleY = accelAngleY * rad_to_deg;
          }
          m_first_run = false;
        }
      } else {
        m_thread_idle = true;
        data_count = 0;
        data_remainder = 0;
        data_to_read = 0;
        previous_timestamp = 0.0;
        delta_angle = 0.0;
        gyro_rate_x = 0.0;
        gyro_rate_y = 0.0;
        gyro_rate_z = 0.0;
        accel_x = 0.0;
        accel_y = 0.0;
        accel_z = 0.0;
        gyro_rate_x_si = 0.0;
        gyro_rate_y_si = 0.0;
        gyro_rate_z_si = 0.0;
        accel_x_si = 0.0;
        accel_y_si = 0.0;
        accel_z_si = 0.0;
        compAngleX = 0.0;
        compAngleY = 0.0;
        accelAngleX = 0.0;
        accelAngleY = 0.0;
      }
    }
  }

  /**
   * @param compAngle
   * @param accAngle
   * @return
   */
  private double formatFastConverge(double compAngle, double accAngle) {
    if (compAngle > accAngle + Math.PI) {
      compAngle = compAngle - 2.0 * Math.PI;
    } else if (accAngle > compAngle + Math.PI) {
      compAngle = compAngle + 2.0 * Math.PI;
    }
    return compAngle;
  }

  /**
   * @param compAngle
   * @return
   */
  private double formatRange0to2PI(double compAngle) {
    while (compAngle >= 2 * Math.PI) {
      compAngle = compAngle - 2.0 * Math.PI;
    }
    while (compAngle < 0.0) {
      compAngle = compAngle + 2.0 * Math.PI;
    }
    return compAngle;
  }

  /**
   * @param accelAngle
   * @param accelZ
   * @return
   */
  private double formatAccelRange(double accelAngle, double accelZ) {
    if (accelZ < 0.0) {
      accelAngle = Math.PI - accelAngle;
    } else if (accelZ > 0.0 && accelAngle < 0.0) {
      accelAngle = 2.0 * Math.PI + accelAngle;
    }
    return accelAngle;
  }

  /**
   * @param compAngle
   * @param accelAngle
   * @param omega
   * @return
   */
  private double compFilterProcess(double compAngle, double accelAngle, double omega) {
    compAngle = formatFastConverge(compAngle, accelAngle);
    compAngle = m_alpha * (compAngle + omega * m_dt) + (1.0 - m_alpha) * accelAngle;
    compAngle = formatRange0to2PI(compAngle);
    if (compAngle > Math.PI) {
      compAngle = compAngle - 2.0 * Math.PI;
    }
    return compAngle;
  }

  /** @return Yaw axis angle in degrees (CCW positive) */
  public synchronized double getAngle() {
    switch (m_yaw_axis) {
      case kX:
        if (m_simGyroAngleX != null) {
          return m_simGyroAngleX.get();
        }
        break;
      case kY:
        if (m_simGyroAngleY != null) {
          return m_simGyroAngleY.get();
        }
        break;
      case kZ:
        if (m_simGyroAngleZ != null) {
          return m_simGyroAngleZ.get();
        }
        break;
    }
    return m_integ_angle;
  }

  /** @return Yaw axis angular rate in degrees per second (CCW positive) */
  public synchronized double getRate() {
    if (m_yaw_axis == IMUAxis.kX) {
      if (m_simGyroRateX != null) {
        return m_simGyroRateX.get();
      }
      return m_gyro_rate_x;
    } else if (m_yaw_axis == IMUAxis.kY) {
      if (m_simGyroRateY != null) {
        return m_simGyroRateY.get();
      }
      return m_gyro_rate_y;
    } else if (m_yaw_axis == IMUAxis.kZ) {
      if (m_simGyroRateZ != null) {
        return m_simGyroRateZ.get();
      }
      return m_gyro_rate_z;
    } else {
      return 0.0;
    }
  }

  /** @return Yaw Axis */
  public IMUAxis getYawAxis() {
    return m_yaw_axis;
  }

  /** @return current acceleration in the X axis */
  public synchronized double getAccelX() {
    return m_accel_x * 9.81;
  }

  /** @return current acceleration in the Y axis */
  public synchronized double getAccelY() {
    return m_accel_y * 9.81;
  }

  /** @return current acceleration in the Z axis */
  public synchronized double getAccelZ() {
    return m_accel_z * 9.81;
  }

  /** @return X axis complementary angle */
  public synchronized double getXComplementaryAngle() {
    return m_compAngleX;
  }

  /** @return Y axis complementary angle */
  public synchronized double getYComplementaryAngle() {
    return m_compAngleY;
  }

  /** @return X axis filtered acceleration angle */
  public synchronized double getXFilteredAccelAngle() {
    return m_accelAngleX;
  }

  /** @return Y axis filtered acceleration angle */
  public synchronized double getYFilteredAccelAngle() {
    return m_accelAngleY;
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
  public void initSendable(NTSendableBuilder builder) {
    builder.setSmartDashboardType("Gyro");
    builder.addDoubleProperty("Value", this::getAngle, null);
  }
}
