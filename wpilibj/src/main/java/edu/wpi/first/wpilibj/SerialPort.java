/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.io.UnsupportedEncodingException;

import edu.wpi.first.wpilibj.visa.Visa;
import edu.wpi.first.wpilibj.visa.VisaException;

/**
 * Driver for the RS-232 serial port on the cRIO.
 *
 * The current implementation uses the VISA formatted I/O mode.  This means that
 *   all traffic goes through the formatted buffers.  This allows the intermingled
 *   use of print(), readString(), and the raw buffer accessors read() and write().
 *
 * More information can be found in the NI-VISA User Manual here:
 *   http://www.ni.com/pdf/manuals/370423a.pdf
 * and the NI-VISA Programmer's Reference Manual here:
 *   http://www.ni.com/pdf/manuals/370132c.pdf
 */
public class SerialPort {

    private int m_resourceManagerHandle;
    private int m_portHandle;

    /**
     * Represents the parity to use for serial communications
     */
    public static class Parity {

        /**
         * The integer value representing this enumeration
         */
        public final int value;
        static final int kNone_val = 0;
        static final int kOdd_val = 1;
        static final int kEven_val = 2;
        static final int kMark_val = 3;
        static final int kSpace_val = 4;
        /**
         * parity: Use no parity
         */
        public static final Parity kNone = new Parity(kNone_val);
        /**
         * parity: Use odd parity
         */
        public static final Parity kOdd = new Parity(kOdd_val);
        /**
         * parity: Use even parity
         */
        public static final Parity kEven = new Parity(kEven_val);
        /**
         * parity: Use mark parity
         */
        public static final Parity kMark = new Parity(kMark_val);
        /**
         * parity: Use space parity
         */
        public static final Parity kSpace = new Parity((kSpace_val));

        private Parity(int value) {
            this.value = value;
        }
    }

    /**
     * Represents the number of stop bits to use for Serial Communication
     */
    public static class StopBits {

        /**
         * The integer value representing this enumeration
         */
        public final int value;
        static final int kOne_val = 10;
        static final int kOnePointFive_val = 15;
        static final int kTwo_val = 20;
        /**
         * stopBits: use 1
         */
        public static final StopBits kOne = new StopBits(kOne_val);
        /**
         * stopBits: use 1.5
         */
        public static final StopBits kOnePointFive = new StopBits(kOnePointFive_val);
        /**
         * stopBits: use 2
         */
        public static final StopBits kTwo = new StopBits(kTwo_val);

        private StopBits(int value) {
            this.value = value;
        }
    }

    /**
     * Represents what type of flow control to use for serial communication
     */
    public static class FlowControl {

        /**
         * The integer value representing this enumeration
         */
        public final int value;
        static final int kNone_val = 0;
        static final int kXonXoff_val = 1;
        static final int kRtsCts_val = 2;
        static final int kDtrDsr_val = 4;
        /**
         * flowControl: use none
         */
        public static final FlowControl kNone = new FlowControl(kNone_val);
        /**
         * flowcontrol: use on/off
         */
        public static final FlowControl kXonXoff = new FlowControl(kXonXoff_val);
        /**
         * flowcontrol: use rts cts
         */
        public static final FlowControl kRtsCts = new FlowControl(kRtsCts_val);
        /**
         * flowcontrol: use dts dsr
         */
        public static final FlowControl kDtrDsr = new FlowControl(kDtrDsr_val);

        private FlowControl(int value) {
            this.value = value;
        }
    }

    /**
     * Represents which type of buffer mode to use when writing to a serial port
     */
    public static class WriteBufferMode {

        /**
         * The integer value representing this enumeration
         */
        public final int value;
        static final int kFlushOnAccess_val = 1;
        static final int kFlushWhenFull_val = 2;
        /**
         * Flush on access
         */
        public static final WriteBufferMode kFlushOnAccess = new WriteBufferMode(kFlushOnAccess_val);
        /**
         * Flush when full
         */
        public static final WriteBufferMode kFlushWhenFull = new WriteBufferMode(kFlushWhenFull_val);

        private WriteBufferMode(int value) {
            this.value = value;
        }
    }

