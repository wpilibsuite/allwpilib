// Copyright (c) National Instruments 2008.  All Rights Reserved.

#ifndef __printFPGAVersion_h__
#define __printFPGAVersion_h__

namespace nFPGA
{

template<typename ttGlobal>
inline void printFPGAVersion(ttGlobal &global)
{
   tRioStatusCode cleanStatus=0;
   uint32_t hardwareGuid[4];
   tSystemInterface &system = *global.getSystemInterface();
   system.getHardwareFpgaSignature(hardwareGuid, &cleanStatus);
   const uint32_t *softwareGuid = system.getExpectedFPGASignature();
   printf("FPGA Hardware GUID: 0x");
   for(int i=0; i<4; i++)
   {
      printf("%08X", hardwareGuid[i]);
   }
   printf("\n");
   printf("FPGA Software GUID: 0x");
   for(int i=0; i<4; i++)
   {
      printf("%08X", softwareGuid[i]);
   }
   printf("\n");
   uint16_t fpgaHardwareVersion = global.readVersion(&cleanStatus);
   uint16_t fpgaSoftwareVersion = system.getExpectedFPGAVersion();
   printf("FPGA Hardware Version: %X\n", fpgaHardwareVersion);
   printf("FPGA Software Version: %X\n", fpgaSoftwareVersion);
   uint32_t fpgaHardwareRevision = global.readRevision(&cleanStatus);
   uint32_t fpgaSoftwareRevision = system.getExpectedFPGARevision();
   printf("FPGA Hardware Revision: %X.%X.%X\n", (fpgaHardwareRevision >> 20) & 0xFFF, (fpgaHardwareRevision >> 12) & 0xFF, fpgaHardwareRevision & 0xFFF);
   printf("FPGA Software Revision: %X.%X.%X\n", (fpgaSoftwareRevision >> 20) & 0xFFF, (fpgaSoftwareRevision >> 12) & 0xFF, fpgaSoftwareRevision & 0xFFF);
}

}

#endif // __printFPGAVersion_h__

