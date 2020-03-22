package edu.wpi.first.wpiutil.datalog;

/**
 * <b>DATA STORAGE FORMAT</b>
 *
 * <p><b>**Timestamp File**</b>
 *
 * <p>Timestamp file (named whatever the user provides as filename) consists of:
 *
 * <p>- 4KiB header
 *
 * <p>- 0 or more fixed-size records
 *
 * <p>The timestamp file header contains 0-padded JSON data containing at least
 * the following fields:
 *
 * <pre><code>{{@literal
 *  "dataLayout": <string>,
 *  "dataType": <string>,
 *  "dataWritePos": <integer>,
 *  "fixedSize": <boolean>,
 *  "gapData": <string>,
 *  "recordSize": <integer>,
 *  "timeWritePos": <integer>
 * }}</code></pre>
 *
 * <p>{@code dataLayout} : user-defined string that describes the detailed layout
 * of the data.
 *
 * <p>{@code dataType} : user-defined string, typically used to make sure there's not
 * a data type conflict when reading the file, or knowing what type of data
 * is stored when opening an arbitrary file.  Suggestions: make this java-style
 * (com.foo.bar) or MIME type.
 *
 * <p>{@code dataWritePos} : next byte write position in the data file
 *
 * <p>{@code fixedSize} : true if each record is fixed size (in which case there will not
 * be a data file), false if the records are variable size
 *
 * <p>{@code gapData} : user-defined string that contains the data that should be written
 * between each record's data in the data file.  Unused if fixedSize is true.
 *
 * <p>{@code recordSize} : the size of each record (including timestamp) in the timestamp
 * file, in bytes
 *
 * <p>{@code timeWritePos} : next byte write position in the timestamp file
 *
 * <p><b> **Timestamp File Records** </b>
 *
 * <p>Each record in the timestamp file starts with a 64-bit timestamp.  The
 * epoch and resolution of the timestamp is unspecified, but most files
 * use microsecond resolution. The timestamps must be monotonically
 * increasing for the find function to work.
 *
 * <p>If {@code fixedSize=true}, the rest of the record contains the user data.
 *
 * <p>If {@code fixedSize=false}, the rest of the record contains the offset and size
 * (in that order) of the data contents in the data file.  The offset
 * and size can either be 32-bit or 64-bit (as determined by recordSize, so
 * {@code recordSize=16} if 32-bit offset+size, {@code recordSize=24} if 64-bit offset+size).
 *
 * <p><b> **Data File** </b>
 *
 * <p>Used only for variable-sized data ({@code fixedSize=false}).  File is named with
 * {@code .data} suffix to whatever the user provided as a filename.
 *
 * <p>Contains continuous data contents, potentially with gaps between each
 * record (as configured by gapData).
 */
@SuppressWarnings({"PMD.TooManyMethods", "PMD.ExcessivePublicCount"})
public class DataLog {
  protected final long m_impl;

  //explicitly package-private so inheritors can call it
  DataLog(long impl) {
    m_impl = impl;
  }

  /**
   * Opens a log file.  Returns log object.
   *
   * @param filename Filename to open
   * @param disp     Creation disposition (e.g. open or create)
   * @param config   Implementation configuration
   * @return Log object
   */
  public static DataLog open(String filename, String dataType, String dataLayout, int recordSize,
                             CreationDisposition disp, DataLogConfig config) {
    return new DataLog(DataLogJNI.open(filename, dataType, dataLayout,
            recordSize, disp.m_id, config));
  }

  /**
   * Opens a log file.  Returns log object.
   *
   * @param filename Filename to open
   * @param config   Implementation configuration
   * @return Log object
   */
  public static DataLog openRaw(String filename, DataLogConfig config) {
    return new DataLog(DataLogJNI.openRaw(filename, config));
  }

  /**
   * Opens a log file.  Returns log object.
   *
   * @param filename Filename to open
   * @param disp     Creation disposition (e.g. open or create)
   * @param config   Implementation configuration
   * @return Log object
   */
  public static DataLog openBoolean(String filename, CreationDisposition disp,
                                    DataLogConfig config) {
    return new BooleanLog(DataLogJNI.openBoolean(filename, disp.m_id, config));
  }

