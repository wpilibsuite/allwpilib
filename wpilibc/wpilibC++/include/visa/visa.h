/*---------------------------------------------------------------------------*/
/* Distributed by IVI Foundation Inc.                                        */
/* Contains National Instruments extensions.                                 */
/* Do not modify the contents of this file.                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Title   : VISA.H                                                          */
/* Date    : 10-09-2006                                                      */
/* Purpose : Include file for the VISA Library 4.0 specification             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* When using NI-VISA extensions, you must link with the VISA library that   */
/* comes with NI-VISA.  Currently, the extensions provided by NI-VISA are:   */
/*                                                                           */
/* PXI (Compact PCI eXtensions for Instrumentation) and PCI support.  To use */
/* this, you must define the macro NIVISA_PXI before including this header.  */
/* You must also create an INF file with the VISA Driver Development Wizard. */
/*                                                                           */
/* A fast set of macros for viPeekXX/viPokeXX that guarantees binary         */
/* compatibility with other implementations of VISA.  To use this, you must  */
/* define the macro NIVISA_PEEKPOKE before including this header.            */
/*                                                                           */
/* Support for USB devices that do not conform to a specific class.  To use  */
/* this, you must define the macro NIVISA_USB before including this header.  */
/* You must also create an INF file with the VISA Driver Development Wizard. */
/*---------------------------------------------------------------------------*/

#ifndef __VISA_HEADER__
#define __VISA_HEADER__

#include <stdarg.h>

#if !defined(__VISATYPE_HEADER__)
#include "visatype.h"
#endif

#define VI_SPEC_VERSION     (0x00400000UL)

