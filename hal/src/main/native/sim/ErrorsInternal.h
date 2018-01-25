/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

typedef enum {
  CTR_OKAY,       // No Error - Function executed as expected
  CTR_RxTimeout,  // CAN frame has not been received within specified period of
                  // time.
  CTR_TxTimeout,  // Not used.
  CTR_InvalidParamValue,  // Caller passed an invalid param
  CTR_UnexpectedArbId,    // Specified CAN Id is invalid.
  CTR_TxFailed,           // Could not transmit the CAN frame.
  CTR_SigNotUpdated,      // Have not received an value response for signal.
  CTR_BufferFull,  // Caller attempted to insert data into a buffer that is
                   // full.
} CTR_Code;

// VISA Error
#define _VI_ERROR (-2147483647L - 1)
#define VI_ERROR_SYSTEM_ERROR (_VI_ERROR + 0x3FFF0000L)
#define VI_ERROR_INV_OBJECT (_VI_ERROR + 0x3FFF000EL)
#define VI_ERROR_RSRC_LOCKED (_VI_ERROR + 0x3FFF000FL)
#define VI_ERROR_INV_EXPR (_VI_ERROR + 0x3FFF0010L)
#define VI_ERROR_RSRC_NFOUND (_VI_ERROR + 0x3FFF0011L)
#define VI_ERROR_INV_RSRC_NAME (_VI_ERROR + 0x3FFF0012L)
#define VI_ERROR_INV_ACC_MODE (_VI_ERROR + 0x3FFF0013L)
#define VI_ERROR_TMO (_VI_ERROR + 0x3FFF0015L)
#define VI_ERROR_CLOSING_FAILED (_VI_ERROR + 0x3FFF0016L)
#define VI_ERROR_INV_DEGREE (_VI_ERROR + 0x3FFF001BL)
#define VI_ERROR_INV_JOB_ID (_VI_ERROR + 0x3FFF001CL)
#define VI_ERROR_NSUP_ATTR (_VI_ERROR + 0x3FFF001DL)
#define VI_ERROR_NSUP_ATTR_STATE (_VI_ERROR + 0x3FFF001EL)
#define VI_ERROR_ATTR_READONLY (_VI_ERROR + 0x3FFF001FL)
#define VI_ERROR_INV_LOCK_TYPE (_VI_ERROR + 0x3FFF0020L)
#define VI_ERROR_INV_ACCESS_KEY (_VI_ERROR + 0x3FFF0021L)
#define VI_ERROR_INV_EVENT (_VI_ERROR + 0x3FFF0026L)
#define VI_ERROR_INV_MECH (_VI_ERROR + 0x3FFF0027L)
#define VI_ERROR_HNDLR_NINSTALLED (_VI_ERROR + 0x3FFF0028L)
#define VI_ERROR_INV_HNDLR_REF (_VI_ERROR + 0x3FFF0029L)
#define VI_ERROR_INV_CONTEXT (_VI_ERROR + 0x3FFF002AL)
#define VI_ERROR_QUEUE_OVERFLOW (_VI_ERROR + 0x3FFF002DL)
#define VI_ERROR_NENABLED (_VI_ERROR + 0x3FFF002FL)
#define VI_ERROR_ABORT (_VI_ERROR + 0x3FFF0030L)
#define VI_ERROR_RAW_WR_PROT_VIOL (_VI_ERROR + 0x3FFF0034L)
#define VI_ERROR_RAW_RD_PROT_VIOL (_VI_ERROR + 0x3FFF0035L)
#define VI_ERROR_OUTP_PROT_VIOL (_VI_ERROR + 0x3FFF0036L)
#define VI_ERROR_INP_PROT_VIOL (_VI_ERROR + 0x3FFF0037L)
#define VI_ERROR_BERR (_VI_ERROR + 0x3FFF0038L)
#define VI_ERROR_IN_PROGRESS (_VI_ERROR + 0x3FFF0039L)
#define VI_ERROR_INV_SETUP (_VI_ERROR + 0x3FFF003AL)
#define VI_ERROR_QUEUE_ERROR (_VI_ERROR + 0x3FFF003BL)
#define VI_ERROR_ALLOC (_VI_ERROR + 0x3FFF003CL)
#define VI_ERROR_INV_MASK (_VI_ERROR + 0x3FFF003DL)
#define VI_ERROR_IO (_VI_ERROR + 0x3FFF003EL)
#define VI_ERROR_INV_FMT (_VI_ERROR + 0x3FFF003FL)
#define VI_ERROR_NSUP_FMT (_VI_ERROR + 0x3FFF0041L)
#define VI_ERROR_LINE_IN_USE (_VI_ERROR + 0x3FFF0042L)
#define VI_ERROR_NSUP_MODE (_VI_ERROR + 0x3FFF0046L)
#define VI_ERROR_SRQ_NOCCURRED (_VI_ERROR + 0x3FFF004AL)
#define VI_ERROR_INV_SPACE (_VI_ERROR + 0x3FFF004EL)
#define VI_ERROR_INV_OFFSET (_VI_ERROR + 0x3FFF0051L)
#define VI_ERROR_INV_WIDTH (_VI_ERROR + 0x3FFF0052L)
#define VI_ERROR_NSUP_OFFSET (_VI_ERROR + 0x3FFF0054L)
#define VI_ERROR_NSUP_VAR_WIDTH (_VI_ERROR + 0x3FFF0055L)
#define VI_ERROR_WINDOW_NMAPPED (_VI_ERROR + 0x3FFF0057L)
#define VI_ERROR_RESP_PENDING (_VI_ERROR + 0x3FFF0059L)
#define VI_ERROR_NLISTENERS (_VI_ERROR + 0x3FFF005FL)
#define VI_ERROR_NCIC (_VI_ERROR + 0x3FFF0060L)
#define VI_ERROR_NSYS_CNTLR (_VI_ERROR + 0x3FFF0061L)
#define VI_ERROR_NSUP_OPER (_VI_ERROR + 0x3FFF0067L)
#define VI_ERROR_INTR_PENDING (_VI_ERROR + 0x3FFF0068L)
#define VI_ERROR_ASRL_PARITY (_VI_ERROR + 0x3FFF006AL)
#define VI_ERROR_ASRL_FRAMING (_VI_ERROR + 0x3FFF006BL)
#define VI_ERROR_ASRL_OVERRUN (_VI_ERROR + 0x3FFF006CL)
#define VI_ERROR_TRIG_NMAPPED (_VI_ERROR + 0x3FFF006EL)
#define VI_ERROR_NSUP_ALIGN_OFFSET (_VI_ERROR + 0x3FFF0070L)
#define VI_ERROR_USER_BUF (_VI_ERROR + 0x3FFF0071L)
#define VI_ERROR_RSRC_BUSY (_VI_ERROR + 0x3FFF0072L)
#define VI_ERROR_NSUP_WIDTH (_VI_ERROR + 0x3FFF0076L)
#define VI_ERROR_INV_PARAMETER (_VI_ERROR + 0x3FFF0078L)
#define VI_ERROR_INV_PROT (_VI_ERROR + 0x3FFF0079L)
#define VI_ERROR_INV_SIZE (_VI_ERROR + 0x3FFF007BL)
#define VI_ERROR_WINDOW_MAPPED (_VI_ERROR + 0x3FFF0080L)
#define VI_ERROR_NIMPL_OPER (_VI_ERROR + 0x3FFF0081L)
#define VI_ERROR_INV_LENGTH (_VI_ERROR + 0x3FFF0083L)
#define VI_ERROR_INV_MODE (_VI_ERROR + 0x3FFF0091L)
#define VI_ERROR_SESN_NLOCKED (_VI_ERROR + 0x3FFF009CL)
#define VI_ERROR_MEM_NSHARED (_VI_ERROR + 0x3FFF009DL)
#define VI_ERROR_LIBRARY_NFOUND (_VI_ERROR + 0x3FFF009EL)
#define VI_ERROR_NSUP_INTR (_VI_ERROR + 0x3FFF009FL)
#define VI_ERROR_INV_LINE (_VI_ERROR + 0x3FFF00A0L)
#define VI_ERROR_FILE_ACCESS (_VI_ERROR + 0x3FFF00A1L)
#define VI_ERROR_FILE_IO (_VI_ERROR + 0x3FFF00A2L)
#define VI_ERROR_NSUP_LINE (_VI_ERROR + 0x3FFF00A3L)
#define VI_ERROR_NSUP_MECH (_VI_ERROR + 0x3FFF00A4L)
#define VI_ERROR_INTF_NUM_NCONFIG (_VI_ERROR + 0x3FFF00A5L)
#define VI_ERROR_CONN_LOST (_VI_ERROR + 0x3FFF00A6L)
#define VI_ERROR_MACHINE_NAVAIL (_VI_ERROR + 0x3FFF00A7L)
#define VI_ERROR_NPERMISSION (_VI_ERROR + 0x3FFF00A8L)