  /**
   * Opens a log file.  Returns log object.
   *
   * @param filename Filename to open
   * @param disp     Creation disposition (e.g. open or create)
   * @param config   Implementation configuration
   * @return Log object
   */
  public static DataLog openDouble(String filename, CreationDisposition disp,
                                   DataLogConfig config) {
    return new DoubleLog(DataLogJNI.openDouble(filename, disp.m_id, config));
  }

  /**
   * Opens a log file.  Returns log object.
   *
   * @param filename Filename to open
   * @param disp     Creation disposition (e.g. open or create)
   * @param config   Implementation configuration
   * @return Log object
   */
  public static DataLog openString(String filename, CreationDisposition disp,
                                   DataLogConfig config) {
    return new StringLog(DataLogJNI.openString(filename, disp.m_id, config));
  }

  /**
   * Opens a log file.  Returns log object, or error if file does not exist.
   *
   * @param filename Filename to open
   * @param disp     Creation disposition (e.g. open or create)
   * @param config   Implementation configuration
   * @return Log object (or error)
   */
  public static DataLog openBooleanArray(String filename, CreationDisposition disp,
                                         DataLogConfig config) {
    return new BooleanArrayLog(DataLogJNI.openBooleanArray(filename, disp.m_id, config));
  }

  /**
   * Opens a log file.  Returns log object.
   *
   * @param filename Filename to open
   * @param disp     Creation disposition (e.g. open or create)
   * @param config   Implementation configuration
   * @return Log object
   */
  public static DataLog openDoubleArray(String filename, CreationDisposition disp,
                                        DataLogConfig config) {
    return new DoubleArrayLog(DataLogJNI.openDoubleArray(filename, disp.m_id, config));
  }

  /**
   * Opens a log file.  Returns log object.
   *
   * @param filename Filename to open
   * @param disp     Creation disposition (e.g. open or create)
   * @param config   Implementation configuration
   * @return Log object
   */
  public static DataLog openStringArray(String filename, CreationDisposition disp,
                                        DataLogConfig config) {
    return new StringArrayLog(DataLogJNI.openStringArray(filename, disp.m_id, config));
  }

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.  If it is not, and the configuration
   * has checkMonotonic true, this function will return false.
   *
   * @param data Data to record
   * @return True if successful, false on failure
   */
  public boolean appendRaw(byte[] data) {
    return DataLogJNI.appendRaw(m_impl, data);
  }

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.  If it is not, and the configuration
   * has checkMonotonic true, this function will return false.
   *
   * @param timestamp Time stamp
   * @param data      Data to record
   * @return True if successful, false on failure
   */
  public boolean appendRawTime(long timestamp, byte[] data) {
    return DataLogJNI.appendRawTime(m_impl, timestamp, data);
  }

  /**
   * Flushes the log data to disk.
   */
  public void flush() {
    DataLogJNI.flush(m_impl);
  }

  public String getDataType() {
    return DataLogJNI.getDataType(m_impl);
  }

  public String getDataLayout() {
    return DataLogJNI.getDataLayout(m_impl);
  }

  public int getRecordSize() {
    return DataLogJNI.getRecordSize(m_impl);
  }

  public boolean isFixedSize() {
    return DataLogJNI.isFixedSize(m_impl);
  }

  public int getSize() {
    return DataLogJNI.getSize(m_impl);
  }

  /**
   * Finds the first record with timestamp greater than or equal to the
   * provided time.
   *
   * @param timestamp Time stamp
   * @param first     index of first record
   * @return Index of record
   */
  public int find(long timestamp, int first) {
    return DataLogJNI.find(m_impl, timestamp, first);
  }

  /**
   * Finds the first record with timestamp greater than or equal to the
   * provided time.
   *
   * @param timestamp Time stamp
   * @param first     index of first record
   * @param last      index of last record
   * @return Index of record
   */
  public int find(long timestamp, int first, int last) {
    return DataLogJNI.find(m_impl, timestamp, first, last);
  }

