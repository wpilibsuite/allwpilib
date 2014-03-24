/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef _ERROR_BASE_H
#define _ERROR_BASE_H

#if (defined __vxworks || defined WIN32)
#include <semLib.h>
#ifdef __vxworks
#include <vxWorks.h>
#endif

#define DISALLOW_COPY_AND_ASSIGN(ErrorBase) 

#define wpi_setErrnoErrorWithContext(context) 
#define wpi_setErrnoError() 
#define wpi_setImaqErrorWithContext(code, context)  
#define wpi_setErrorWithContext(code, context)   
#define wpi_setError(code)   
#define wpi_setStaticErrorWithContext(object, code, context)  
#define wpi_setStaticError(object, code)
#define wpi_setGlobalErrorWithContext(code, context)  
#define wpi_setGlobalError(code) 
#define wpi_setWPIErrorWithContext(error, context)
#define wpi_setWPIError(error)   
#define wpi_setStaticWPIErrorWithContext(object, error, context)  
#define wpi_setStaticWPIError(object, error)   
#define wpi_setGlobalWPIErrorWithContext(error, context)  
#define wpi_setGlobalWPIError(error) 

/**
 * Base class for most objects.
 * ErrorBase is the base class for most objects since it holds the generated error
 * for that object. In addition, there is a single instance of a global error object
 */
class ErrorBase
{
//TODO: Consider initializing instance variables and cleanup in destructor
public:
	
};

#endif
#endif // __vxworks
