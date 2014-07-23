/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.util.Stack;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.parsing.IInputOutput;

/**
 * Pack data into the "user data" field that gets sent to the dashboard laptop
 * via the driver station.
 */
public class Dashboard  implements IDashboard, IInputOutput {

    protected class MemAccess {

        byte[] m_bytes;

        protected MemAccess(byte[] bytes) {
            m_bytes = bytes;
        }

        protected MemAccess(int length) {
            m_bytes = new byte[length];
        }

        public void setByte(int index, byte value) {
            m_bytes[index] = value;
        }

        public void setShort(int index, short value) {
            setByte(index++, (byte) (value >>> 8));
            setByte(index++, (byte) (value));
        }

        public void setInt(int index, int value) {
            setByte(index++, (byte) (value >>> 24));
            setByte(index++, (byte) (value >>> 16));
            setByte(index++, (byte) (value >>> 8));
            setByte(index++, (byte) (value));
        }

        public void setFloat(int index, float value) {
            setInt(index, Float.floatToIntBits(value));
        }

        public void setDouble(int index, double value) {
            setInt(index, (int) (Double.doubleToLongBits(value) >>> 32));
            setInt(index + 4, (int) Double.doubleToLongBits(value));
        }

        public void setString(int index, String value) {
            setBytes(index, value.getBytes(), 0, value.length());
        }

        public void setBytes(int index, byte[] value, int offset, int number) {
            for (int i = 0; i < number; i++) {
                m_bytes[i + index] = value[i + offset];
            }
        }
    }
    private static final String kArray = "Array";
    private static final String kCluster = "Cluster";
    private static final Integer kByte = new Integer(0);
    private static final Integer kShort = new Integer(1);
    private static final Integer kInt = new Integer(2);
    private static final Integer kFloat = new Integer(3);
    private static final Integer kDouble = new Integer(4);
    private static final Integer kString = new Integer(5);
    private static final Integer kOther = new Integer(6);
    private static final Integer kBoolean = new Integer(7);
    private static final int kMaxDashboardDataSize = DriverStation.USER_STATUS_DATA_SIZE -
            4 * 3 - 1; // 13 bytes needed for 3 size parameters and the sequence number
    private static boolean m_reported = false;
    protected MemAccess m_userStatus;
    protected int m_userStatusSize = 0;
    private MemAccess m_localBuffer;
    private int m_packPtr;
    private Stack m_expectedArrayElementType = new Stack();
    private Stack m_arrayElementCount = new Stack();
    private Stack m_arraySizePtr = new Stack();
    private Stack m_complexTypeStack = new Stack();
    private final Object m_statusDataSemaphore;

    /**
     * Dashboard constructor.
     *
     * This is only called once when the DriverStation constructor is called.
     * @param statusDataSemaphore the object to synchronize on
     */
    protected Dashboard(Object statusDataSemaphore) {
        m_userStatus = new MemAccess(kMaxDashboardDataSize);
        m_localBuffer = new MemAccess(kMaxDashboardDataSize);
        m_packPtr = 0;
        m_statusDataSemaphore = statusDataSemaphore;
    }

    /**
     * Pack a signed 8-bit int into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @return True on success
     */
    public boolean addByte(byte value) {
        if (!validateAdd(1)) {
            return false;
        }
        m_localBuffer.setByte(m_packPtr, value);
        m_packPtr += 1;
        return addedElement(kByte);
    }

    /**
     * Pack a signed 16-bit int into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @return True on success
     */
    public boolean addShort(short value) {
        if (!validateAdd(2)) {
            return false;
        }
        m_localBuffer.setShort(m_packPtr, value);
        m_packPtr += 2;
        return addedElement(kShort);
    }

    /**
     * Pack a signed 32-bit int into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @return True on success
     */
    public boolean addInt(int value) {
        if (!validateAdd(4)) {
            return false;
        }
        m_localBuffer.setInt(m_packPtr, value);
        m_packPtr += 4;
        return addedElement(kInt);
    }

    /**
     * Pack a 32-bit floating point number into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @return True on success
     */
    public boolean addFloat(float value) {
        if (!validateAdd(4)) {
            return false;
        }
        m_localBuffer.setFloat(m_packPtr, value);
        m_packPtr += 4;
        return addedElement(kFloat);
    }

    /**
     * Pack a 64-bit floating point number into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @return True on success
     */
    public boolean addDouble(double value) {
        if (!validateAdd(8)) {
            return false;
        }
        m_localBuffer.setDouble(m_packPtr, value);
        m_packPtr += 8;
        return addedElement(kDouble);
    }

    /**
     * Pack a boolean into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @return True on success
     */
    public boolean addBoolean(boolean value) {
        if (!validateAdd(1)) {
            return false;
        }
        m_localBuffer.setByte(m_packPtr, (byte) (value ? 1 : 0));
        m_packPtr += 1;
        return addedElement(kBoolean);
    }

    /**
     * Pack a NULL-terminated string of 8-bit characters into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @return True on success
     */
    public boolean addString(String value) {
        if (!validateAdd(value.length() + 4)) {
            return false;
        }
        m_localBuffer.setInt(m_packPtr, value.length());
        m_packPtr += 4;
        m_localBuffer.setString(m_packPtr, value);
        m_packPtr += value.length();
        return addedElement(kString);
    }

