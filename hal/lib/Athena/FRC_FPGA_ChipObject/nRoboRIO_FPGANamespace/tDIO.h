// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_DIO_h__
#define __nFRC_2015_1_0_A_DIO_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
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
      kNumPWMDutyCycleAElements = 4,
   } tPWMDutyCycleA_IfaceConstants;

   virtual void writePWMDutyCycleA(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPWMDutyCycleA(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumPWMDutyCycleBElements = 2,
   } tPWMDutyCycleB_IfaceConstants;

   virtual void writePWMDutyCycleB(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPWMDutyCycleB(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterSelectHdrElements = 16,
   } tFilterSelectHdr_IfaceConstants;

   virtual void writeFilterSelectHdr(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readFilterSelectHdr(unsigned char bitfield_index, tRioStatusCode *status) = 0;


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
      kNumPWMOutputSelectElements = 6,
   } tPWMOutputSelect_IfaceConstants;

   virtual void writePWMOutputSelect(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPWMOutputSelect(unsigned char bitfield_index, tRioStatusCode *status) = 0;


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
   } tPWMPeriodPower_IfaceConstants;

   virtual void writePWMPeriodPower(unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readPWMPeriodPower(tRioStatusCode *status) = 0;




   typedef enum
   {
      kNumFilterPeriodMXPRegisters = 3,
   } tFilterPeriodMXP_IfaceConstants;

   virtual void writeFilterPeriodMXP(unsigned char reg_index, unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readFilterPeriodMXP(unsigned char reg_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterPeriodHdrRegisters = 3,
   } tFilterPeriodHdr_IfaceConstants;

   virtual void writeFilterPeriodHdr(unsigned char reg_index, unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readFilterPeriodHdr(unsigned char reg_index, tRioStatusCode *status) = 0;


private:
   tDIO(const tDIO&);
   void operator=(const tDIO&);
};

}
}

#endif // __nFRC_2015_1_0_A_DIO_h__
