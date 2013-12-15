// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_2_DIO_h__
#define __nFRC_2015_1_0_2_DIO_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_2
{

class tDIO
{
public:
   tDIO(){}
   virtual ~tDIO(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tDIO* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Headers : 10;
         unsigned Reserved : 6;
         unsigned MXP : 16;
#else
         unsigned MXP : 16;
         unsigned Reserved : 6;
         unsigned Headers : 10;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tDO;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Headers : 10;
         unsigned Reserved : 6;
         unsigned MXP : 16;
#else
         unsigned MXP : 16;
         unsigned Reserved : 6;
         unsigned Headers : 10;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tOutputEnable;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Headers : 10;
         unsigned Reserved : 6;
         unsigned MXP : 16;
#else
         unsigned MXP : 16;
         unsigned Reserved : 6;
         unsigned Headers : 10;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tPulse;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Headers : 10;
         unsigned Reserved : 6;
         unsigned MXP : 16;
#else
         unsigned MXP : 16;
         unsigned Reserved : 6;
         unsigned Headers : 10;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tDI;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned PeriodPower : 6;
         unsigned OutputSelect_0 : 5;
         unsigned OutputSelect_1 : 5;
         unsigned OutputSelect_2 : 5;
         unsigned OutputSelect_3 : 5;
#else
         unsigned OutputSelect_3 : 5;
         unsigned OutputSelect_2 : 5;
         unsigned OutputSelect_1 : 5;
         unsigned OutputSelect_0 : 5;
         unsigned PeriodPower : 6;
#endif
      };
      struct{
         unsigned value : 26;
      };
   } tPWMConfig;



   typedef enum
   {
   } tDO_IfaceConstants;

   virtual void writeDO(tDO value, tRioStatusCode *status) = 0;
   virtual void writeDO_Headers(unsigned short value, tRioStatusCode *status) = 0;
   virtual void writeDO_Reserved(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeDO_MXP(unsigned short value, tRioStatusCode *status) = 0;
   virtual tDO readDO(tRioStatusCode *status) = 0;
   virtual unsigned short readDO_Headers(tRioStatusCode *status) = 0;
   virtual unsigned char readDO_Reserved(tRioStatusCode *status) = 0;
   virtual unsigned short readDO_MXP(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumPWMDutyCycleElements = 4,
   } tPWMDutyCycle_IfaceConstants;

   virtual void writePWMDutyCycle(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPWMDutyCycle(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterSelectHdrElements = 16,
   } tFilterSelectHdr_IfaceConstants;

   virtual void writeFilterSelectHdr(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readFilterSelectHdr(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterPeriodMXPElements = 3,
   } tFilterPeriodMXP_IfaceConstants;

   virtual void writeFilterPeriodMXP(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readFilterPeriodMXP(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
   } tOutputEnable_IfaceConstants;

   virtual void writeOutputEnable(tOutputEnable value, tRioStatusCode *status) = 0;
   virtual void writeOutputEnable_Headers(unsigned short value, tRioStatusCode *status) = 0;
   virtual void writeOutputEnable_Reserved(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeOutputEnable_MXP(unsigned short value, tRioStatusCode *status) = 0;
   virtual tOutputEnable readOutputEnable(tRioStatusCode *status) = 0;
   virtual unsigned short readOutputEnable_Headers(tRioStatusCode *status) = 0;
   virtual unsigned char readOutputEnable_Reserved(tRioStatusCode *status) = 0;
   virtual unsigned short readOutputEnable_MXP(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tPulse_IfaceConstants;

   virtual void writePulse(tPulse value, tRioStatusCode *status) = 0;
   virtual void writePulse_Headers(unsigned short value, tRioStatusCode *status) = 0;
   virtual void writePulse_Reserved(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writePulse_MXP(unsigned short value, tRioStatusCode *status) = 0;
   virtual tPulse readPulse(tRioStatusCode *status) = 0;
   virtual unsigned short readPulse_Headers(tRioStatusCode *status) = 0;
   virtual unsigned char readPulse_Reserved(tRioStatusCode *status) = 0;
   virtual unsigned short readPulse_MXP(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDI_IfaceConstants;

   virtual tDI readDI(tRioStatusCode *status) = 0;
   virtual unsigned short readDI_Headers(tRioStatusCode *status) = 0;
   virtual unsigned char readDI_Reserved(tRioStatusCode *status) = 0;
   virtual unsigned short readDI_MXP(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tEnableMXPSpecialFunction_IfaceConstants;

   virtual void writeEnableMXPSpecialFunction(unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readEnableMXPSpecialFunction(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterSelectMXPElements = 16,
   } tFilterSelectMXP_IfaceConstants;

   virtual void writeFilterSelectMXP(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readFilterSelectMXP(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
   } tPulseLength_IfaceConstants;

   virtual void writePulseLength(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPulseLength(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterPeriodHdrElements = 3,
   } tFilterPeriodHdr_IfaceConstants;

   virtual void writeFilterPeriodHdr(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readFilterPeriodHdr(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
   } tPWMConfig_IfaceConstants;

   virtual void writePWMConfig(tPWMConfig value, tRioStatusCode *status) = 0;
   virtual void writePWMConfig_PeriodPower(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writePWMConfig_OutputSelect_0(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writePWMConfig_OutputSelect_1(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writePWMConfig_OutputSelect_2(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writePWMConfig_OutputSelect_3(unsigned char value, tRioStatusCode *status) = 0;
   virtual tPWMConfig readPWMConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readPWMConfig_PeriodPower(tRioStatusCode *status) = 0;
   virtual unsigned char readPWMConfig_OutputSelect_0(tRioStatusCode *status) = 0;
   virtual unsigned char readPWMConfig_OutputSelect_1(tRioStatusCode *status) = 0;
   virtual unsigned char readPWMConfig_OutputSelect_2(tRioStatusCode *status) = 0;
   virtual unsigned char readPWMConfig_OutputSelect_3(tRioStatusCode *status) = 0;




private:
   tDIO(const tDIO&);
   void operator=(const tDIO&);
};

}
}

#endif // __nFRC_2015_1_0_2_DIO_h__