// FPGA Errors

/**
 * Represents the resulting status of a function call through its return value.
 * 0 is success, negative values are errors, and positive values are warnings.
 */
typedef int32_t NiFpga_Status;

/**
 * No errors or warnings.
 */
static const NiFpga_Status NiFpga_Status_Success = 0;

/**
 * The timeout expired before the FIFO operation could complete.
 */
static const NiFpga_Status NiFpga_Status_FifoTimeout = -50400;

/**
 * No transfer is in progress because the transfer was aborted by the client.
 * The operation could not be completed as specified.
 */
static const NiFpga_Status NiFpga_Status_TransferAborted = -50405;

/**
 * A memory allocation failed. Try again after rebooting.
 */
static const NiFpga_Status NiFpga_Status_MemoryFull = -52000;

/**
 * An unexpected software error occurred.
 */
static const NiFpga_Status NiFpga_Status_SoftwareFault = -52003;

/**
 * A parameter to a function was not valid. This could be a NULL pointer, a bad
 * value, etc.
 */
static const NiFpga_Status NiFpga_Status_InvalidParameter = -52005;

/**
 * A required resource was not found. The NiFpga.* library, the RIO resource, or
 * some other resource may be missing.
 */
static const NiFpga_Status NiFpga_Status_ResourceNotFound = -52006;

