// Base class for generated chip objects
// Copyright (c) National Instruments 2008.  All Rights Reserved.

#ifndef __tSystem_h__
#define __tSystem_h__

#include "fpgainterfacecapi/NiFpga.h"
typedef NiFpga_Status tRioStatusCode;

#define FRC_FPGA_PRELOAD_BITFILE

typedef uint32_t NiFpga_Session;

namespace nFPGA
{

class tSystem
{
public:
   tSystem(tRioStatusCode *status);
   ~tSystem();
   void getFpgaGuid(uint32_t *guid_ptr, tRioStatusCode *status);
   void reset(tRioStatusCode *status);

protected:
   static NiFpga_Session _DeviceHandle;

#ifdef FRC_FPGA_PRELOAD_BITFILE
   void NiFpga_SharedOpen_common(const char*     bitfile);
   NiFpga_Status NiFpga_SharedOpen(const char*     bitfile,
                            const char*     signature,
                            const char*     resource,
                            uint32_t        attribute,
                            NiFpga_Session* session);
   NiFpga_Status NiFpgaLv_SharedOpen(const char* const     bitfile,
                            const char* const     apiSignature,
                            const char* const     resource,
                            const uint32_t        attribute,
                            NiFpga_Session* const session);
private:
    static char *_FileName;
    static char *_Bitfile;
#endif
};

}

#endif // __tSystem_h__
