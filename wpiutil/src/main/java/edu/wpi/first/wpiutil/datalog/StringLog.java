package edu.wpi.first.wpiutil.datalog;

public class StringLog extends DataLog {
  //explicitly package-private, object should be created via factory method
  StringLog(long impl) {
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
  public boolean appendString(String value) {
    return DataLogJNI.appendString(m_impl, value);
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
  public boolean appendStringTime(long timestamp, String value) {
    return DataLogJNI.appendStringTime(m_impl, timestamp, value);
  }
}