/**
 * A required resource was not properly initialized. This could occur if
 * NiFpga_Initialize was not called or a required NiFpga_IrqContext was not
 * reserved.
 */
static const NiFpga_Status NiFpga_Status_ResourceNotInitialized = -52010;

/**
 * A hardware failure has occurred. The operation could not be completed as
 * specified.
 */
static const NiFpga_Status NiFpga_Status_HardwareFault = -52018;

/**
 * The FPGA is already running.
 */
static const NiFpga_Status NiFpga_Status_FpgaAlreadyRunning = -61003;

/**
 * An error occurred downloading the VI to the FPGA device. Verify that
 * the target is connected and powered and that the resource of the target
 * is properly configured.
 */
static const NiFpga_Status NiFpga_Status_DownloadError = -61018;

/**
 * The bitfile was not compiled for the specified resource's device type.
 */
static const NiFpga_Status NiFpga_Status_DeviceTypeMismatch = -61024;

/**
 * An error was detected in the communication between the host computer and the
 * FPGA target.
 */
static const NiFpga_Status NiFpga_Status_CommunicationTimeout = -61046;

/**
 * The timeout expired before any of the IRQs were asserted.
 */
static const NiFpga_Status NiFpga_Status_IrqTimeout = -61060;

/**
 * The specified bitfile is invalid or corrupt.
 */
static const NiFpga_Status NiFpga_Status_CorruptBitfile = -61070;

/**
 * The requested FIFO depth is invalid. It is either 0 or an amount not
 * supported by the hardware.
 */
static const NiFpga_Status NiFpga_Status_BadDepth = -61072;

/**
 * The number of FIFO elements is invalid. Either the number is greater than the
 * depth of the host memory DMA FIFO, or more elements were requested for
 * release than had been acquired.
 */
static const NiFpga_Status NiFpga_Status_BadReadWriteCount = -61073;

/**
 * A hardware clocking error occurred. A derived clock lost lock with its base
 * clock during the execution of the LabVIEW FPGA VI. If any base clocks with
 * derived clocks are referencing an external source, make sure that the
 * external source is connected and within the supported frequency, jitter,
 * accuracy, duty cycle, and voltage specifications. Also verify that the
 * characteristics of the base clock match the configuration specified in the
 * FPGA Base Clock Properties. If all base clocks with derived clocks are
 * generated from free-running, on-board sources, please contact National
 * Instruments technical support at ni.com/support.
 */
static const NiFpga_Status NiFpga_Status_ClockLostLock = -61083;

