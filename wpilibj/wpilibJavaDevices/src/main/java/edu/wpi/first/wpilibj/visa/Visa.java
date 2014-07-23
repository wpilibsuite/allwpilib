/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.visa;

import com.sun.jna.Function;
import com.sun.jna.NativeLibrary;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

/**
 *  This port of visa.h includes only the functions and definitions used in
 * SerialPort.java
 * @author dtjones
 */
public class Visa {

    /**
     * Stores the status of the last operation.
     */
    private static int status;

    /**
     * temp buffer for getting values by references
     */
    private static final IntByReference pValue = new IntByReference(0);

    public static final int VI_ATTR_ASRL_BAUD = (0x3FFF0021);
    public static final int VI_ATTR_ASRL_DATA_BITS = (0x3FFF0022);
    public static final int VI_ATTR_ASRL_PARITY = (0x3FFF0023);
    public static final int VI_ATTR_ASRL_STOP_BITS = (0x3FFF0024);
    public static final int VI_ATTR_ASRL_FLOW_CNTRL = (0x3FFF0025);
    public static final int VI_ATTR_TERMCHAR_EN = (0x3FFF0038);
    public static final int VI_ATTR_TERMCHAR = (0x3FFF0018);
    public static final int VI_ATTR_ASRL_END_IN = (0x3FFF00B3);
    public static final int VI_ASRL_END_TERMCHAR = 2;
    public static final int VI_ASRL_END_NONE = 0;
    public static final int VI_ATTR_ASRL_AVAIL_NUM = (0x3FFF00AC);
    public static final int VI_SUCCESS_TERM_CHAR = (0x3FFF0005);
    public static final int VI_SUCCESS_MAX_CNT = (0x3FFF0006);
    public static final int VI_ATTR_TMO_VALUE = (0x3FFF001A);
    public static final int VI_ATTR_WR_BUF_OPER_MODE = (0x3FFF002D);
    public static final short VI_READ_BUF = 1;
    public static final short VI_WRITE_BUF = 2;

    private static final Function viOpenDefaultRMFn = NativeLibrary.getDefaultInstance().getFunction("viOpenDefaultRM");
    public synchronized static int viOpenDefaultRM() throws VisaException {
        status = viOpenDefaultRMFn.call1(pValue.getPointer());
        assertCleanStatus("viOpenDefaultRM");
        return pValue.getValue();
    }

    private static final Function viOpenFn = NativeLibrary.getDefaultInstance().getFunction("viOpen");
    public synchronized static int viOpen(int sesn, String name, int mode,
                                          int timeout) throws VisaException {
        Pointer pName = new Pointer(name.length());
        pName.setBytes(0, name.getBytes(), 0, name.length());
        status = viOpenFn.call5(sesn, pName, mode, timeout, pValue.getPointer());
        assertCleanStatus("viOpen");
        pName.free();
        return pValue.getValue();
    }

    private static final Function viSetAttributeFn = NativeLibrary.getDefaultInstance().getFunction("viSetAttribute");
    public static void viSetAttribute(int vi, int attrName, int attrValue) throws VisaException {
        status = viSetAttributeFn.call3(vi, attrName, attrValue);
        assertCleanStatus("viSetAttribute");
    }

    public static void viSetAttribute(int vi, int attrName, boolean attrValue) throws VisaException {
        status = viSetAttributeFn.call3(vi, attrName, attrValue?1:0);
        assertCleanStatus("viSetAttribute");
    }

    private static final Function viSetBufFn = NativeLibrary.getDefaultInstance().getFunction("viSetBuf");
    public static void viSetBuf(int vi, short buffer, int size) throws VisaException {
        status = viSetBufFn.call3(vi, buffer, size);
        assertCleanStatus("viSetBuf");
    }

    private static final Function viCloseFn = NativeLibrary.getDefaultInstance().getFunction("viClose");
    public static void viClose(int vi) {
        viCloseFn.call1(vi);
    }

    private static final Function viGetAttributeFn = NativeLibrary.getDefaultInstance().getFunction("viGetAttribute");
    public synchronized static int viGetAttribute(int vi, int attrName) throws VisaException {
        status = viGetAttributeFn.call3(vi, attrName, pValue.getPointer());
        assertCleanStatus("viGetAttribute");
        return pValue.getValue();
    }

    //Doesn't work correctly. Correct parameters are vi, a format string, and a list of args
    private static final Function viVPrintfFn = NativeLibrary.getDefaultInstance().getFunction("viVPrintf");
    public static void viVPrintf(int vi, String write) throws VisaException {
        Pointer string = new Pointer(write.length());
        string.setBytes(0, write.getBytes(), 0, write.length());
        status = viVPrintfFn.call2(vi, string);
        string.free();
        assertCleanStatus("viPrintf");
    }

    private static final Function viBufReadFn = NativeLibrary.getDefaultInstance().getFunction("viBufRead");
    public static byte[] viBufRead(int vi, int cnt) throws VisaException {
        Pointer bytes = new Pointer(cnt);
        Pointer retCnt = new Pointer(4);
        status = viBufReadFn.call4(vi, bytes, cnt, retCnt);
        switch (status) {
        case VI_SUCCESS_TERM_CHAR:
        case VI_SUCCESS_MAX_CNT:
            status = 0;
            break;
        default:
            assertCleanStatus("viBufRead");
        }

        byte[] toReturn = new byte[cnt];
        bytes.getBytes(0, toReturn, 0, cnt);
        bytes.free();
        return toReturn;
    }

    private static final Function viBufWriteFn = NativeLibrary.getDefaultInstance().getFunction("viBufWrite");
    public synchronized static int viBufWrite(int vi, byte[] buf, int cnt) throws VisaException {
        Pointer string = new Pointer(buf.length);
        string.setBytes(0, buf, 0, buf.length);
        status = viBufWriteFn.call4(vi, string, cnt, pValue.getPointer());
        assertCleanStatus("viBufWrite");
        string.free();
        return pValue.getValue();
    }

    private static final Function viFlushFn = NativeLibrary.getDefaultInstance().getFunction("viFlush");
    public static void viFlush(int vi, short mask) throws VisaException {
        status = viFlushFn.call2(vi, mask);
        assertCleanStatus("viFlush");
    }

    private static final Function viClearFn = NativeLibrary.getDefaultInstance().getFunction("viClear");
    public static void viClear(int vi) throws VisaException {
        viClearFn.call1(vi);
        assertCleanStatus("viClear");
    }

    protected static final void assertCleanStatus(String function) throws VisaException {
        if (status < 0) {
            throw new VisaException(function, status);
        } else if (status != 0) {
            System.out.println(VisaException.makeMessage(function, status));
            status = 0;
        }
    }

    private Visa() {
        /* do not instantiate utility class */
    }
}
