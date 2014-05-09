/*!
   \file environs.h
   \brief Defines export symbols and namespace aliases
*/
/*
   Copyright (c) 2014,
   National Instruments Corporation.
   All rights reserved.

   File:        $Id: //lvaddon/FIRST/FRC/trunk/2015/tools/frcrti/export/1.0/1.0.0a6/includes/wpilib/i2clib/i2clib/environs.h#1 $
   Author:      nipg.pl
   Originated:  Mon Feb 10 10:39:42 2014
*/

#ifndef ___i2clib_environs_h___
#define ___i2clib_environs_h___

#include <nibuild/platform.h>

/*
 * kI2CLIBExportSymbols directs the build to export symbols modified by
 * the kI2CLIBExport keyword. kI2CLIBNoExportSymbols directs the build to not
 * import or export symbols modified by the kI2CLIBExport keyword. If
 * neither of these are defined, the symbols modified by kI2CLIBExport are
 * imported. These should be defined only when building the component,
 * so clients do not need to trouble themselves with it.
 */
#if defined(kI2CLIBExportSymbols)
   #define kI2CLIBExport kNIExport
   #define kI2CLIBExportPre kNIExportPre
   #define kI2CLIBExportPost kNIExportPost
   #define kI2CLIBExportInline kNIExportInline
   #define kI2CLIBExportData kNIExportData
#elif defined(kI2CLIBNoExportSymbols)
   #define kI2CLIBExport
   #define kI2CLIBExportPre
   #define kI2CLIBExportPost
   #define kI2CLIBExportInline
   #define kI2CLIBExportData
#else
   #define kI2CLIBExport kNIImport
   #define kI2CLIBExportPre kNIImportPre
   #define kI2CLIBExportPost kNIImportPost
   #define kI2CLIBExportInline kNIImportInline
   #define kI2CLIBExportData kNIImportData
#endif

// namespace declarations for aliasing ...

#ifdef __cplusplus


/*!
   \namespace nI2CLIB_1_0
   \brief i2c user-mode library Release 1.0
*/
namespace nI2CLIB_1_0
{
   // current versioned namespace aliases used by this package

}

#endif // __cplusplus

#endif // ___i2clib_environs_h___

