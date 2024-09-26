package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.PowerDistributionFaults;
import edu.wpi.first.hal.PowerDistributionJNI;
import edu.wpi.first.hal.PowerDistributionStickyFaults;
import edu.wpi.first.hal.PowerDistributionVersion;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StructPublisher;
import edu.wpi.first.util.datalog.StructLogEntry;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.nio.ByteBuffer;
import java.util.function.Consumer;

/**
 * A class for logging PowerDistribution data to NetworkTables or DataLog.
 *
 * <p>This class is designed to be instantiated in the `Robot.java` and have the {@link #log()}
 * method called in the `robotPeriodic` method.
 *
 * <p>The logger supports rate limiting the logging to prevent needless JNI calls when no new data
 * is available. This is done via {@link #setLogWindup(int)}.
 */
public class PowerDistributionLogger {
  private final int m_handle;
  private final Consumer<PowerDistributionStats> m_output;
  private final PowerDistributionStats m_stats = new PowerDistributionStats();

  /**
   * This value determines how many calls to {@link #log()} are required before the data is logged.
   */
  private int m_logWindup;

  /** This value tracks the number of times {@link #log()} has been called. */
  private int m_logCount;

  /**
   * Constructs a PowerDistributionLogger, this provides a performant way to log PowerDistribution
   * data to NetworkTables or only to a DataLog.
   *
   * @param pd The PowerDistribution object to log
   * @param logPath The path to log the data to
   * @param datalogOnly If true, only log to the datalog
   */
  public PowerDistributionLogger(PowerDistribution pd, String logPath, boolean datalogOnly) {
    m_handle = pd.getHandle();
    String statsLogPath = NetworkTable.normalizeKey(logPath + "/Stats", true);
    String metaLogPath = NetworkTable.normalizeKey(logPath + "/Meta", true);
    PowerDistributionMeta meta = new PowerDistributionMeta(m_handle);
    if (datalogOnly) {
      StructLogEntry<PowerDistributionStats> entry =
          StructLogEntry.create(
              DataLogManager.getLog(), statsLogPath, new PowerDistributionStatsStruct());
      m_output = entry::append;
      StructLogEntry.create(DataLogManager.getLog(), metaLogPath, new PowerDistributionMetaStruct())
          .append(meta);
    } else {
      StructPublisher<PowerDistributionStats> entry =
          NetworkTableInstance.getDefault()
              .getStructTopic(statsLogPath, new PowerDistributionStatsStruct())
              .publish();
      m_output = entry;
      NetworkTableInstance.getDefault()
          .getStructTopic(metaLogPath, new PowerDistributionMetaStruct())
          .publish()
          .set(meta);
    }
  }

  /**
   * Logs the PowerDistribution data to NetworkTables or DataLog.
   *
   * <p>If {@link #setLogWindup(int)} has been called, this method will only log every N calls to
   * {@link #log()}.
   */
  public void log() {
    m_logCount++;
    if (m_logWindup == 0 || m_logCount % m_logWindup == 0) {
      m_stats.update(m_handle);
      m_output.accept(m_stats);
    }
  }

  /**
   * Sets the log windup value, this value determines how many calls to {@link #log()} are required
   * before the data is logged. This is available to prevent needless JNI calls when no new data is
   * available.
   *
   * <p>For example, if this value is set to 5, the data will only be logged every 5 calls to {@link
   * #log()}.
   *
   * <p>The {@link PowerDistribution} refreshes it's data every 80ms.
   *
   * @param windup The number of calls to {@link #log()} required before the data is logged
   */
  public void setLogWindup(int windup) {
    m_logWindup = windup;
  }

  @SuppressWarnings("PMD.RedundantFieldInitializer")
  private static final class PowerDistributionStats implements StructSerializable {

    private int m_faults = 0;
    private int m_stickyFaults = 0;
    private double m_voltage = 0.0;
    private double m_totalCurrent = 0.0;
    private boolean m_switchableChannel = false;
    private double m_temperature = 0.0;
    private double[] m_currents = new double[24];

    private PowerDistributionStats() {}

    private PowerDistributionStats(
        int faults,
        int stickyFaults,
        double voltage,
        double totalCurrent,
        boolean switchableChannel,
        double temperature,
        double[] currents) {
      m_faults = faults;
      m_stickyFaults = stickyFaults;
      m_voltage = voltage;
      m_totalCurrent = totalCurrent;
      m_switchableChannel = switchableChannel;
      m_temperature = temperature;
      m_currents = currents;
    }

