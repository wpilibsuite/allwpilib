/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Base.h"
#include "Error.h"
#include "HAL/Semaphore.hpp"

#define wpi_setErrnoErrorWithContext(context)   (this->SetErrnoError((context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setErrnoError()   (wpi_setErrnoErrorWithContext(""))
#define wpi_setImaqErrorWithContext(code, context)   (this->SetImaqError((code), (context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setErrorWithContext(code, context)   (this->SetError((code), (context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setError(code)   (wpi_setErrorWithContext(code, ""))
#define wpi_setStaticErrorWithContext(object, code, context)   (object->SetError((code), (context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setStaticError(object, code)   (wpi_setStaticErrorWithContext(object, code, ""))
#define wpi_setGlobalErrorWithContext(code, context)   (ErrorBase::SetGlobalError((code), (context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setGlobalError(code)   (wpi_setGlobalErrorWithContext(code, ""))
#define wpi_setWPIErrorWithContext(error, context)   (this->SetWPIError((wpi_error_s_##error), (wpi_error_value_##error), (context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setWPIError(error)   (wpi_setWPIErrorWithContext(error, ""))
#define wpi_setStaticWPIErrorWithContext(object, error, context)   (object->SetWPIError((wpi_error_s_##error), (context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setStaticWPIError(object, error)   (wpi_setStaticWPIErrorWithContext(object, error, ""))
#define wpi_setGlobalWPIErrorWithContext(error, context)   (ErrorBase::SetGlobalWPIError((wpi_error_s_##error), (context), __FILE__, __FUNCTION__, __LINE__))
#define wpi_setGlobalWPIError(error)   (wpi_setGlobalWPIErrorWithContext(error, ""))

/**
 * Base class for most objects.
 * ErrorBase is the base class for most objects since it holds the generated error
 * for that object. In addition, there is a single instance of a global error object
 */
class ErrorBase
{
//TODO: Consider initializing instance variables and cleanup in destructor
public:
	virtual ~ErrorBase();
	virtual Error& GetError();
	virtual const Error& GetError() const;
	virtual void SetErrnoError(const char *contextMessage, const char* filename,
			const char* function, uint32_t lineNumber) const;
	virtual void SetImaqError(int success, const char *contextMessage, const char* filename,
			const char* function, uint32_t lineNumber) const;
	virtual void SetError(Error::Code code, const char *contextMessage, const char* filename,
			const char* function, uint32_t lineNumber) const;
	virtual void SetWPIError(const char *errorMessage, Error::Code code, const char *contextMessage,
			const char* filename, const char* function, uint32_t lineNumber) const;
	virtual void CloneError(ErrorBase *rhs) const;
	virtual void ClearError() const;
	virtual bool StatusIsFatal() const;
	static void SetGlobalError(Error::Code code, const char *contextMessage, const char* filename,
			const char* function, uint32_t lineNumber);
	static void SetGlobalWPIError(const char *errorMessage, const char *contextMessage,
			const char* filename, const char* function, uint32_t lineNumber);
	static Error& GetGlobalError();
protected:
	mutable Error m_error;
	// TODO: Replace globalError with a global list of all errors.
	static MUTEX_ID _globalErrorMutex;
	static Error _globalError;
	ErrorBase();
private:
	DISALLOW_COPY_AND_ASSIGN(ErrorBase);
};
