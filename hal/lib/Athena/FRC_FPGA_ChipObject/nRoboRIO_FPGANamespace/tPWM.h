// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_PWM_h__
#define __nFRC_2015_1_0_A_PWM_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tPWM
{
public:
   tPWM(){}
   virtual ~tPWM(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tPWM* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Period : 16;
         unsigned MinHigh : 16;
#else
         unsigned MinHigh : 16;
         unsigned Period : 16;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tConfig;



   typedef enum
   {
   } tConfig_IfaceConstants;

   virtual void writeConfig(tConfig value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Period(unsigned short value, tRioStatusCode *status) = 0;
   virtual void writeConfig_MinHigh(unsigned short value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual unsigned short readConfig_Period(tRioStatusCode *status) = 0;
   virtual unsigned short readConfig_MinHigh(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tLoopTiming_IfaceConstants;

   virtual unsigned short readLoopTiming(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumPeriodScaleMXPElements = 10,
   } tPeriodScaleMXP_IfaceConstants;

   virtual void writePeriodScaleMXP(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPeriodScaleMXP(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumPeriodScaleHdrElements = 10,
   } tPeriodScaleHdr_IfaceConstants;

   virtual void writePeriodScaleHdr(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPeriodScaleHdr(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumZeroLatchElements = 20,
   } tZeroLatch_IfaceConstants;

   virtual void writeZeroLatch(unsigned char bitfield_index, bool value, tRioStatusCode *status) = 0;
   virtual bool readZeroLatch(unsigned char bitfield_index, tRioStatusCode *status) = 0;




   typedef enum
   {
      kNumHdrRegisters = 10,
   } tHdr_IfaceConstants;

   virtual void writeHdr(unsigned char reg_index, unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readHdr(unsigned char reg_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumMXPRegisters = 10,
   } tMXP_IfaceConstants;

   virtual void writeMXP(unsigned char reg_index, unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readMXP(unsigned char reg_index, tRioStatusCode *status) = 0;


private:
   tPWM(const tPWM&);
   void operator=(const tPWM&);
};

}
}

#endif // __nFRC_2015_1_0_A_PWM_h__