    private void update(int handle) {
      m_faults = PowerDistributionJNI.getFaultsNative(handle);
      m_stickyFaults = PowerDistributionJNI.getStickyFaultsNative(handle);
      m_voltage = PowerDistributionJNI.getVoltage(handle);
      m_totalCurrent = PowerDistributionJNI.getTotalCurrent(handle);
      m_switchableChannel = PowerDistributionJNI.getSwitchableChannel(handle);
      m_temperature = PowerDistributionJNI.getTemperature(handle);
      PowerDistributionJNI.getAllCurrents(handle, m_currents);
    }
  }

  private static final class PowerDistributionStatsStruct
      implements Struct<PowerDistributionStats> {
    private static final int kSize = 4 + 4 + 8 + 8 + 1 + 8 + (8 * 24);

    @Override
    public Class<PowerDistributionStats> getTypeClass() {
      return PowerDistributionStats.class;
    }

    @Override
    public int getSize() {
      return kSize;
    }

    @Override
    public String getSchema() {
      return "PowerDistributionFaults faults; "
          + "PowerDistributionStickyFaults stickyFaults; "
          + "double voltage; "
          + "double totalCurrent; "
          + "bool switchableChannel; "
          + "double temperature;"
          + "double currents[24];";
    }

    @Override
    public String getTypeName() {
      return "PowerDistributionStats";
    }

    @Override
    public void pack(ByteBuffer bb, PowerDistributionStats value) {
      bb.putInt(value.m_faults);
      bb.putInt(value.m_stickyFaults);
      bb.putDouble(value.m_voltage);
      bb.putDouble(value.m_totalCurrent);
      bb.put((byte) (value.m_switchableChannel ? 1 : 0));
      bb.putDouble(value.m_temperature);
      for (double current : value.m_currents) {
        bb.putDouble(current);
      }
    }

    @Override
    public PowerDistributionStats unpack(ByteBuffer bb) {
      PowerDistributionStats ret =
          new PowerDistributionStats(
              bb.getInt(),
              bb.getInt(),
              bb.getDouble(),
              bb.getDouble(),
              bb.get() != 0,
              bb.getDouble(),
              new double[24]);
      for (int i = 0; i < 24; i++) {
        ret.m_currents[i] = bb.getDouble();
      }
      return ret;
    }

    @Override
    public Struct<?>[] getNested() {
      return new Struct<?>[] {
        new PowerDistributionFaults.PowerDistributionFaultsStruct(),
        new PowerDistributionStickyFaults.PowerDistributionStickyFaultsStruct()
      };
    }
  }

  private static final class PowerDistributionMeta implements StructSerializable {

    private final int m_canId;
    private final int m_type;
    private final int m_channelCount;
    private final PowerDistributionVersion m_version;

    private PowerDistributionMeta(int handle) {
      m_canId = PowerDistributionJNI.getModuleNumber(handle);
      m_type = PowerDistributionJNI.getType(handle);
      m_channelCount = PowerDistributionJNI.getNumChannels(handle);
      m_version = PowerDistributionJNI.getVersion(handle);
    }

    private PowerDistributionMeta(
        int canId, int type, int channelCount, PowerDistributionVersion version) {
      m_canId = canId;
      m_type = type;
      m_channelCount = channelCount;
      m_version = version;
    }
  }

  private static final class PowerDistributionMetaStruct implements Struct<PowerDistributionMeta> {
    private static final int kSize = 4 + 4 + 4 + PowerDistributionVersion.struct.getSize();

    @Override
    public Class<PowerDistributionMeta> getTypeClass() {
      return PowerDistributionMeta.class;
    }

    @Override
    public int getSize() {
      return kSize;
    }

    @Override
    public String getSchema() {
      return "uint8 canId; "
          + "enum{REV_PDH=1,CTRE_PDP=2} uint8 type; "
          + "uint8 channelCount; "
          + "PowerDistributionVersion version;";
    }

    @Override
    public String getTypeName() {
      return "PowerDistributionMeta";
    }

    @Override
    public void pack(ByteBuffer bb, PowerDistributionMeta value) {
      bb.put((byte) value.m_canId);
      bb.put((byte) value.m_type);
      bb.put((byte) value.m_channelCount);
      PowerDistributionVersion.struct.pack(bb, value.m_version);
    }

    @Override
    public PowerDistributionMeta unpack(ByteBuffer bb) {
      return new PowerDistributionMeta(
          (int) bb.get(),
          (int) bb.get(),
          (int) bb.get(),
          PowerDistributionVersion.struct.unpack(bb));
    }

    @Override
    public Struct<?>[] getNested() {
      return new Struct<?>[] {new PowerDistributionVersion.PowerDistributionVersionStruct()};
    }
  }
}
