package edu.wpi.first.wpiutil.datalog;

public class BooleanArrayLog extends DataLog {
  //explicitly package-private, object should be created via factory method
  BooleanArrayLog(long impl) {
    super(impl);
  }

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.  If it is not, and the configuration
   * has checkMonotonic true, this function will return false.
   *
   * @param value Data to record
   * @return True if successful, false on failure
   */
  public boolean appendBooleanArray(boolean[] value) {
    return DataLogJNI.appendBooleanArray(m_impl, value);
  }

  /**
   * Appends a record to the log.  For find functions to work, timestamp
   * must be monotonically increasing.  If it is not, and the configuration
   * has checkMonotonic true, this function will return false.
   *
   * @param timestamp Time stamp
   * @param value     Data to record
   * @return True if successful, false on failure
   */
  public boolean appendBooleanArrayTime(long timestamp, boolean[] value) {
    return DataLogJNI.appendBooleanArrayTime(m_impl, timestamp, value);
  }

}