    /**
     * Create an instance of a Serial Port class.
     *
     * @param baudRate The baud rate to configure the serial port.  The cRIO-9074 supports up to 230400 Baud.
     * @param dataBits The number of data bits per transfer.  Valid values are between 5 and 8 bits.
     * @param parity Select the type of parity checking to use.
     * @param stopBits The number of stop bits to use as defined by the enum StopBits.
     */
    public SerialPort(final int baudRate, final int dataBits, Parity parity, StopBits stopBits) throws VisaException {
        m_resourceManagerHandle = 0;
        m_portHandle = 0;

        m_resourceManagerHandle = Visa.viOpenDefaultRM();

        m_portHandle = Visa.viOpen(m_resourceManagerHandle, "ASRL1::INSTR", 0, 0);

        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_BAUD, baudRate);

        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_DATA_BITS, dataBits);

        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_PARITY, parity.value);

        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_STOP_BITS, stopBits.value);

        // Set the default read buffer size to 1 to return bytes immediately
        setReadBufferSize(1);

        // Set the default timeout to 5 seconds.
        setTimeout(5.0f);

        // Don't wait until the buffer is full to transmit.
        setWriteBufferMode(WriteBufferMode.kFlushOnAccess);

        disableTermination();

        //viInstallHandler(m_portHandle, VI_EVENT_IO_COMPLETION, ioCompleteHandler, this);
        //viEnableEvent(m_portHandle, VI_EVENT_IO_COMPLETION, VI_HNDLR, VI_NULL);

		// XXX: Resource Reporting Fixes
