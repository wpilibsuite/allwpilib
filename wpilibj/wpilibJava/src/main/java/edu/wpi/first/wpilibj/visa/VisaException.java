/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.visa;

/**
 * Exception class which looks up visa error codes
 * @author dtjones
 */
public class VisaException extends Exception {

    /**
     * Create a new VisaException
     * @param function the name of the function which returned the status code
     * @param errorCode the status code returned by the function
     */
    public VisaException(String function, int errorCode) {
        super(makeMessage(function, errorCode));
    }

    public static String makeMessage(String function, int errorCode) {
        return lookUpCode(errorCode) + " in function " + function;
    }

    public static String lookUpCode(int errorCode) {

        switch (errorCode) {
        case -1073807360:
            return "VI_ERROR_SYSTEM_ERROR";
        case -1073807346:
            return "VI_ERROR_INV_OBJECT";
        case -1073807345:
            return "VI_ERROR_RSRC_LOCKED";
        case -1073807344:
            return "VI_ERROR_INV_EXPR";
        case -1073807343:
            return "VI_ERROR_RSRC_NFOUND";
        case -1073807342:
            return "VI_ERROR_INV_RSRC_NAME";
        case -1073807341:
            return "VI_ERROR_INV_ACC_MODE";
        case -1073807339:
            return "VI_ERROR_TMO";
        case -1073807338:
            return "VI_ERROR_CLOSING_FAILED";
        case -1073807333:
            return "VI_ERROR_INV_DEGREE";
        case -1073807332:
            return "VI_ERROR_INV_JOB_ID";
        case -1073807331:
            return "VI_ERROR_NSUP_ATTR";
        case -1073807330:
            return "VI_ERROR_NSUP_ATTR_STATE";
        case -1073807329:
            return "VI_ERROR_ATTR_READONLY";
        case -1073807328:
            return "VI_ERROR_INV_LOCK_TYPE";
        case -1073807327:
            return "VI_ERROR_INV_ACCESS_KEY";
        case -1073807322:
            return "VI_ERROR_INV_EVENT";
        case -1073807321:
            return "VI_ERROR_INV_MECH";
        case -1073807320:
            return "VI_ERROR_HNDLR_NINSTALLED";
        case -1073807319:
            return "VI_ERROR_INV_HNDLR_REF";
        case -1073807318:
            return "VI_ERROR_INV_CONTEXT";
        case -1073807315:
            return "VI_ERROR_QUEUE_OVERFLOW";
        case -1073807313:
            return "VI_ERROR_NENABLED";
        case -1073807312:
            return "VI_ERROR_ABORT";
        case -1073807308:
            return "VI_ERROR_RAW_WR_PROT_VIOL";
        case -1073807307:
            return "VI_ERROR_RAW_RD_PROT_VIOL";
        case -1073807306:
            return "VI_ERROR_OUTP_PROT_VIOL";
        case -1073807305:
            return "VI_ERROR_INP_PROT_VIOL";
        case -1073807304:
            return "VI_ERROR_BERR";
        case -1073807303:
            return "VI_ERROR_IN_PROGRESS";
        case -1073807302:
            return "VI_ERROR_INV_SETUP";
        case -1073807301:
            return "VI_ERROR_QUEUE_ERROR";
        case -1073807300:
            return "VI_ERROR_ALLOC";
        case -1073807299:
            return "VI_ERROR_INV_MASK";
        case -1073807298:
            return "VI_ERROR_IO";
        case -1073807297:
            return "VI_ERROR_INV_FMT";
        case -1073807295:
            return "VI_ERROR_NSUP_FMT";
        case -1073807294:
            return "VI_ERROR_LINE_IN_USE";
        case -1073807290:
            return "VI_ERROR_NSUP_MODE";
        case -1073807286:
            return "VI_ERROR_SRQ_NOCCURRED";
        case -1073807282:
            return "VI_ERROR_INV_SPACE";
        case -1073807279:
            return "VI_ERROR_INV_OFFSET";
        case -1073807278:
            return "VI_ERROR_INV_WIDTH";
        case -1073807276:
            return "VI_ERROR_NSUP_OFFSET";
        case -1073807275:
            return "VI_ERROR_NSUP_VAR_WIDTH";
        case -1073807273:
            return "VI_ERROR_WINDOW_NMAPPED";
        case -1073807271:
            return "VI_ERROR_RESP_PENDING";
        case -1073807265:
            return "VI_ERROR_NLISTENERS";
        case -1073807264:
            return "VI_ERROR_NCIC";
        case -1073807263:
            return "VI_ERROR_NSYS_CNTLR";
        case -1073807257:
            return "VI_ERROR_NSUP_OPER";
        case -1073807256:
            return "VI_ERROR_INTR_PENDING";
        case -1073807254:
            return "VI_ERROR_ASRL_PARITY";
        case -1073807253:
            return "VI_ERROR_ASRL_FRAMING";
        case -1073807252:
            return "VI_ERROR_ASRL_OVERRUN";
        case -1073807250:
            return "VI_ERROR_TRIG_NMAPPED";
        case -1073807248:
            return "VI_ERROR_NSUP_ALIGN_OFFSET";
        case -1073807247:
            return "VI_ERROR_USER_BUF";
        case -1073807246:
            return "VI_ERROR_RSRC_BUSY";
        case -1073807242:
            return "VI_ERROR_NSUP_WIDTH";
        case -1073807240:
            return "VI_ERROR_INV_PARAMETER";
        case -1073807239:
            return "VI_ERROR_INV_PROT";
        case -1073807237:
            return "VI_ERROR_INV_SIZE";
        case -1073807232:
            return "VI_ERROR_WINDOW_MAPPED";
        case -1073807231:
            return "VI_ERROR_NIMPL_OPER";
        case -1073807229:
            return "VI_ERROR_INV_LENGTH";
        case -1073807215:
            return "VI_ERROR_INV_MODE";
        case -1073807204:
            return "VI_ERROR_SESN_NLOCKED";
        case -1073807203:
            return "VI_ERROR_MEM_NSHARED";
        case -1073807202:
            return "VI_ERROR_LIBRARY_NFOUND";
        case -1073807201:
            return "VI_ERROR_NSUP_INTR";
        case -1073807200:
            return "VI_ERROR_INV_LINE";
        case -1073807199:
            return "VI_ERROR_FILE_ACCESS";
        case -1073807198:
            return "VI_ERROR_FILE_IO";
        case -1073807197:
            return "VI_ERROR_NSUP_LINE";
        case -1073807196:
            return "VI_ERROR_NSUP_MECH";
        case -1073807195:
            return "VI_ERROR_INTF_NUM_NCONFIG";
        case -1073807194:
            return "VI_ERROR_CONN_LOST";
        case -1073807193:
            return "VI_ERROR_MACHINE_NAVAIL";
        case -1073807192:
            return "VI_ERROR_NPERMISSION";
        default:
            return "Unknown error code";
        }
    }
}
