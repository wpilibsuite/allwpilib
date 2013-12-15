/*!
   \file environs.h
   \brief Defines export symbols and namespace aliases
*/
/*
   Copyright (c) 2013,
   National Instruments Corporation.
   All rights reserved.

   File:        $Id: //lvaddon/FIRST/FRC/trunk/2015/tools/FRCNetworkCommunication/export/1.5/1.5.0a3/includes/FRC_NetworkCommunication/environs.h#1 $
   Author:      nipg.pl
   Originated:  Wed Aug 28 16:07:10 2013
*/

#ifndef ___FRC_NetworkCommunication_environs_h___
#define ___FRC_NetworkCommunication_environs_h___

#include <nibuild/platform.h>

/*
 * kFRC_NETWORKCOMMUNICATIONExportSymbols directs the build to export symbols modified by
 * the kFRC_NETWORKCOMMUNICATIONExport keyword. kFRC_NETWORKCOMMUNICATIONNoExportSymbols directs the build to not
 * import or export symbols modified by the kFRC_NETWORKCOMMUNICATIONExport keyword. If
 * neither of these are defined, the symbols modified by kFRC_NETWORKCOMMUNICATIONExport are
 * imported. These should be defined only when building the component,
 * so clients do not need to trouble themselves with it.
 */
#if defined(kFRC_NETWORKCOMMUNICATIONExportSymbols)
   #define kFRC_NETWORKCOMMUNICATIONExport kNIExport
   #define kFRC_NETWORKCOMMUNICATIONExportPre kNIExportPre
   #define kFRC_NETWORKCOMMUNICATIONExportPost kNIExportPost
   #define kFRC_NETWORKCOMMUNICATIONExportInline kNIExportInline
   #define kFRC_NETWORKCOMMUNICATIONExportData kNIExportData
#elif defined(kFRC_NETWORKCOMMUNICATIONNoExportSymbols)
   #define kFRC_NETWORKCOMMUNICATIONExport
   #define kFRC_NETWORKCOMMUNICATIONExportPre
   #define kFRC_NETWORKCOMMUNICATIONExportPost
   #define kFRC_NETWORKCOMMUNICATIONExportInline
   #define kFRC_NETWORKCOMMUNICATIONExportData
#else
   #define kFRC_NETWORKCOMMUNICATIONExport kNIImport
   #define kFRC_NETWORKCOMMUNICATIONExportPre kNIImportPre
   #define kFRC_NETWORKCOMMUNICATIONExportPost kNIImportPost
   #define kFRC_NETWORKCOMMUNICATIONExportInline kNIImportInline
   #define kFRC_NETWORKCOMMUNICATIONExportData kNIImportData
#endif

// namespace declarations for aliasing ...

#ifdef __cplusplus

namespace nACE_STATIC_5_6 { }
namespace nCINTOOLS_1_2 { }
namespace nFRC_FPGA_CHIPOBJECT_1_2 { }
namespace nIAK_SHARED_13_0 { }
namespace nNIFPGA_13_0 { }
namespace nNI_EMB_6_0 { }
namespace nROBORIO_FRC_CHIPOBJECT_1_2 { }

/*!
   \namespace nFRC_NETWORKCOMMUNICATION_1_5
   \brief FRC Network Communication Release 1.5
*/
namespace nFRC_NETWORKCOMMUNICATION_1_5
{
   // current versioned namespace aliases used by this package

   // FRC_FPGA_ChipObject 1.2:
   namespace nFRC_FPGA_CHIPOBJECT = nFRC_FPGA_CHIPOBJECT_1_2;

   // RoboRIO_FRC_ChipObject 1.2:
   namespace nROBORIO_FRC_CHIPOBJECT = nROBORIO_FRC_CHIPOBJECT_1_2;

   // ace_static 5.6:
   namespace nACE_STATIC = nACE_STATIC_5_6;

   // cintools 1.2:
   namespace nCINTOOLS = nCINTOOLS_1_2;

   // iak_shared 13.0:
   namespace nIAK_SHARED = nIAK_SHARED_13_0;

   // ni_emb 6.0:
   namespace nNI_EMB = nNI_EMB_6_0;

   // nifpga 13.0:
   namespace nNIFPGA = nNIFPGA_13_0;

}

#endif // __cplusplus

#endif // ___FRC_NetworkCommunication_environs_h___