/**
 * The operation could not be performed because the FPGA is busy. Stop all
 * activities on the FPGA before requesting this operation. If the target is in
 * Scan Interface programming mode, put it in FPGA Interface programming mode.
 */
static const NiFpga_Status NiFpga_Status_FpgaBusy = -61141;

/**
 * The operation could not be performed because the FPGA is busy operating in
 * FPGA Interface C API mode. Stop all activities on the FPGA before requesting
 * this operation.
 */
static const NiFpga_Status NiFpga_Status_FpgaBusyFpgaInterfaceCApi = -61200;

/**
 * The chassis is in Scan Interface programming mode. In order to run FPGA VIs,
 * you must go to the chassis properties page, select FPGA programming mode, and
 * deploy settings.
 */
static const NiFpga_Status NiFpga_Status_FpgaBusyScanInterface = -61201;

/**
 * The operation could not be performed because the FPGA is busy operating in
 * FPGA Interface mode. Stop all activities on the FPGA before requesting this
 * operation.
 */
static const NiFpga_Status NiFpga_Status_FpgaBusyFpgaInterface = -61202;

/**
 * The operation could not be performed because the FPGA is busy operating in
 * Interactive mode. Stop all activities on the FPGA before requesting this
 * operation.
 */
static const NiFpga_Status NiFpga_Status_FpgaBusyInteractive = -61203;

/**
 * The operation could not be performed because the FPGA is busy operating in
 * Emulation mode. Stop all activities on the FPGA before requesting this
 * operation.
 */
static const NiFpga_Status NiFpga_Status_FpgaBusyEmulation = -61204;

/**
 * LabVIEW FPGA does not support the Reset method for bitfiles that allow
 * removal of implicit enable signals in single-cycle Timed Loops.
 */
static const NiFpga_Status NiFpga_Status_ResetCalledWithImplicitEnableRemoval =
    -61211;

/**
 * LabVIEW FPGA does not support the Abort method for bitfiles that allow
 * removal of implicit enable signals in single-cycle Timed Loops.
 */
static const NiFpga_Status NiFpga_Status_AbortCalledWithImplicitEnableRemoval =
    -61212;

/**
 * LabVIEW FPGA does not support Close and Reset if Last Reference for bitfiles
 * that allow removal of implicit enable signals in single-cycle Timed Loops.
 * Pass the NiFpga_CloseAttribute_NoResetIfLastSession attribute to NiFpga_Close
 * instead of 0.
 */
static const NiFpga_Status
    NiFpga_Status_CloseAndResetCalledWithImplicitEnableRemoval = -61213;

/**
 * For bitfiles that allow removal of implicit enable signals in single-cycle
 * Timed Loops, LabVIEW FPGA does not support this method prior to running the
 * bitfile.
 */
static const NiFpga_Status NiFpga_Status_ImplicitEnableRemovalButNotYetRun =
    -61214;

/**
 * Bitfiles that allow removal of implicit enable signals in single-cycle Timed
 * Loops can run only once. Download the bitfile again before re-running the VI.
 */
static const NiFpga_Status
    NiFpga_Status_RunAfterStoppedCalledWithImplicitEnableRemoval = -61215;

/**
 * A gated clock has violated the handshaking protocol. If you are using
 * external gated clocks, ensure that they follow the required clock gating
 * protocol. If you are generating your clocks internally, please contact
 * National Instruments Technical Support.
 */
static const NiFpga_Status NiFpga_Status_GatedClockHandshakingViolation =
    -61216;

/**
 * The number of elements requested must be less than or equal to the number of
 * unacquired elements left in the host memory DMA FIFO. There are currently
 * fewer unacquired elements left in the FIFO than are being requested. Release
 * some acquired elements before acquiring more elements.
 */
static const NiFpga_Status NiFpga_Status_ElementsNotPermissibleToBeAcquired =
    -61219;

/**
 * The operation could not be performed because the FPGA is in configuration or
 * discovery mode. Wait for configuration or discovery to complete and retry
 * your operation.
 */
static const NiFpga_Status NiFpga_Status_FpgaBusyConfiguration = -61252;

/**
 * An unexpected internal error occurred.
 */
static const NiFpga_Status NiFpga_Status_InternalError = -61499;