  /**
   * Checks the currently opened log file header.
   *
   * @param dataType    Data type
   * @param recordSize  Record size; 0 indicates variable data size
   * @param checkType   Check data type matches
   * @param checkLayout Check data layout matches
   * @param checkSize   Check size matches
   * @return True if successful
   */
  public boolean check(String dataType, int recordSize, boolean checkType, boolean checkLayout,
                       boolean checkSize) {
    return DataLogJNI.check(m_impl, dataType, recordSize, checkType, checkLayout, checkSize);

  }

  public void close() {
    DataLogJNI.close(m_impl);
  }

  /**
   * Reads the raw stored data.
   *
   * @param index Log index
   * @return Pair of timestamp and reference to raw stored data
   */
  public DataLogEntry readRaw(int index) {
    return new DataLogEntry(0L, DataLogJNI.readRaw(m_impl, index));
  }


  public static final class DataLogEntry {
    private long m_timestamp;
    private byte[] m_data;

    private DataLogEntry(long timestamp, byte[] data) {
      m_timestamp = timestamp;
      setData(data);
    }

    public long getTimestamp() {
      return m_timestamp;
    }

    public void setTimestamp(long timestamp) {
      m_timestamp = timestamp;
    }

    public byte[] getData() {
      return m_data.clone();
    }

    public void setData(byte[] data) {
      m_data = data.clone();
    }

  }

  /**
   * Creation disposition.
   */
  public enum CreationDisposition {
    /// CD_CreateAlways - When opening a file:
    ///   * If it already exists, truncate it.
    ///   * If it does not already exist, create a new file.
    CreateAlways(0),

    /// CD_CreateNew - When opening a file:
    ///   * If it already exists, fail.
    ///   * If it does not already exist, create a new file.
    CreateNew(1),

    /// CD_OpenExisting - When opening a file:
    ///   * If it already exists, open the file.
    ///   * If it does not already exist, fail.
    OpenExisting(2),

    /// CD_OpenAlways - When opening a file:
    ///   * If it already exists, open the file.
    ///   * If it does not already exist, create a new file.
    OpenAlways(3);

    final int m_id;

    CreationDisposition(int id) {
      m_id = id;
    }
  }

  public static final class DataLogConfig {
    /**
     * Start out timestamp file with space for this many records.  Note the
     * actual file size will start out this big, but it's a sparse file.
     */
    public int m_initialSize = 1000;

    /**
     * Once size has reached this size, grow by this number of records each
     * time.  Prior to it reaching this size, the space is doubled.
     */
    public int m_maxGrowSize = 60000;

    /**
     * Maximum map window size.  Larger is more efficient, but may have
     * issues on 32-bit systems.  Defaults to unlimited.
     */
    public int m_maxMapSize;

    /**
     * Periodic flush setting.  Flushes log to disk every N appends.
     * Defaults to no periodic flush.
     */
    public int m_periodicFlush;

    /**
     * Start out data file with space for this many bytes.  Note the
     * actual file size will start out this big, but it's a sparse file.
     */
    public long m_initialDataSize = 100000;

    /**
     * Once data file has reached this size, grow by this number of bytes each
     * time.  Prior to it reaching this size, the space is doubled.
     */
    public long m_maxDataGrowSize = 1024 * 1024;

    /**
     * Use large (e.g. 64-bit) variable-sized data files when creating a new
     * log.  The default is to use 32-bit sizes for the variable-sized data.
     */
    public boolean m_largeData;

    /**
     * Fill data to put in between each record of variable-sized data in data
     * file.  Useful for e.g. making strings null terminated.  Defaults to
     * nothing.
     */
    public String m_gapData;

    /**
     * Check data type when opening existing file.  Defaults to checking.
     */
    public boolean m_checkType = true;

    /**
     * Check record size when opening existing file.  Defaults to checking.
     */
    public boolean m_checkSize = true;

    /**
     * Check data layout when opening existing file.  Defaults to checking.
     */
    public boolean m_checkLayout = true;

    /**
     * Check timestamp is monotonically increasing and don't save the value if
     * timestamp decreased.  Defaults to true.
     */
    public boolean m_checkMonotonic = true;

    /**
     * Open file in read-only mode.
     */
    public boolean m_readOnly;
  }
}