#if defined(__cplusplus) || defined(__cplusplus__)
   extern "C" {
#endif

#if defined(_CVI_)
#pragma EnableLibraryRuntimeChecking
#endif

/*- VISA Types --------------------------------------------------------------*/

typedef ViObject             ViEvent;
typedef ViEvent      _VI_PTR ViPEvent;
typedef ViObject             ViFindList;
typedef ViFindList   _VI_PTR ViPFindList;

#if defined(_VI_INT64_UINT64_DEFINED) && defined(_VISA_ENV_IS_64_BIT)
typedef ViUInt64             ViBusAddress;
typedef ViUInt64             ViBusSize;
typedef ViUInt64             ViAttrState;
#else
typedef ViUInt32             ViBusAddress;
typedef ViUInt32             ViBusSize;
typedef ViUInt32             ViAttrState;
#endif

#if defined(_VI_INT64_UINT64_DEFINED)
typedef ViUInt64             ViBusAddress64;
typedef ViBusAddress64 _VI_PTR ViPBusAddress64;
#endif

typedef ViUInt32             ViEventType;
typedef ViEventType  _VI_PTR ViPEventType;
typedef ViEventType  _VI_PTR ViAEventType;
typedef void         _VI_PTR ViPAttrState;
typedef ViAttr       _VI_PTR ViPAttr;
typedef ViAttr       _VI_PTR ViAAttr;

typedef ViString             ViKeyId;
typedef ViPString            ViPKeyId;
typedef ViUInt32             ViJobId;
typedef ViJobId      _VI_PTR ViPJobId;
typedef ViUInt32             ViAccessMode;
typedef ViAccessMode _VI_PTR ViPAccessMode;
typedef ViBusAddress _VI_PTR ViPBusAddress;
typedef ViUInt32             ViEventFilter;

typedef va_list              ViVAList;

typedef ViStatus (_VI_FUNCH _VI_PTR ViHndlr)
   (ViSession vi, ViEventType eventType, ViEvent event, ViAddr userHandle);

/*- Resource Manager Functions and Operations -------------------------------*/

ViStatus _VI_FUNC  viOpenDefaultRM (ViPSession vi);
ViStatus _VI_FUNC  viFindRsrc      (ViSession sesn, ViString expr, ViPFindList vi,
                                    ViPUInt32 retCnt, ViChar _VI_FAR desc[]);
ViStatus _VI_FUNC  viFindNext      (ViFindList vi, ViChar _VI_FAR desc[]);
ViStatus _VI_FUNC  viParseRsrc     (ViSession rmSesn, ViRsrc rsrcName,
                                    ViPUInt16 intfType, ViPUInt16 intfNum);
ViStatus _VI_FUNC  viParseRsrcEx   (ViSession rmSesn, ViRsrc rsrcName, ViPUInt16 intfType,
                                    ViPUInt16 intfNum, ViChar _VI_FAR rsrcClass[],
                                    ViChar _VI_FAR expandedUnaliasedName[],
                                    ViChar _VI_FAR aliasIfExists[]);
ViStatus _VI_FUNC  viOpen          (ViSession sesn, ViRsrc name, ViAccessMode mode,
                                    ViUInt32 timeout, ViPSession vi);

/*- Resource Template Operations --------------------------------------------*/

ViStatus _VI_FUNC  viClose         (ViObject vi);
ViStatus _VI_FUNC  viSetAttribute  (ViObject vi, ViAttr attrName, ViAttrState attrValue);
ViStatus _VI_FUNC  viGetAttribute  (ViObject vi, ViAttr attrName, void _VI_PTR attrValue);
ViStatus _VI_FUNC  viStatusDesc    (ViObject vi, ViStatus status, ViChar _VI_FAR desc[]);
ViStatus _VI_FUNC  viTerminate     (ViObject vi, ViUInt16 degree, ViJobId jobId);

ViStatus _VI_FUNC  viLock          (ViSession vi, ViAccessMode lockType, ViUInt32 timeout,
                                    ViKeyId requestedKey, ViChar _VI_FAR accessKey[]);
ViStatus _VI_FUNC  viUnlock        (ViSession vi);
ViStatus _VI_FUNC  viEnableEvent   (ViSession vi, ViEventType eventType, ViUInt16 mechanism,
                                    ViEventFilter context);
ViStatus _VI_FUNC  viDisableEvent  (ViSession vi, ViEventType eventType, ViUInt16 mechanism);
ViStatus _VI_FUNC  viDiscardEvents (ViSession vi, ViEventType eventType, ViUInt16 mechanism);
ViStatus _VI_FUNC  viWaitOnEvent   (ViSession vi, ViEventType inEventType, ViUInt32 timeout,
                                    ViPEventType outEventType, ViPEvent outContext);
ViStatus _VI_FUNC  viInstallHandler(ViSession vi, ViEventType eventType, ViHndlr handler,
                                    ViAddr userHandle);
ViStatus _VI_FUNC  viUninstallHandler(ViSession vi, ViEventType eventType, ViHndlr handler,
                                      ViAddr userHandle);

/*- Basic I/O Operations ----------------------------------------------------*/

ViStatus _VI_FUNC  viRead          (ViSession vi, ViPBuf buf, ViUInt32 cnt, ViPUInt32 retCnt);
ViStatus _VI_FUNC  viReadAsync     (ViSession vi, ViPBuf buf, ViUInt32 cnt, ViPJobId  jobId);
ViStatus _VI_FUNC  viReadToFile    (ViSession vi, ViConstString filename, ViUInt32 cnt,
                                    ViPUInt32 retCnt);
ViStatus _VI_FUNC  viWrite         (ViSession vi, ViBuf  buf, ViUInt32 cnt, ViPUInt32 retCnt);
ViStatus _VI_FUNC  viWriteAsync    (ViSession vi, ViBuf  buf, ViUInt32 cnt, ViPJobId  jobId);
ViStatus _VI_FUNC  viWriteFromFile (ViSession vi, ViConstString filename, ViUInt32 cnt,
                                    ViPUInt32 retCnt);
ViStatus _VI_FUNC  viAssertTrigger (ViSession vi, ViUInt16 protocol);
ViStatus _VI_FUNC  viReadSTB       (ViSession vi, ViPUInt16 status);
ViStatus _VI_FUNC  viClear         (ViSession vi);

/*- Formatted and Buffered I/O Operations -----------------------------------*/

ViStatus _VI_FUNC  viSetBuf        (ViSession vi, ViUInt16 mask, ViUInt32 size);
ViStatus _VI_FUNC  viFlush         (ViSession vi, ViUInt16 mask);

ViStatus _VI_FUNC  viBufWrite      (ViSession vi, ViBuf  buf, ViUInt32 cnt, ViPUInt32 retCnt);
ViStatus _VI_FUNC  viBufRead       (ViSession vi, ViPBuf buf, ViUInt32 cnt, ViPUInt32 retCnt);

ViStatus _VI_FUNCC viPrintf        (ViSession vi, ViString writeFmt, ...);
ViStatus _VI_FUNC  viVPrintf       (ViSession vi, ViString writeFmt, ViVAList params);
ViStatus _VI_FUNCC viSPrintf       (ViSession vi, ViPBuf buf, ViString writeFmt, ...);
ViStatus _VI_FUNC  viVSPrintf      (ViSession vi, ViPBuf buf, ViString writeFmt,
                                    ViVAList parms);

ViStatus _VI_FUNCC viScanf         (ViSession vi, ViString readFmt, ...);
ViStatus _VI_FUNC  viVScanf        (ViSession vi, ViString readFmt, ViVAList params);
ViStatus _VI_FUNCC viSScanf        (ViSession vi, ViBuf buf, ViString readFmt, ...);
ViStatus _VI_FUNC  viVSScanf       (ViSession vi, ViBuf buf, ViString readFmt,
                                    ViVAList parms);

ViStatus _VI_FUNCC viQueryf        (ViSession vi, ViString writeFmt, ViString readFmt, ...);
ViStatus _VI_FUNC  viVQueryf       (ViSession vi, ViString writeFmt, ViString readFmt, 
                                    ViVAList params);

/*- Memory I/O Operations ---------------------------------------------------*/

ViStatus _VI_FUNC  viIn8           (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViPUInt8  val8);
ViStatus _VI_FUNC  viOut8          (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViUInt8   val8);
ViStatus _VI_FUNC  viIn16          (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViPUInt16 val16);
ViStatus _VI_FUNC  viOut16         (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViUInt16  val16);
ViStatus _VI_FUNC  viIn32          (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViPUInt32 val32);
ViStatus _VI_FUNC  viOut32         (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViUInt32  val32);

#if defined(_VI_INT64_UINT64_DEFINED)
ViStatus _VI_FUNC  viIn64          (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViPUInt64 val64);
ViStatus _VI_FUNC  viOut64         (ViSession vi, ViUInt16 space,
                                    ViBusAddress offset, ViUInt64  val64);

ViStatus _VI_FUNC  viIn8Ex         (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViPUInt8  val8);
ViStatus _VI_FUNC  viOut8Ex        (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViUInt8   val8);
ViStatus _VI_FUNC  viIn16Ex        (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViPUInt16 val16);
ViStatus _VI_FUNC  viOut16Ex       (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViUInt16  val16);
ViStatus _VI_FUNC  viIn32Ex        (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViPUInt32 val32);
ViStatus _VI_FUNC  viOut32Ex       (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViUInt32  val32);
ViStatus _VI_FUNC  viIn64Ex        (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViPUInt64 val64);
ViStatus _VI_FUNC  viOut64Ex       (ViSession vi, ViUInt16 space,
                                    ViBusAddress64 offset, ViUInt64  val64);
#endif

ViStatus _VI_FUNC  viMoveIn8       (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt8  buf8);
ViStatus _VI_FUNC  viMoveOut8      (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt8  buf8);
ViStatus _VI_FUNC  viMoveIn16      (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt16 buf16);
ViStatus _VI_FUNC  viMoveOut16     (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt16 buf16);
ViStatus _VI_FUNC  viMoveIn32      (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt32 buf32);
ViStatus _VI_FUNC  viMoveOut32     (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt32 buf32);

#if defined(_VI_INT64_UINT64_DEFINED)
ViStatus _VI_FUNC  viMoveIn64      (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt64 buf64);
ViStatus _VI_FUNC  viMoveOut64     (ViSession vi, ViUInt16 space, ViBusAddress offset,
                                    ViBusSize length, ViAUInt64 buf64);

ViStatus _VI_FUNC  viMoveIn8Ex     (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt8  buf8);
ViStatus _VI_FUNC  viMoveOut8Ex    (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt8  buf8);
ViStatus _VI_FUNC  viMoveIn16Ex    (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt16 buf16);
ViStatus _VI_FUNC  viMoveOut16Ex   (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt16 buf16);
ViStatus _VI_FUNC  viMoveIn32Ex    (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt32 buf32);
ViStatus _VI_FUNC  viMoveOut32Ex   (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt32 buf32);
ViStatus _VI_FUNC  viMoveIn64Ex    (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt64 buf64);
ViStatus _VI_FUNC  viMoveOut64Ex   (ViSession vi, ViUInt16 space, ViBusAddress64 offset,
                                    ViBusSize length, ViAUInt64 buf64);
#endif

ViStatus _VI_FUNC  viMove          (ViSession vi, ViUInt16 srcSpace, ViBusAddress srcOffset,
                                    ViUInt16 srcWidth, ViUInt16 destSpace, 
                                    ViBusAddress destOffset, ViUInt16 destWidth, 
                                    ViBusSize srcLength); 
ViStatus _VI_FUNC  viMoveAsync     (ViSession vi, ViUInt16 srcSpace, ViBusAddress srcOffset,
                                    ViUInt16 srcWidth, ViUInt16 destSpace, 
                                    ViBusAddress destOffset, ViUInt16 destWidth, 
                                    ViBusSize srcLength, ViPJobId jobId);

#if defined(_VI_INT64_UINT64_DEFINED)
ViStatus _VI_FUNC  viMoveEx        (ViSession vi, ViUInt16 srcSpace, ViBusAddress64 srcOffset,
                                    ViUInt16 srcWidth, ViUInt16 destSpace, 
                                    ViBusAddress64 destOffset, ViUInt16 destWidth, 
                                    ViBusSize srcLength); 
ViStatus _VI_FUNC  viMoveAsyncEx   (ViSession vi, ViUInt16 srcSpace, ViBusAddress64 srcOffset,
                                    ViUInt16 srcWidth, ViUInt16 destSpace, 
                                    ViBusAddress64 destOffset, ViUInt16 destWidth, 
                                    ViBusSize srcLength, ViPJobId jobId);
#endif

ViStatus _VI_FUNC  viMapAddress    (ViSession vi, ViUInt16 mapSpace, ViBusAddress mapOffset,
                                    ViBusSize mapSize, ViBoolean access,
                                    ViAddr suggested, ViPAddr address);
ViStatus _VI_FUNC  viUnmapAddress  (ViSession vi);

#if defined(_VI_INT64_UINT64_DEFINED)
ViStatus _VI_FUNC  viMapAddressEx  (ViSession vi, ViUInt16 mapSpace, ViBusAddress64 mapOffset,
                                    ViBusSize mapSize, ViBoolean access,
                                    ViAddr suggested, ViPAddr address);
#endif

void     _VI_FUNC  viPeek8         (ViSession vi, ViAddr address, ViPUInt8  val8);
void     _VI_FUNC  viPoke8         (ViSession vi, ViAddr address, ViUInt8   val8);
void     _VI_FUNC  viPeek16        (ViSession vi, ViAddr address, ViPUInt16 val16);
void     _VI_FUNC  viPoke16        (ViSession vi, ViAddr address, ViUInt16  val16);
void     _VI_FUNC  viPeek32        (ViSession vi, ViAddr address, ViPUInt32 val32);
void     _VI_FUNC  viPoke32        (ViSession vi, ViAddr address, ViUInt32  val32);

#if defined(_VI_INT64_UINT64_DEFINED)
void     _VI_FUNC  viPeek64        (ViSession vi, ViAddr address, ViPUInt64 val64);
void     _VI_FUNC  viPoke64        (ViSession vi, ViAddr address, ViUInt64  val64);
#endif

/*- Shared Memory Operations ------------------------------------------------*/

ViStatus _VI_FUNC  viMemAlloc      (ViSession vi, ViBusSize size, ViPBusAddress offset);
ViStatus _VI_FUNC  viMemFree       (ViSession vi, ViBusAddress offset);

#if defined(_VI_INT64_UINT64_DEFINED)
ViStatus _VI_FUNC  viMemAllocEx    (ViSession vi, ViBusSize size, ViPBusAddress64 offset);
ViStatus _VI_FUNC  viMemFreeEx     (ViSession vi, ViBusAddress64 offset);
#endif

/*- Interface Specific Operations -------------------------------------------*/

ViStatus _VI_FUNC  viGpibControlREN(ViSession vi, ViUInt16 mode);
ViStatus _VI_FUNC  viGpibControlATN(ViSession vi, ViUInt16 mode);
ViStatus _VI_FUNC  viGpibSendIFC   (ViSession vi);
ViStatus _VI_FUNC  viGpibCommand   (ViSession vi, ViBuf cmd, ViUInt32 cnt, ViPUInt32 retCnt);
ViStatus _VI_FUNC  viGpibPassControl(ViSession vi, ViUInt16 primAddr, ViUInt16 secAddr);

ViStatus _VI_FUNC  viVxiCommandQuery(ViSession vi, ViUInt16 mode, ViUInt32 cmd,
                                     ViPUInt32 response);
ViStatus _VI_FUNC  viAssertUtilSignal(ViSession vi, ViUInt16 line);
ViStatus _VI_FUNC  viAssertIntrSignal(ViSession vi, ViInt16 mode, ViUInt32 statusID);
ViStatus _VI_FUNC  viMapTrigger    (ViSession vi, ViInt16 trigSrc, ViInt16 trigDest, 
                                    ViUInt16 mode);
ViStatus _VI_FUNC  viUnmapTrigger  (ViSession vi, ViInt16 trigSrc, ViInt16 trigDest);
ViStatus _VI_FUNC  viUsbControlOut (ViSession vi, ViInt16 bmRequestType, ViInt16 bRequest,
                                    ViUInt16 wValue, ViUInt16 wIndex, ViUInt16 wLength,
                                    ViBuf buf);
ViStatus _VI_FUNC  viUsbControlIn  (ViSession vi, ViInt16 bmRequestType, ViInt16 bRequest,
                                    ViUInt16 wValue, ViUInt16 wIndex, ViUInt16 wLength,
                                    ViPBuf buf, ViPUInt16 retCnt);

/*- Attributes (platform independent size) ----------------------------------*/

#define VI_ATTR_RSRC_CLASS          (0xBFFF0001UL)
#define VI_ATTR_RSRC_NAME           (0xBFFF0002UL)
#define VI_ATTR_RSRC_IMPL_VERSION   (0x3FFF0003UL)
#define VI_ATTR_RSRC_LOCK_STATE     (0x3FFF0004UL)
#define VI_ATTR_MAX_QUEUE_LENGTH    (0x3FFF0005UL)
#define VI_ATTR_USER_DATA_32        (0x3FFF0007UL)
#define VI_ATTR_FDC_CHNL            (0x3FFF000DUL)
#define VI_ATTR_FDC_MODE            (0x3FFF000FUL)
#define VI_ATTR_FDC_GEN_SIGNAL_EN   (0x3FFF0011UL)
#define VI_ATTR_FDC_USE_PAIR        (0x3FFF0013UL)
#define VI_ATTR_SEND_END_EN         (0x3FFF0016UL)
#define VI_ATTR_TERMCHAR            (0x3FFF0018UL)
#define VI_ATTR_TMO_VALUE           (0x3FFF001AUL)
#define VI_ATTR_GPIB_READDR_EN      (0x3FFF001BUL)
#define VI_ATTR_IO_PROT             (0x3FFF001CUL)
#define VI_ATTR_DMA_ALLOW_EN        (0x3FFF001EUL)
#define VI_ATTR_ASRL_BAUD           (0x3FFF0021UL)
#define VI_ATTR_ASRL_DATA_BITS      (0x3FFF0022UL)
#define VI_ATTR_ASRL_PARITY         (0x3FFF0023UL)
#define VI_ATTR_ASRL_STOP_BITS      (0x3FFF0024UL)
#define VI_ATTR_ASRL_FLOW_CNTRL     (0x3FFF0025UL)
#define VI_ATTR_RD_BUF_OPER_MODE    (0x3FFF002AUL)
#define VI_ATTR_RD_BUF_SIZE         (0x3FFF002BUL)
#define VI_ATTR_WR_BUF_OPER_MODE    (0x3FFF002DUL)
#define VI_ATTR_WR_BUF_SIZE         (0x3FFF002EUL)
#define VI_ATTR_SUPPRESS_END_EN     (0x3FFF0036UL)
#define VI_ATTR_TERMCHAR_EN         (0x3FFF0038UL)
#define VI_ATTR_DEST_ACCESS_PRIV    (0x3FFF0039UL)
#define VI_ATTR_DEST_BYTE_ORDER     (0x3FFF003AUL)
#define VI_ATTR_SRC_ACCESS_PRIV     (0x3FFF003CUL)
#define VI_ATTR_SRC_BYTE_ORDER      (0x3FFF003DUL)
#define VI_ATTR_SRC_INCREMENT       (0x3FFF0040UL)
#define VI_ATTR_DEST_INCREMENT      (0x3FFF0041UL)
#define VI_ATTR_WIN_ACCESS_PRIV     (0x3FFF0045UL)
#define VI_ATTR_WIN_BYTE_ORDER      (0x3FFF0047UL)
#define VI_ATTR_GPIB_ATN_STATE      (0x3FFF0057UL)
#define VI_ATTR_GPIB_ADDR_STATE     (0x3FFF005CUL)
#define VI_ATTR_GPIB_CIC_STATE      (0x3FFF005EUL)
#define VI_ATTR_GPIB_NDAC_STATE     (0x3FFF0062UL)
#define VI_ATTR_GPIB_SRQ_STATE      (0x3FFF0067UL)
#define VI_ATTR_GPIB_SYS_CNTRL_STATE (0x3FFF0068UL)
#define VI_ATTR_GPIB_HS488_CBL_LEN  (0x3FFF0069UL)
#define VI_ATTR_CMDR_LA             (0x3FFF006BUL)
#define VI_ATTR_VXI_DEV_CLASS       (0x3FFF006CUL)
#define VI_ATTR_MAINFRAME_LA        (0x3FFF0070UL)
#define VI_ATTR_MANF_NAME           (0xBFFF0072UL)
#define VI_ATTR_MODEL_NAME          (0xBFFF0077UL)
#define VI_ATTR_VXI_VME_INTR_STATUS (0x3FFF008BUL)
#define VI_ATTR_VXI_TRIG_STATUS     (0x3FFF008DUL)
#define VI_ATTR_VXI_VME_SYSFAIL_STATE (0x3FFF0094UL)
#define VI_ATTR_WIN_BASE_ADDR_32    (0x3FFF0098UL)
#define VI_ATTR_WIN_SIZE_32         (0x3FFF009AUL)
#define VI_ATTR_ASRL_AVAIL_NUM      (0x3FFF00ACUL)
#define VI_ATTR_MEM_BASE_32         (0x3FFF00ADUL)
#define VI_ATTR_ASRL_CTS_STATE      (0x3FFF00AEUL)
#define VI_ATTR_ASRL_DCD_STATE      (0x3FFF00AFUL)
#define VI_ATTR_ASRL_DSR_STATE      (0x3FFF00B1UL)
#define VI_ATTR_ASRL_DTR_STATE      (0x3FFF00B2UL)
#define VI_ATTR_ASRL_END_IN         (0x3FFF00B3UL)
#define VI_ATTR_ASRL_END_OUT        (0x3FFF00B4UL)
#define VI_ATTR_ASRL_REPLACE_CHAR   (0x3FFF00BEUL)
#define VI_ATTR_ASRL_RI_STATE       (0x3FFF00BFUL)
#define VI_ATTR_ASRL_RTS_STATE      (0x3FFF00C0UL)
#define VI_ATTR_ASRL_XON_CHAR       (0x3FFF00C1UL)
#define VI_ATTR_ASRL_XOFF_CHAR      (0x3FFF00C2UL)
#define VI_ATTR_WIN_ACCESS          (0x3FFF00C3UL)
#define VI_ATTR_RM_SESSION          (0x3FFF00C4UL)
#define VI_ATTR_VXI_LA              (0x3FFF00D5UL)
#define VI_ATTR_MANF_ID             (0x3FFF00D9UL)
#define VI_ATTR_MEM_SIZE_32         (0x3FFF00DDUL)
#define VI_ATTR_MEM_SPACE           (0x3FFF00DEUL)
#define VI_ATTR_MODEL_CODE          (0x3FFF00DFUL)
#define VI_ATTR_SLOT                (0x3FFF00E8UL)
#define VI_ATTR_INTF_INST_NAME      (0xBFFF00E9UL)
#define VI_ATTR_IMMEDIATE_SERV      (0x3FFF0100UL)
#define VI_ATTR_INTF_PARENT_NUM     (0x3FFF0101UL)
#define VI_ATTR_RSRC_SPEC_VERSION   (0x3FFF0170UL)
#define VI_ATTR_INTF_TYPE           (0x3FFF0171UL)
#define VI_ATTR_GPIB_PRIMARY_ADDR   (0x3FFF0172UL)
#define VI_ATTR_GPIB_SECONDARY_ADDR (0x3FFF0173UL)
#define VI_ATTR_RSRC_MANF_NAME      (0xBFFF0174UL)
#define VI_ATTR_RSRC_MANF_ID        (0x3FFF0175UL)
#define VI_ATTR_INTF_NUM            (0x3FFF0176UL)
#define VI_ATTR_TRIG_ID             (0x3FFF0177UL)
#define VI_ATTR_GPIB_REN_STATE      (0x3FFF0181UL)
#define VI_ATTR_GPIB_UNADDR_EN      (0x3FFF0184UL)
#define VI_ATTR_DEV_STATUS_BYTE     (0x3FFF0189UL)
#define VI_ATTR_FILE_APPEND_EN      (0x3FFF0192UL)
#define VI_ATTR_VXI_TRIG_SUPPORT    (0x3FFF0194UL)
#define VI_ATTR_TCPIP_ADDR          (0xBFFF0195UL)
#define VI_ATTR_TCPIP_HOSTNAME      (0xBFFF0196UL)
#define VI_ATTR_TCPIP_PORT          (0x3FFF0197UL)
#define VI_ATTR_TCPIP_DEVICE_NAME   (0xBFFF0199UL)
#define VI_ATTR_TCPIP_NODELAY       (0x3FFF019AUL)
#define VI_ATTR_TCPIP_KEEPALIVE     (0x3FFF019BUL)
#define VI_ATTR_4882_COMPLIANT      (0x3FFF019FUL)
#define VI_ATTR_USB_SERIAL_NUM      (0xBFFF01A0UL)
#define VI_ATTR_USB_INTFC_NUM       (0x3FFF01A1UL)
#define VI_ATTR_USB_PROTOCOL        (0x3FFF01A7UL)
#define VI_ATTR_USB_MAX_INTR_SIZE   (0x3FFF01AFUL)
#define VI_ATTR_PXI_DEV_NUM         (0x3FFF0201UL)
#define VI_ATTR_PXI_FUNC_NUM        (0x3FFF0202UL)
#define VI_ATTR_PXI_BUS_NUM         (0x3FFF0205UL)
#define VI_ATTR_PXI_CHASSIS         (0x3FFF0206UL)
#define VI_ATTR_PXI_SLOTPATH        (0xBFFF0207UL)
#define VI_ATTR_PXI_SLOT_LBUS_LEFT  (0x3FFF0208UL)
#define VI_ATTR_PXI_SLOT_LBUS_RIGHT (0x3FFF0209UL)
#define VI_ATTR_PXI_TRIG_BUS        (0x3FFF020AUL)
#define VI_ATTR_PXI_STAR_TRIG_BUS   (0x3FFF020BUL)
#define VI_ATTR_PXI_STAR_TRIG_LINE  (0x3FFF020CUL)
#define VI_ATTR_PXI_MEM_TYPE_BAR0   (0x3FFF0211UL)
#define VI_ATTR_PXI_MEM_TYPE_BAR1   (0x3FFF0212UL)
#define VI_ATTR_PXI_MEM_TYPE_BAR2   (0x3FFF0213UL)
#define VI_ATTR_PXI_MEM_TYPE_BAR3   (0x3FFF0214UL)
#define VI_ATTR_PXI_MEM_TYPE_BAR4   (0x3FFF0215UL)
#define VI_ATTR_PXI_MEM_TYPE_BAR5   (0x3FFF0216UL)
#define VI_ATTR_PXI_MEM_BASE_BAR0   (0x3FFF0221UL)
#define VI_ATTR_PXI_MEM_BASE_BAR1   (0x3FFF0222UL)
#define VI_ATTR_PXI_MEM_BASE_BAR2   (0x3FFF0223UL)
#define VI_ATTR_PXI_MEM_BASE_BAR3   (0x3FFF0224UL)
#define VI_ATTR_PXI_MEM_BASE_BAR4   (0x3FFF0225UL)
#define VI_ATTR_PXI_MEM_BASE_BAR5   (0x3FFF0226UL)
#define VI_ATTR_PXI_MEM_SIZE_BAR0   (0x3FFF0231UL)
#define VI_ATTR_PXI_MEM_SIZE_BAR1   (0x3FFF0232UL)
#define VI_ATTR_PXI_MEM_SIZE_BAR2   (0x3FFF0233UL)
#define VI_ATTR_PXI_MEM_SIZE_BAR3   (0x3FFF0234UL)
#define VI_ATTR_PXI_MEM_SIZE_BAR4   (0x3FFF0235UL)
#define VI_ATTR_PXI_MEM_SIZE_BAR5   (0x3FFF0236UL)
#define VI_ATTR_PXI_IS_EXPRESS      (0x3FFF0240UL)
#define VI_ATTR_PXI_SLOT_LWIDTH     (0x3FFF0241UL)
#define VI_ATTR_PXI_MAX_LWIDTH      (0x3FFF0242UL)
#define VI_ATTR_PXI_ACTUAL_LWIDTH   (0x3FFF0243UL)
#define VI_ATTR_PXI_DSTAR_BUS       (0x3FFF0244UL)
#define VI_ATTR_PXI_DSTAR_SET       (0x3FFF0245UL)

#define VI_ATTR_JOB_ID              (0x3FFF4006UL)
#define VI_ATTR_EVENT_TYPE          (0x3FFF4010UL)
#define VI_ATTR_SIGP_STATUS_ID      (0x3FFF4011UL)
#define VI_ATTR_RECV_TRIG_ID        (0x3FFF4012UL)
#define VI_ATTR_INTR_STATUS_ID      (0x3FFF4023UL)
#define VI_ATTR_STATUS              (0x3FFF4025UL)
#define VI_ATTR_RET_COUNT_32        (0x3FFF4026UL)
#define VI_ATTR_BUFFER              (0x3FFF4027UL)
#define VI_ATTR_RECV_INTR_LEVEL     (0x3FFF4041UL)
#define VI_ATTR_OPER_NAME           (0xBFFF4042UL)
#define VI_ATTR_GPIB_RECV_CIC_STATE (0x3FFF4193UL)
#define VI_ATTR_RECV_TCPIP_ADDR     (0xBFFF4198UL)
#define VI_ATTR_USB_RECV_INTR_SIZE  (0x3FFF41B0UL)
#define VI_ATTR_USB_RECV_INTR_DATA  (0xBFFF41B1UL)

/*- Attributes (platform dependent size) ------------------------------------*/

#if defined(_VI_INT64_UINT64_DEFINED) && defined(_VISA_ENV_IS_64_BIT)
#define VI_ATTR_USER_DATA_64        (0x3FFF000AUL)
#define VI_ATTR_RET_COUNT_64        (0x3FFF4028UL)
#define VI_ATTR_USER_DATA           (VI_ATTR_USER_DATA_64)
#define VI_ATTR_RET_COUNT           (VI_ATTR_RET_COUNT_64)
#else
#define VI_ATTR_USER_DATA           (VI_ATTR_USER_DATA_32)
#define VI_ATTR_RET_COUNT           (VI_ATTR_RET_COUNT_32)
#endif

#if defined(_VI_INT64_UINT64_DEFINED)
#define VI_ATTR_WIN_BASE_ADDR_64    (0x3FFF009BUL)
#define VI_ATTR_WIN_SIZE_64         (0x3FFF009CUL)
#define VI_ATTR_MEM_BASE_64         (0x3FFF00D0UL)
#define VI_ATTR_MEM_SIZE_64         (0x3FFF00D1UL)
#endif
#if defined(_VI_INT64_UINT64_DEFINED) && defined(_VISA_ENV_IS_64_BIT)
#define VI_ATTR_WIN_BASE_ADDR       (VI_ATTR_WIN_BASE_ADDR_64)
#define VI_ATTR_WIN_SIZE            (VI_ATTR_WIN_SIZE_64)
#define VI_ATTR_MEM_BASE            (VI_ATTR_MEM_BASE_64)
#define VI_ATTR_MEM_SIZE            (VI_ATTR_MEM_SIZE_64)
#else
#define VI_ATTR_WIN_BASE_ADDR       (VI_ATTR_WIN_BASE_ADDR_32)
#define VI_ATTR_WIN_SIZE            (VI_ATTR_WIN_SIZE_32)
#define VI_ATTR_MEM_BASE            (VI_ATTR_MEM_BASE_32)
#define VI_ATTR_MEM_SIZE            (VI_ATTR_MEM_SIZE_32)
#endif

/*- Event Types -------------------------------------------------------------*/

#define VI_EVENT_IO_COMPLETION      (0x3FFF2009UL)
#define VI_EVENT_TRIG               (0xBFFF200AUL)
#define VI_EVENT_SERVICE_REQ        (0x3FFF200BUL)
#define VI_EVENT_CLEAR              (0x3FFF200DUL)
#define VI_EVENT_EXCEPTION          (0xBFFF200EUL)
#define VI_EVENT_GPIB_CIC           (0x3FFF2012UL)
#define VI_EVENT_GPIB_TALK          (0x3FFF2013UL)
#define VI_EVENT_GPIB_LISTEN        (0x3FFF2014UL)
#define VI_EVENT_VXI_VME_SYSFAIL    (0x3FFF201DUL)
#define VI_EVENT_VXI_VME_SYSRESET   (0x3FFF201EUL)
#define VI_EVENT_VXI_SIGP           (0x3FFF2020UL)
#define VI_EVENT_VXI_VME_INTR       (0xBFFF2021UL)
#define VI_EVENT_PXI_INTR           (0x3FFF2022UL)
#define VI_EVENT_TCPIP_CONNECT      (0x3FFF2036UL)
#define VI_EVENT_USB_INTR           (0x3FFF2037UL)

#define VI_ALL_ENABLED_EVENTS       (0x3FFF7FFFUL)

/*- Completion and Error Codes ----------------------------------------------*/

#define VI_SUCCESS_EVENT_EN                   (0x3FFF0002L) /* 3FFF0002,  1073676290 */
#define VI_SUCCESS_EVENT_DIS                  (0x3FFF0003L) /* 3FFF0003,  1073676291 */
#define VI_SUCCESS_QUEUE_EMPTY                (0x3FFF0004L) /* 3FFF0004,  1073676292 */
#define VI_SUCCESS_TERM_CHAR                  (0x3FFF0005L) /* 3FFF0005,  1073676293 */
#define VI_SUCCESS_MAX_CNT                    (0x3FFF0006L) /* 3FFF0006,  1073676294 */
#define VI_SUCCESS_DEV_NPRESENT               (0x3FFF007DL) /* 3FFF007D,  1073676413 */
#define VI_SUCCESS_TRIG_MAPPED                (0x3FFF007EL) /* 3FFF007E,  1073676414 */
#define VI_SUCCESS_QUEUE_NEMPTY               (0x3FFF0080L) /* 3FFF0080,  1073676416 */
#define VI_SUCCESS_NCHAIN                     (0x3FFF0098L) /* 3FFF0098,  1073676440 */
#define VI_SUCCESS_NESTED_SHARED              (0x3FFF0099L) /* 3FFF0099,  1073676441 */
#define VI_SUCCESS_NESTED_EXCLUSIVE           (0x3FFF009AL) /* 3FFF009A,  1073676442 */
#define VI_SUCCESS_SYNC                       (0x3FFF009BL) /* 3FFF009B,  1073676443 */

#define VI_WARN_QUEUE_OVERFLOW                (0x3FFF000CL) /* 3FFF000C,  1073676300 */
#define VI_WARN_CONFIG_NLOADED                (0x3FFF0077L) /* 3FFF0077,  1073676407 */
#define VI_WARN_NULL_OBJECT                   (0x3FFF0082L) /* 3FFF0082,  1073676418 */
#define VI_WARN_NSUP_ATTR_STATE               (0x3FFF0084L) /* 3FFF0084,  1073676420 */
#define VI_WARN_UNKNOWN_STATUS                (0x3FFF0085L) /* 3FFF0085,  1073676421 */
#define VI_WARN_NSUP_BUF                      (0x3FFF0088L) /* 3FFF0088,  1073676424 */
#define VI_WARN_EXT_FUNC_NIMPL                (0x3FFF00A9L) /* 3FFF00A9,  1073676457 */

#define VI_ERROR_SYSTEM_ERROR       (_VI_ERROR+0x3FFF0000L) /* BFFF0000, -1073807360 */
#define VI_ERROR_INV_OBJECT         (_VI_ERROR+0x3FFF000EL) /* BFFF000E, -1073807346 */
#define VI_ERROR_RSRC_LOCKED        (_VI_ERROR+0x3FFF000FL) /* BFFF000F, -1073807345 */
#define VI_ERROR_INV_EXPR           (_VI_ERROR+0x3FFF0010L) /* BFFF0010, -1073807344 */
#define VI_ERROR_RSRC_NFOUND        (_VI_ERROR+0x3FFF0011L) /* BFFF0011, -1073807343 */
#define VI_ERROR_INV_RSRC_NAME      (_VI_ERROR+0x3FFF0012L) /* BFFF0012, -1073807342 */
#define VI_ERROR_INV_ACC_MODE       (_VI_ERROR+0x3FFF0013L) /* BFFF0013, -1073807341 */
#define VI_ERROR_TMO                (_VI_ERROR+0x3FFF0015L) /* BFFF0015, -1073807339 */
#define VI_ERROR_CLOSING_FAILED     (_VI_ERROR+0x3FFF0016L) /* BFFF0016, -1073807338 */
#define VI_ERROR_INV_DEGREE         (_VI_ERROR+0x3FFF001BL) /* BFFF001B, -1073807333 */
#define VI_ERROR_INV_JOB_ID         (_VI_ERROR+0x3FFF001CL) /* BFFF001C, -1073807332 */
#define VI_ERROR_NSUP_ATTR          (_VI_ERROR+0x3FFF001DL) /* BFFF001D, -1073807331 */
#define VI_ERROR_NSUP_ATTR_STATE    (_VI_ERROR+0x3FFF001EL) /* BFFF001E, -1073807330 */
#define VI_ERROR_ATTR_READONLY      (_VI_ERROR+0x3FFF001FL) /* BFFF001F, -1073807329 */
#define VI_ERROR_INV_LOCK_TYPE      (_VI_ERROR+0x3FFF0020L) /* BFFF0020, -1073807328 */
#define VI_ERROR_INV_ACCESS_KEY     (_VI_ERROR+0x3FFF0021L) /* BFFF0021, -1073807327 */
#define VI_ERROR_INV_EVENT          (_VI_ERROR+0x3FFF0026L) /* BFFF0026, -1073807322 */
#define VI_ERROR_INV_MECH           (_VI_ERROR+0x3FFF0027L) /* BFFF0027, -1073807321 */
#define VI_ERROR_HNDLR_NINSTALLED   (_VI_ERROR+0x3FFF0028L) /* BFFF0028, -1073807320 */
#define VI_ERROR_INV_HNDLR_REF      (_VI_ERROR+0x3FFF0029L) /* BFFF0029, -1073807319 */
#define VI_ERROR_INV_CONTEXT        (_VI_ERROR+0x3FFF002AL) /* BFFF002A, -1073807318 */
#define VI_ERROR_QUEUE_OVERFLOW     (_VI_ERROR+0x3FFF002DL) /* BFFF002D, -1073807315 */
#define VI_ERROR_NENABLED           (_VI_ERROR+0x3FFF002FL) /* BFFF002F, -1073807313 */
#define VI_ERROR_ABORT              (_VI_ERROR+0x3FFF0030L) /* BFFF0030, -1073807312 */
#define VI_ERROR_RAW_WR_PROT_VIOL   (_VI_ERROR+0x3FFF0034L) /* BFFF0034, -1073807308 */
#define VI_ERROR_RAW_RD_PROT_VIOL   (_VI_ERROR+0x3FFF0035L) /* BFFF0035, -1073807307 */
#define VI_ERROR_OUTP_PROT_VIOL     (_VI_ERROR+0x3FFF0036L) /* BFFF0036, -1073807306 */
#define VI_ERROR_INP_PROT_VIOL      (_VI_ERROR+0x3FFF0037L) /* BFFF0037, -1073807305 */
#define VI_ERROR_BERR               (_VI_ERROR+0x3FFF0038L) /* BFFF0038, -1073807304 */
#define VI_ERROR_IN_PROGRESS        (_VI_ERROR+0x3FFF0039L) /* BFFF0039, -1073807303 */
#define VI_ERROR_INV_SETUP          (_VI_ERROR+0x3FFF003AL) /* BFFF003A, -1073807302 */
#define VI_ERROR_QUEUE_ERROR        (_VI_ERROR+0x3FFF003BL) /* BFFF003B, -1073807301 */
#define VI_ERROR_ALLOC              (_VI_ERROR+0x3FFF003CL) /* BFFF003C, -1073807300 */
#define VI_ERROR_INV_MASK           (_VI_ERROR+0x3FFF003DL) /* BFFF003D, -1073807299 */
#define VI_ERROR_IO                 (_VI_ERROR+0x3FFF003EL) /* BFFF003E, -1073807298 */
#define VI_ERROR_INV_FMT            (_VI_ERROR+0x3FFF003FL) /* BFFF003F, -1073807297 */
#define VI_ERROR_NSUP_FMT           (_VI_ERROR+0x3FFF0041L) /* BFFF0041, -1073807295 */
#define VI_ERROR_LINE_IN_USE        (_VI_ERROR+0x3FFF0042L) /* BFFF0042, -1073807294 */
#define VI_ERROR_NSUP_MODE          (_VI_ERROR+0x3FFF0046L) /* BFFF0046, -1073807290 */
#define VI_ERROR_SRQ_NOCCURRED      (_VI_ERROR+0x3FFF004AL) /* BFFF004A, -1073807286 */
#define VI_ERROR_INV_SPACE          (_VI_ERROR+0x3FFF004EL) /* BFFF004E, -1073807282 */
#define VI_ERROR_INV_OFFSET         (_VI_ERROR+0x3FFF0051L) /* BFFF0051, -1073807279 */
#define VI_ERROR_INV_WIDTH          (_VI_ERROR+0x3FFF0052L) /* BFFF0052, -1073807278 */
#define VI_ERROR_NSUP_OFFSET        (_VI_ERROR+0x3FFF0054L) /* BFFF0054, -1073807276 */
#define VI_ERROR_NSUP_VAR_WIDTH     (_VI_ERROR+0x3FFF0055L) /* BFFF0055, -1073807275 */
#define VI_ERROR_WINDOW_NMAPPED     (_VI_ERROR+0x3FFF0057L) /* BFFF0057, -1073807273 */
#define VI_ERROR_RESP_PENDING       (_VI_ERROR+0x3FFF0059L) /* BFFF0059, -1073807271 */
#define VI_ERROR_NLISTENERS         (_VI_ERROR+0x3FFF005FL) /* BFFF005F, -1073807265 */
#define VI_ERROR_NCIC               (_VI_ERROR+0x3FFF0060L) /* BFFF0060, -1073807264 */
#define VI_ERROR_NSYS_CNTLR         (_VI_ERROR+0x3FFF0061L) /* BFFF0061, -1073807263 */
#define VI_ERROR_NSUP_OPER          (_VI_ERROR+0x3FFF0067L) /* BFFF0067, -1073807257 */
#define VI_ERROR_INTR_PENDING       (_VI_ERROR+0x3FFF0068L) /* BFFF0068, -1073807256 */
#define VI_ERROR_ASRL_PARITY        (_VI_ERROR+0x3FFF006AL) /* BFFF006A, -1073807254 */
#define VI_ERROR_ASRL_FRAMING       (_VI_ERROR+0x3FFF006BL) /* BFFF006B, -1073807253 */
#define VI_ERROR_ASRL_OVERRUN       (_VI_ERROR+0x3FFF006CL) /* BFFF006C, -1073807252 */
#define VI_ERROR_TRIG_NMAPPED       (_VI_ERROR+0x3FFF006EL) /* BFFF006E, -1073807250 */
#define VI_ERROR_NSUP_ALIGN_OFFSET  (_VI_ERROR+0x3FFF0070L) /* BFFF0070, -1073807248 */
#define VI_ERROR_USER_BUF           (_VI_ERROR+0x3FFF0071L) /* BFFF0071, -1073807247 */
#define VI_ERROR_RSRC_BUSY          (_VI_ERROR+0x3FFF0072L) /* BFFF0072, -1073807246 */
#define VI_ERROR_NSUP_WIDTH         (_VI_ERROR+0x3FFF0076L) /* BFFF0076, -1073807242 */
#define VI_ERROR_INV_PARAMETER      (_VI_ERROR+0x3FFF0078L) /* BFFF0078, -1073807240 */
#define VI_ERROR_INV_PROT           (_VI_ERROR+0x3FFF0079L) /* BFFF0079, -1073807239 */
#define VI_ERROR_INV_SIZE           (_VI_ERROR+0x3FFF007BL) /* BFFF007B, -1073807237 */
#define VI_ERROR_WINDOW_MAPPED      (_VI_ERROR+0x3FFF0080L) /* BFFF0080, -1073807232 */
#define VI_ERROR_NIMPL_OPER         (_VI_ERROR+0x3FFF0081L) /* BFFF0081, -1073807231 */
#define VI_ERROR_INV_LENGTH         (_VI_ERROR+0x3FFF0083L) /* BFFF0083, -1073807229 */
#define VI_ERROR_INV_MODE           (_VI_ERROR+0x3FFF0091L) /* BFFF0091, -1073807215 */
#define VI_ERROR_SESN_NLOCKED       (_VI_ERROR+0x3FFF009CL) /* BFFF009C, -1073807204 */
#define VI_ERROR_MEM_NSHARED        (_VI_ERROR+0x3FFF009DL) /* BFFF009D, -1073807203 */
#define VI_ERROR_LIBRARY_NFOUND     (_VI_ERROR+0x3FFF009EL) /* BFFF009E, -1073807202 */
#define VI_ERROR_NSUP_INTR          (_VI_ERROR+0x3FFF009FL) /* BFFF009F, -1073807201 */
#define VI_ERROR_INV_LINE           (_VI_ERROR+0x3FFF00A0L) /* BFFF00A0, -1073807200 */
#define VI_ERROR_FILE_ACCESS        (_VI_ERROR+0x3FFF00A1L) /* BFFF00A1, -1073807199 */
#define VI_ERROR_FILE_IO            (_VI_ERROR+0x3FFF00A2L) /* BFFF00A2, -1073807198 */
#define VI_ERROR_NSUP_LINE          (_VI_ERROR+0x3FFF00A3L) /* BFFF00A3, -1073807197 */
#define VI_ERROR_NSUP_MECH          (_VI_ERROR+0x3FFF00A4L) /* BFFF00A4, -1073807196 */
#define VI_ERROR_INTF_NUM_NCONFIG   (_VI_ERROR+0x3FFF00A5L) /* BFFF00A5, -1073807195 */
#define VI_ERROR_CONN_LOST          (_VI_ERROR+0x3FFF00A6L) /* BFFF00A6, -1073807194 */
#define VI_ERROR_MACHINE_NAVAIL     (_VI_ERROR+0x3FFF00A7L) /* BFFF00A7, -1073807193 */
#define VI_ERROR_NPERMISSION        (_VI_ERROR+0x3FFF00A8L) /* BFFF00A8, -1073807192 */

/*- Other VISA Definitions --------------------------------------------------*/

#define VI_VERSION_MAJOR(ver)       ((((ViVersion)ver) & 0xFFF00000UL) >> 20)
#define VI_VERSION_MINOR(ver)       ((((ViVersion)ver) & 0x000FFF00UL) >>  8)
#define VI_VERSION_SUBMINOR(ver)    ((((ViVersion)ver) & 0x000000FFUL)      )

#define VI_FIND_BUFLEN              (256)

#define VI_INTF_GPIB                (1)
#define VI_INTF_VXI                 (2)
#define VI_INTF_GPIB_VXI            (3)
#define VI_INTF_ASRL                (4)
#define VI_INTF_PXI                 (5)
#define VI_INTF_TCPIP               (6)
#define VI_INTF_USB                 (7)

#define VI_PROT_NORMAL              (1)
#define VI_PROT_FDC                 (2)
#define VI_PROT_HS488               (3)
#define VI_PROT_4882_STRS           (4)
#define VI_PROT_USBTMC_VENDOR       (5)

#define VI_FDC_NORMAL               (1)
#define VI_FDC_STREAM               (2)

#define VI_LOCAL_SPACE              (0)
#define VI_A16_SPACE                (1)
#define VI_A24_SPACE                (2)
#define VI_A32_SPACE                (3)
#define VI_A64_SPACE                (4)
#define VI_PXI_ALLOC_SPACE          (9)
#define VI_PXI_CFG_SPACE            (10)
#define VI_PXI_BAR0_SPACE           (11)
#define VI_PXI_BAR1_SPACE           (12)
#define VI_PXI_BAR2_SPACE           (13)
#define VI_PXI_BAR3_SPACE           (14)
#define VI_PXI_BAR4_SPACE           (15)
#define VI_PXI_BAR5_SPACE           (16)
#define VI_OPAQUE_SPACE             (0xFFFF)

#define VI_UNKNOWN_LA               (-1)
#define VI_UNKNOWN_SLOT             (-1)
#define VI_UNKNOWN_LEVEL            (-1)
#define VI_UNKNOWN_CHASSIS          (-1)

#define VI_QUEUE                    (1)
#define VI_HNDLR                    (2)
#define VI_SUSPEND_HNDLR            (4)
#define VI_ALL_MECH                 (0xFFFF)

#define VI_ANY_HNDLR                (0)

#define VI_TRIG_ALL                 (-2)
#define VI_TRIG_SW                  (-1)
#define VI_TRIG_TTL0                (0)
#define VI_TRIG_TTL1                (1)
#define VI_TRIG_TTL2                (2)
#define VI_TRIG_TTL3                (3)
#define VI_TRIG_TTL4                (4)
#define VI_TRIG_TTL5                (5)
#define VI_TRIG_TTL6                (6)
#define VI_TRIG_TTL7                (7)
#define VI_TRIG_ECL0                (8)
#define VI_TRIG_ECL1                (9)
#define VI_TRIG_PANEL_IN            (27)
#define VI_TRIG_PANEL_OUT           (28)

#define VI_TRIG_PROT_DEFAULT        (0)
#define VI_TRIG_PROT_ON             (1)
#define VI_TRIG_PROT_OFF            (2)
#define VI_TRIG_PROT_SYNC           (5)
#define VI_TRIG_PROT_RESERVE        (6)
#define VI_TRIG_PROT_UNRESERVE      (7)

#define VI_READ_BUF                 (1)
#define VI_WRITE_BUF                (2)
#define VI_READ_BUF_DISCARD         (4)
#define VI_WRITE_BUF_DISCARD        (8)
#define VI_IO_IN_BUF                (16)
#define VI_IO_OUT_BUF               (32)
#define VI_IO_IN_BUF_DISCARD        (64)
#define VI_IO_OUT_BUF_DISCARD       (128)

#define VI_FLUSH_ON_ACCESS          (1)
#define VI_FLUSH_WHEN_FULL          (2)
#define VI_FLUSH_DISABLE            (3)

#define VI_NMAPPED                  (1)
#define VI_USE_OPERS                (2)
#define VI_DEREF_ADDR               (3)
#define VI_DEREF_ADDR_BYTE_SWAP     (4)

#define VI_TMO_IMMEDIATE            (0L)
#define VI_TMO_INFINITE             (0xFFFFFFFFUL)

#define VI_NO_LOCK                  (0)
#define VI_EXCLUSIVE_LOCK           (1)
#define VI_SHARED_LOCK              (2)
#define VI_LOAD_CONFIG              (4)

#define VI_NO_SEC_ADDR              (0xFFFF)

#define VI_ASRL_PAR_NONE            (0)
#define VI_ASRL_PAR_ODD             (1)
#define VI_ASRL_PAR_EVEN            (2)
#define VI_ASRL_PAR_MARK            (3)
#define VI_ASRL_PAR_SPACE           (4)

#define VI_ASRL_STOP_ONE            (10)
#define VI_ASRL_STOP_ONE5           (15)
#define VI_ASRL_STOP_TWO            (20)

#define VI_ASRL_FLOW_NONE           (0)
#define VI_ASRL_FLOW_XON_XOFF       (1)
#define VI_ASRL_FLOW_RTS_CTS        (2)
#define VI_ASRL_FLOW_DTR_DSR        (4)

#define VI_ASRL_END_NONE            (0)
#define VI_ASRL_END_LAST_BIT        (1)
#define VI_ASRL_END_TERMCHAR        (2)
#define VI_ASRL_END_BREAK           (3)

#define VI_STATE_ASSERTED           (1)
#define VI_STATE_UNASSERTED         (0)
#define VI_STATE_UNKNOWN            (-1)

#define VI_BIG_ENDIAN               (0)
#define VI_LITTLE_ENDIAN            (1)

#define VI_DATA_PRIV                (0)
#define VI_DATA_NPRIV               (1)
#define VI_PROG_PRIV                (2)
#define VI_PROG_NPRIV               (3)
#define VI_BLCK_PRIV                (4)
#define VI_BLCK_NPRIV               (5)
#define VI_D64_PRIV                 (6)
#define VI_D64_NPRIV                (7)

#define VI_WIDTH_8                  (1)
#define VI_WIDTH_16                 (2)
#define VI_WIDTH_32                 (4)
#define VI_WIDTH_64                 (8)

#define VI_GPIB_REN_DEASSERT        (0)
#define VI_GPIB_REN_ASSERT          (1)
#define VI_GPIB_REN_DEASSERT_GTL    (2)
#define VI_GPIB_REN_ASSERT_ADDRESS  (3)
#define VI_GPIB_REN_ASSERT_LLO      (4)
#define VI_GPIB_REN_ASSERT_ADDRESS_LLO (5)
#define VI_GPIB_REN_ADDRESS_GTL     (6)

#define VI_GPIB_ATN_DEASSERT        (0)
#define VI_GPIB_ATN_ASSERT          (1)
#define VI_GPIB_ATN_DEASSERT_HANDSHAKE (2)
#define VI_GPIB_ATN_ASSERT_IMMEDIATE (3)

#define VI_GPIB_HS488_DISABLED      (0)
#define VI_GPIB_HS488_NIMPL         (-1)

#define VI_GPIB_UNADDRESSED         (0)
#define VI_GPIB_TALKER              (1)
#define VI_GPIB_LISTENER            (2)

#define VI_VXI_CMD16                (0x0200)
#define VI_VXI_CMD16_RESP16         (0x0202)
#define VI_VXI_RESP16               (0x0002)
#define VI_VXI_CMD32                (0x0400)
#define VI_VXI_CMD32_RESP16         (0x0402)
#define VI_VXI_CMD32_RESP32         (0x0404)
#define VI_VXI_RESP32               (0x0004)

#define VI_ASSERT_SIGNAL            (-1)
#define VI_ASSERT_USE_ASSIGNED      (0)
#define VI_ASSERT_IRQ1              (1)
#define VI_ASSERT_IRQ2              (2)
#define VI_ASSERT_IRQ3              (3)
#define VI_ASSERT_IRQ4              (4)
#define VI_ASSERT_IRQ5              (5)
#define VI_ASSERT_IRQ6              (6)
#define VI_ASSERT_IRQ7              (7)

#define VI_UTIL_ASSERT_SYSRESET     (1)
#define VI_UTIL_ASSERT_SYSFAIL      (2)
#define VI_UTIL_DEASSERT_SYSFAIL    (3)

#define VI_VXI_CLASS_MEMORY         (0)
#define VI_VXI_CLASS_EXTENDED       (1)
#define VI_VXI_CLASS_MESSAGE        (2)
#define VI_VXI_CLASS_REGISTER       (3)
#define VI_VXI_CLASS_OTHER          (4)

#define VI_PXI_ADDR_NONE            (0)
#define VI_PXI_ADDR_MEM             (1)
#define VI_PXI_ADDR_IO              (2)
#define VI_PXI_ADDR_CFG             (3)

#define VI_TRIG_UNKNOWN             (-1)

#define VI_PXI_LBUS_UNKNOWN         (-1)
#define VI_PXI_LBUS_NONE            (0)
#define VI_PXI_LBUS_STAR_TRIG_BUS_0 (1000)
#define VI_PXI_LBUS_STAR_TRIG_BUS_1 (1001)
#define VI_PXI_LBUS_STAR_TRIG_BUS_2 (1002)
#define VI_PXI_LBUS_STAR_TRIG_BUS_3 (1003)
#define VI_PXI_LBUS_STAR_TRIG_BUS_4 (1004)
#define VI_PXI_LBUS_STAR_TRIG_BUS_5 (1005)
#define VI_PXI_LBUS_STAR_TRIG_BUS_6 (1006)
#define VI_PXI_LBUS_STAR_TRIG_BUS_7 (1007)
#define VI_PXI_LBUS_STAR_TRIG_BUS_8 (1008)
#define VI_PXI_LBUS_STAR_TRIG_BUS_9 (1009)
#define VI_PXI_STAR_TRIG_CONTROLLER (1413)

/*- Backward Compatibility Macros -------------------------------------------*/

#define viGetDefaultRM(vi)          viOpenDefaultRM(vi)
#define VI_ERROR_INV_SESSION        (VI_ERROR_INV_OBJECT)
#define VI_INFINITE                 (VI_TMO_INFINITE)
#define VI_NORMAL                   (VI_PROT_NORMAL)
#define VI_FDC                      (VI_PROT_FDC)
#define VI_HS488                    (VI_PROT_HS488)
#define VI_ASRL488                  (VI_PROT_4882_STRS)
#define VI_ASRL_IN_BUF              (VI_IO_IN_BUF)
#define VI_ASRL_OUT_BUF             (VI_IO_OUT_BUF)
#define VI_ASRL_IN_BUF_DISCARD      (VI_IO_IN_BUF_DISCARD)
#define VI_ASRL_OUT_BUF_DISCARD     (VI_IO_OUT_BUF_DISCARD)

/*- National Instruments ----------------------------------------------------*/

#define VI_INTF_RIO                 (8)
#define VI_INTF_FIREWIRE            (9) 

#define VI_ATTR_SYNC_MXI_ALLOW_EN   (0x3FFF0161UL) /* ViBoolean, read/write */

/* This is for VXI SERVANT resources */

#define VI_EVENT_VXI_DEV_CMD        (0xBFFF200FUL)
#define VI_ATTR_VXI_DEV_CMD_TYPE    (0x3FFF4037UL) /* ViInt16, read-only */
#define VI_ATTR_VXI_DEV_CMD_VALUE   (0x3FFF4038UL) /* ViUInt32, read-only */

#define VI_VXI_DEV_CMD_TYPE_16      (16)
#define VI_VXI_DEV_CMD_TYPE_32      (32)

ViStatus _VI_FUNC viVxiServantResponse(ViSession vi, ViInt16 mode, ViUInt32 resp);
/* mode values include VI_VXI_RESP16, VI_VXI_RESP32, and the next 2 values */
#define VI_VXI_RESP_NONE            (0)
#define VI_VXI_RESP_PROT_ERROR      (-1)

/* This allows extended Serial support on Win32 and on NI ENET Serial products */

#define VI_ATTR_ASRL_DISCARD_NULL   (0x3FFF00B0UL)
#define VI_ATTR_ASRL_CONNECTED      (0x3FFF01BBUL)
#define VI_ATTR_ASRL_BREAK_STATE    (0x3FFF01BCUL)
#define VI_ATTR_ASRL_BREAK_LEN      (0x3FFF01BDUL)
#define VI_ATTR_ASRL_ALLOW_TRANSMIT (0x3FFF01BEUL)
#define VI_ATTR_ASRL_WIRE_MODE      (0x3FFF01BFUL)

#define VI_ASRL_WIRE_485_4          (0)
#define VI_ASRL_WIRE_485_2_DTR_ECHO (1)
#define VI_ASRL_WIRE_485_2_DTR_CTRL (2)
#define VI_ASRL_WIRE_485_2_AUTO     (3)
#define VI_ASRL_WIRE_232_DTE        (128)
#define VI_ASRL_WIRE_232_DCE        (129)
#define VI_ASRL_WIRE_232_AUTO       (130)

#define VI_EVENT_ASRL_BREAK         (0x3FFF2023UL)
#define VI_EVENT_ASRL_CTS           (0x3FFF2029UL)
#define VI_EVENT_ASRL_DSR           (0x3FFF202AUL)
#define VI_EVENT_ASRL_DCD           (0x3FFF202CUL)
#define VI_EVENT_ASRL_RI            (0x3FFF202EUL)
#define VI_EVENT_ASRL_CHAR          (0x3FFF2035UL)
#define VI_EVENT_ASRL_TERMCHAR      (0x3FFF2024UL)

/* This is for fast viPeek/viPoke macros */

#if defined(NIVISA_PEEKPOKE)

#if defined(NIVISA_PEEKPOKE_SUPP)
#undef NIVISA_PEEKPOKE_SUPP
#endif

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(_NI_mswin16_)
/* This macro is supported for all Win32 compilers, including CVI. */
#define NIVISA_PEEKPOKE_SUPP
#elif (defined(_WINDOWS) || defined(_Windows)) && !defined(_CVI_) && !defined(_NI_mswin16_)
/* This macro is supported for Borland and Microsoft compilers on Win16, but not CVI. */
#define NIVISA_PEEKPOKE_SUPP
#elif defined(_CVI_) && defined(_NI_sparc_)
/* This macro is supported for Solaris 1 and 2, from CVI only. */
#define NIVISA_PEEKPOKE_SUPP
#else
/* This macro is not supported on other platforms. */
#endif

#if defined(NIVISA_PEEKPOKE_SUPP)

extern ViBoolean NI_viImplVISA1;
ViStatus _VI_FUNC NI_viOpenDefaultRM (ViPSession vi);
#define viOpenDefaultRM(vi) NI_viOpenDefaultRM(vi)

#define viPeek8(vi,addr,val)                                                \
   {                                                                        \
      if ((NI_viImplVISA1) && (*((ViPUInt32)(vi))))                         \
      {                                                                     \
         do (*((ViPUInt8)(val)) = *((volatile ViUInt8 _VI_PTR)(addr)));     \
         while (**((volatile ViUInt8 _VI_PTR _VI_PTR)(vi)) & 0x10);         \
      }                                                                     \
      else                                                                  \
      {                                                                     \
         (viPeek8)((vi),(addr),(val));                                      \
      }                                                                     \
   }

#define viPoke8(vi,addr,val)                                                \
   {                                                                        \
      if ((NI_viImplVISA1) && (*((ViPUInt32)(vi))))                         \
      {                                                                     \
         do (*((volatile ViUInt8 _VI_PTR)(addr)) = ((ViUInt8)(val)));       \
         while (**((volatile ViUInt8 _VI_PTR _VI_PTR)(vi)) & 0x10);         \
      }                                                                     \
      else                                                                  \
      {                                                                     \
         (viPoke8)((vi),(addr),(val));                                      \
      }                                                                     \
   }

#define viPeek16(vi,addr,val)                                               \
   {                                                                        \
      if ((NI_viImplVISA1) && (*((ViPUInt32)(vi))))                         \
      {                                                                     \
         do (*((ViPUInt16)(val)) = *((volatile ViUInt16 _VI_PTR)(addr)));   \
         while (**((volatile ViUInt8 _VI_PTR _VI_PTR)(vi)) & 0x10);         \
      }                                                                     \
      else                                                                  \
      {                                                                     \
         (viPeek16)((vi),(addr),(val));                                     \
      }                                                                     \
   }

#define viPoke16(vi,addr,val)                                               \
   {                                                                        \
      if ((NI_viImplVISA1) && (*((ViPUInt32)(vi))))                         \
      {                                                                     \
         do (*((volatile ViUInt16 _VI_PTR)(addr)) = ((ViUInt16)(val)));     \
         while (**((volatile ViUInt8 _VI_PTR _VI_PTR)(vi)) & 0x10);         \
      }                                                                     \
      else                                                                  \
      {                                                                     \
         (viPoke16)((vi),(addr),(val));                                     \
      }                                                                     \
   }

#define viPeek32(vi,addr,val)                                               \
   {                                                                        \
      if ((NI_viImplVISA1) && (*((ViPUInt32)(vi))))                         \
      {                                                                     \
         do (*((ViPUInt32)(val)) = *((volatile ViUInt32 _VI_PTR)(addr)));   \
         while (**((volatile ViUInt8 _VI_PTR _VI_PTR)(vi)) & 0x10);         \
      }                                                                     \
      else                                                                  \
      {                                                                     \
         (viPeek32)((vi),(addr),(val));                                     \
      }                                                                     \
   }

#define viPoke32(vi,addr,val)                                               \
   {                                                                        \
      if ((NI_viImplVISA1) && (*((ViPUInt32)(vi))))                         \
      {                                                                     \
         do (*((volatile ViUInt32 _VI_PTR)(addr)) = ((ViUInt32)(val)));     \
         while (**((volatile ViUInt8 _VI_PTR _VI_PTR)(vi)) & 0x10);         \
      }                                                                     \
      else                                                                  \
      {                                                                     \
         (viPoke32)((vi),(addr),(val));                                     \
      }                                                                     \
   }

#endif

#endif

#if defined(NIVISA_PXI) || defined(PXISAVISA_PXI)

#if 0
/* The following 2 attributes were incorrectly implemented in earlier
   versions of NI-VISA.  You should now query VI_ATTR_MANF_ID or
   VI_ATTR_MODEL_CODE.  Those attributes contain sub-vendor information
   when it exists.  To get both the actual primary and subvendor codes
   from the device, you should call viIn16 using VI_PXI_CFG_SPACE. */
#define VI_ATTR_PXI_SUB_MANF_ID     (0x3FFF0203UL)
#define VI_ATTR_PXI_SUB_MODEL_CODE  (0x3FFF0204UL)
#endif

#define VI_ATTR_PXI_SRC_TRIG_BUS    (0x3FFF020DUL)
#define VI_ATTR_PXI_DEST_TRIG_BUS   (0x3FFF020EUL)

#define VI_ATTR_PXI_RECV_INTR_SEQ   (0x3FFF4240UL)
#define VI_ATTR_PXI_RECV_INTR_DATA  (0x3FFF4241UL)

#endif

#if defined(NIVISA_USB)

#define VI_ATTR_USB_BULK_OUT_PIPE   (0x3FFF01A2UL)
#define VI_ATTR_USB_BULK_IN_PIPE    (0x3FFF01A3UL)
#define VI_ATTR_USB_INTR_IN_PIPE    (0x3FFF01A4UL)
#define VI_ATTR_USB_CLASS           (0x3FFF01A5UL)
#define VI_ATTR_USB_SUBCLASS        (0x3FFF01A6UL)
#define VI_ATTR_USB_ALT_SETTING     (0x3FFF01A8UL)
#define VI_ATTR_USB_END_IN          (0x3FFF01A9UL)
#define VI_ATTR_USB_NUM_INTFCS      (0x3FFF01AAUL)
#define VI_ATTR_USB_NUM_PIPES       (0x3FFF01ABUL)
#define VI_ATTR_USB_BULK_OUT_STATUS (0x3FFF01ACUL)
#define VI_ATTR_USB_BULK_IN_STATUS  (0x3FFF01ADUL)
#define VI_ATTR_USB_INTR_IN_STATUS  (0x3FFF01AEUL)
#define VI_ATTR_USB_CTRL_PIPE       (0x3FFF01B0UL)

#define VI_USB_PIPE_STATE_UNKNOWN   (-1)
#define VI_USB_PIPE_READY           (0)
#define VI_USB_PIPE_STALLED         (1)

#define VI_USB_END_NONE             (0)
#define VI_USB_END_SHORT            (4)
#define VI_USB_END_SHORT_OR_COUNT   (5)

#endif

#define VI_ATTR_FIREWIRE_DEST_UPPER_OFFSET (0x3FFF01F0UL)
#define VI_ATTR_FIREWIRE_SRC_UPPER_OFFSET  (0x3FFF01F1UL)
#define VI_ATTR_FIREWIRE_WIN_UPPER_OFFSET  (0x3FFF01F2UL)
#define VI_ATTR_FIREWIRE_VENDOR_ID         (0x3FFF01F3UL)
#define VI_ATTR_FIREWIRE_LOWER_CHIP_ID     (0x3FFF01F4UL)
#define VI_ATTR_FIREWIRE_UPPER_CHIP_ID     (0x3FFF01F5UL)

#define VI_FIREWIRE_DFLT_SPACE           (5)

#if defined(__cplusplus) || defined(__cplusplus__)
   }
#endif

#endif

/*- The End -----------------------------------------------------------------*/