    /**
     * Pack a string of 8-bit characters of specified length into the dashboard data structure.
     * @param value Data to be packed into the structure.
     * @param length The number of bytes in the string to pack.
     * @return True on success
     */
    public boolean addString(String value, int length) {
        return addString(value.substring(0, length));
    }

    /**
     * Start an array in the packed dashboard data structure.
     *
     * After calling addArray(), call the appropriate Add method for each element of the array.
     * Make sure you call the same add each time.  An array must contain elements of the same type.
     * You can use clusters inside of arrays to make each element of the array contain a structure of values.
     * You can also nest arrays inside of other arrays.
     * Every call to addArray() must have a matching call to finalizeArray().
     * @return True on success
     */
    public boolean addArray() {
        if (!validateAdd(4)) {
            return false;
        }
        m_complexTypeStack.push(kArray);
        m_arrayElementCount.push(new Integer(0));
        m_arraySizePtr.push(new Integer(m_packPtr));
        m_packPtr += 4;
        return true;
    }

    /**
     * Indicate the end of an array packed into the dashboard data structure.
     *
     * After packing data into the array, call finalizeArray().
     * Every call to addArray() must have a matching call to finalizeArray().
     * @return True on success
     */
    public boolean finalizeArray() {
        if (m_complexTypeStack.peek() != kArray) {
            System.err.println("Attempted to finalize an array in the middle of a cluster or without starting the array");
            return false;
        }
        m_complexTypeStack.pop();
        m_localBuffer.setInt(((Integer) m_arraySizePtr.pop()).intValue(),
                             ((Integer) m_arrayElementCount.peek()).intValue());


        if (((Integer) m_arrayElementCount.peek()).intValue() != 0) {
            m_expectedArrayElementType.pop();
        }
        m_arrayElementCount.pop();
        return addedElement(kOther);
    }

    /**
     * Start a cluster in the packed dashboard data structure.
     *
     * After calling addCluster(), call the appropriate Add method for each element of the cluster.
     * You can use clusters inside of arrays to make each element of the array contain a structure of values.
     * Every call to addCluster() must have a matching call to finalizeCluster().
     * @return True on success
     */
    public boolean addCluster() {
        m_complexTypeStack.push(kCluster);
        return true;
    }

    /**
     * Indicate the end of a cluster packed into the dashboard data structure.
     *
     * After packing data into the cluster, call finalizeCluster().
     * Every call to addCluster() must have a matching call to finalizeCluster
     * @return True on success
     */
    public boolean finalizeCluster() {
        if (m_complexTypeStack.peek() != kCluster) {
            System.err.println("Attempted to close a cluster on an open array or without starting the cluster");
            return false;
        }
        m_complexTypeStack.pop();
        return addedElement(kOther);
    }

    /**
     * Indicate that the packing is complete and commit the buffer to the DriverStation.
     *
     * The packing of the dashboard packet is complete.
     * If you are not using the packed dashboard data, you can call commit() to commit the Printf() buffer and the error string buffer.
     * In effect, you are packing an empty structure.
     * Prepares a packet to go to the dashboard...
     * Pack the sequence number, Printf() buffer, the errors messages (not implemented yet), and packed dashboard data buffer.
     * @return The total size of the data packed into the userData field of the status packet.
     */
    public synchronized int commit() {

        if (!m_complexTypeStack.empty()) {
            System.err.println("didn't finish complex type");
            m_packPtr = 0;
            System.err.println("didn't finish complex type");
            return 0;
        }

        if(!m_reported) {
            UsageReporting.report(tResourceType.kResourceType_Dashboard, 0);
            m_reported = true;
        }

        synchronized (m_statusDataSemaphore) {
            // Sequence number
            DriverStation.getInstance().incrementUpdateNumber();

            // Packed Dashboard Data
            m_userStatusSize = m_packPtr;
            m_userStatus.setBytes(0, m_localBuffer.m_bytes, 0, m_userStatusSize);
            m_packPtr = 0;

        }
        return m_userStatusSize;
    }

    /**
     * Validate that the data being packed will fit in the buffer.
     */
    private boolean validateAdd(int size) {
        if (m_packPtr + size > kMaxDashboardDataSize) {
            m_packPtr = 0;
            System.err.println("Dashboard data is too long to send");
            return false;
        }
        return true;
    }

    /**
     * Check for consistent types when adding elements to an array and keep track of the number of elements in the array.
     */
    private boolean addedElement(Integer type) {
        if (isArrayRoot()) {
            if (((Integer) m_arrayElementCount.peek()).intValue() == 0) {
                m_expectedArrayElementType.push(type);
            } else {
                if (type != m_expectedArrayElementType.peek()) {
                    System.err.println("Attempted to add multiple datatypes to the same array");
                    return false;
                }
            }
            m_arrayElementCount.push(new Integer(((Integer) m_arrayElementCount.pop()).intValue() + 1));
        }
        return true;
    }

    /**
     * If the top of the type stack an array?
     */
    private boolean isArrayRoot() {
        return !m_complexTypeStack.empty() && m_complexTypeStack.peek() == kArray;
    }

    public byte[] getBytes() {
        return m_userStatus.m_bytes;
    }

    public int getBytesLength() {
        return m_userStatusSize;
    }

    public void flush() {
    }
}