/**
 * The NI-RIO driver was unable to allocate memory for a FIFO. This can happen
 * when the combined depth of all DMA FIFOs exceeds the maximum depth for the
 * controller, or when the controller runs out of system memory. You may be able
 * to reconfigure the controller with a greater maximum FIFO depth. For more
 * information, refer to the NI KnowledgeBase article 65OF2ERQ.
 */
static const NiFpga_Status NiFpga_Status_TotalDmaFifoDepthExceeded = -63003;

/**
 * Access to the remote system was denied. Use MAX to check the Remote Device
 * Access settings under Software>>NI-RIO>>NI-RIO Settings on the remote system.
 */
static const NiFpga_Status NiFpga_Status_AccessDenied = -63033;

/**
 * The NI-RIO software on the host is not compatible with the software on the
 * target. Upgrade the NI-RIO software on the host in order to connect to this
 * target.
 */
static const NiFpga_Status NiFpga_Status_HostVersionMismatch = -63038;

/**
 * A connection could not be established to the specified remote device. Ensure
 * that the device is on and accessible over the network, that NI-RIO software
 * is installed, and that the RIO server is running and properly configured.
 */
static const NiFpga_Status NiFpga_Status_RpcConnectionError = -63040;

/**
 * The RPC session is invalid. The target may have reset or been rebooted. Check
 * the network connection and retry the operation.
 */
static const NiFpga_Status NiFpga_Status_RpcSessionError = -63043;

/**
 * The operation could not complete because another session is accessing the
 * FIFO. Close the other session and retry.
 */
static const NiFpga_Status NiFpga_Status_FifoReserved = -63082;

/**
 * A Configure FIFO, Stop FIFO, Read FIFO, or Write FIFO function was called
 * while the host had acquired elements of the FIFO. Release all acquired
 * elements before configuring, stopping, reading, or writing.
 */
static const NiFpga_Status NiFpga_Status_FifoElementsCurrentlyAcquired = -63083;

/**
 * A function was called using a misaligned address. The address must be a
 * multiple of the size of the datatype.
 */
static const NiFpga_Status NiFpga_Status_MisalignedAccess = -63084;

/**
 * The FPGA Read/Write Control Function is accessing a control or indicator
 * with data that exceeds the maximum size supported on the current target.
 * Refer to the hardware documentation for the limitations on data types for
 * this target.
 */
static const NiFpga_Status NiFpga_Status_ControlOrIndicatorTooLarge = -63085;

/**
 * A valid .lvbitx bitfile is required. If you are using a valid .lvbitx
 * bitfile, the bitfile may not be compatible with the software you are using.
 * Determine which version of LabVIEW was used to make the bitfile, update your
 * software to that version or later, and try again.
 */
static const NiFpga_Status NiFpga_Status_BitfileReadError = -63101;

/**
 * The specified signature does not match the signature of the bitfile. If the
 * bitfile has been recompiled, regenerate the C API and rebuild the
 * application.
 */
static const NiFpga_Status NiFpga_Status_SignatureMismatch = -63106;

/**
 * The bitfile you are trying to use is incompatible with the version
 * of NI-RIO installed on the target and/or host. Update the version
 * of NI-RIO on the target and/or host to the same version (or later)
 * used to compile the bitfile. Alternatively, recompile the bitfile
 * with the same version of NI-RIO that is currently installed on the
 * target and/or host.
 */
static const NiFpga_Status NiFpga_Status_IncompatibleBitfile = -63107;

/**
 * Either the supplied resource name is invalid as a RIO resource name, or the
 * device was not found. Use MAX to find the proper resource name for the
 * intended device.
 */
static const NiFpga_Status NiFpga_Status_InvalidResourceName = -63192;

/**
 * The requested feature is not supported.
 */
static const NiFpga_Status NiFpga_Status_FeatureNotSupported = -63193;

/**
 * The NI-RIO software on the target system is not compatible with this
 * software. Upgrade the NI-RIO software on the target system.
 */
static const NiFpga_Status NiFpga_Status_VersionMismatch = -63194;

/**
 * The session is invalid or has been closed.
 */
static const NiFpga_Status NiFpga_Status_InvalidSession = -63195;

/**
 * The maximum number of open FPGA sessions has been reached. Close some open
 * sessions.
 */
static const NiFpga_Status NiFpga_Status_OutOfHandles = -63198;