//        UsageReporting.report(UsageReporting.kResourceType_SerialPort,0);
    }

    /**
     * Create an instance of a Serial Port class. Defaults to one stop bit.
     *
     * @param baudRate The baud rate to configure the serial port.  The cRIO-9074 supports up to 230400 Baud.
     * @param dataBits The number of data bits per transfer.  Valid values are between 5 and 8 bits.
     * @param parity Select the type of parity checking to use.
     */
    public SerialPort(final int baudRate, final int dataBits, Parity parity) throws VisaException {
        this(baudRate, dataBits, parity, StopBits.kOne);
    }

    /**
     * Create an instance of a Serial Port class. Defaults to no parity and one
     * stop bit.
     *
     * @param baudRate The baud rate to configure the serial port.  The cRIO-9074 supports up to 230400 Baud.
     * @param dataBits The number of data bits per transfer.  Valid values are between 5 and 8 bits.
     */
    public SerialPort(final int baudRate, final int dataBits) throws VisaException {
        this(baudRate, dataBits, Parity.kNone, StopBits.kOne);
    }

    /**
     * Create an instance of a Serial Port class. Defaults to 8 databits,
     * no parity, and one stop bit.
     *
     * @param baudRate The baud rate to configure the serial port.  The cRIO-9074 supports up to 230400 Baud.
     */
    public SerialPort(final int baudRate) throws VisaException {
        this(baudRate, 8, Parity.kNone, StopBits.kOne);
    }

    /**
     * Destructor.
     */
    public void free() {
        //viUninstallHandler(m_portHandle, VI_EVENT_IO_COMPLETION, ioCompleteHandler, this);
        Visa.viClose(m_portHandle);
        Visa.viClose(m_resourceManagerHandle);
    }

    /**
     * Set the type of flow control to enable on this port.
     *
     * By default, flow control is disabled.
     * @param flowControl
     */
    public void setFlowControl(FlowControl flowControl) throws VisaException {
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_FLOW_CNTRL, flowControl.value);
    }

    /**
     * Enable termination and specify the termination character.
     *
     * Termination is currently only implemented for receive.
     * When the the terminator is received, the read() or readString() will return
     *   fewer bytes than requested, stopping after the terminator.
     *
     * @param terminator The character to use for termination.
     */
    public void enableTermination(char terminator) throws VisaException {
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_TERMCHAR_EN, true);
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_TERMCHAR, terminator);
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_END_IN, Visa.VI_ASRL_END_TERMCHAR);
    }

    /**
     * Enable termination and specify the termination character.
     *
     * Termination is currently only implemented for receive.
     * When the the terminator is received, the read() or readString() will return
     *   fewer bytes than requested, stopping after the terminator.
     *
     * The default terminator is '\n'
     */
    public void enableTermination() throws VisaException {
        this.enableTermination('\n');
    }

    /**
     * Disable termination behavior.
     */
    public void disableTermination() throws VisaException {
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_TERMCHAR_EN, false);
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_END_IN, Visa.VI_ASRL_END_NONE);
    }

    /**
     * Get the number of bytes currently available to read from the serial port.
     *
     * @return The number of bytes available to read.
     */
    public int getBytesReceived() throws VisaException {
        return Visa.viGetAttribute(m_portHandle, Visa.VI_ATTR_ASRL_AVAIL_NUM);
    }

    /**
     * Output formatted text to the serial port.
     *
     * @deprecated use write(string.getBytes()) instead
     * @bug All pointer-based parameters seem to return an error.
     *
     * @param write A string to write
     */
    public void print(String write) throws VisaException {
        Visa.viVPrintf(m_portHandle, write);
    }

    /**
     * Read a string out of the buffer. Reads the entire contents of the buffer
     *
     * @return The read string
     */
    public String readString() throws VisaException {
        return readString(getBytesReceived());
    }

    /**
     * Read a string out of the buffer. Reads the entire contents of the buffer
     *
     * @param count the number of characters to read into the string
     * @return The read string
     */
    public String readString(int count) throws VisaException {
        byte[] out = Visa.viBufRead(m_portHandle, count);
        try {
            return new String(out, 0, count, "US-ASCII");
        } catch (UnsupportedEncodingException ex) {
            ex.printStackTrace();
            return new String();
        }
    }

    /**
     * Read raw bytes out of the buffer.
     *
     * @param count The maximum number of bytes to read.
     * @return An array of the read bytes
     */
    public byte[] read(final int count) throws VisaException {
        return Visa.viBufRead(m_portHandle, count);
    }

    /**
     * Write raw bytes to the buffer.
     *
     * @param buffer the buffer to read the bytes from.
     * @param count The maximum number of bytes to write.
     * @return The number of bytes actually written into the port.
     */
    public int write(byte[] buffer, int count) throws VisaException {
        return Visa.viBufWrite(m_portHandle, buffer, count);
    }

    /**
     * Configure the timeout of the serial port.
     *
     * This defines the timeout for transactions with the hardware.
     * It will affect reads if less bytes are available than the
     * read buffer size (defaults to 1) and very large writes.
     *
     * @param timeout The number of seconds to to wait for I/O.
     */
    public void setTimeout(double timeout) throws VisaException {
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_TMO_VALUE, (int) (timeout * 1e3));
    }

    /**
     * Specify the size of the input buffer.
     *
     * Specify the amount of data that can be stored before data
     * from the device is returned to Read.  If you want
     * data that is received to be returned immediately, set this to 1.
     *
     * It the buffer is not filled before the read timeout expires, all
     * data that has been received so far will be returned.
     *
     * @param size The read buffer size.
     */
    void setReadBufferSize(int size) throws VisaException {
        Visa.viSetBuf(m_portHandle, Visa.VI_READ_BUF, size);
    }

    /**
    * Specify the size of the output buffer.
    *
    * Specify the amount of data that can be stored before being
    * transmitted to the device.
    *
    * @param size The write buffer size.
    */
    void setWriteBufferSize(int size) throws VisaException {
        Visa.viSetBuf(m_portHandle, Visa.VI_WRITE_BUF, size);
    }

    /**
     * Specify the flushing behavior of the output buffer.
     *
     * When set to kFlushOnAccess, data is synchronously written to the serial port
     *   after each call to either print() or write().
     *
     * When set to kFlushWhenFull, data will only be written to the serial port when
     *   the buffer is full or when flush() is called.
     *
     * @param mode The write buffer mode.
     */
    public void setWriteBufferMode(WriteBufferMode mode) throws VisaException {
        Visa.viSetAttribute(m_portHandle, Visa.VI_ATTR_WR_BUF_OPER_MODE, mode.value);
    }

    /**
     * Force the output buffer to be written to the port.
     *
     * This is used when setWriteBufferMode() is set to kFlushWhenFull to force a
     * flush before the buffer is full.
     */
    public void flush() throws VisaException {
        Visa.viFlush(m_portHandle, Visa.VI_WRITE_BUF);
    }

    /**
     * Reset the serial port driver to a known state.
     *
     * Empty the transmit and receive buffers in the device and formatted I/O.
     */
    public void reset() throws VisaException {
        Visa.viClear(m_portHandle);
    }
}
