package edu.wpi.first.wpiutil.datalog;

public class DoubleLog extends DataLog {
  //explicitly package-private, object should be created via factory method
  DoubleLog(long impl) {
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
  public boolean appendDouble(double value) {
    return DataLogJNI.appendDouble(m_impl, value);
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
  public boolean appendDoubleTime(long timestamp, double value) {
    return DataLogJNI.appendDoubleTime(m_impl, timestamp, value);
  }
}
