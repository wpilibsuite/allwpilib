// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_Power_h__
#define __nFRC_2015_1_0_A_Power_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tPower
{
public:
   tPower(){}
   virtual ~tPower(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tPower* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned User3V3 : 8;
         unsigned User5V : 8;
         unsigned User6V : 8;
#else
         unsigned User6V : 8;
         unsigned User5V : 8;
         unsigned User3V3 : 8;
#endif
      };
      struct{
         unsigned value : 24;
      };
   } tStatus;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned OverCurrentFaultCount3V3 : 10;
         unsigned OverCurrentFaultCount5V : 10;
         unsigned OverCurrentFaultCount6V : 10;
#else
         unsigned OverCurrentFaultCount6V : 10;
         unsigned OverCurrentFaultCount5V : 10;
         unsigned OverCurrentFaultCount3V3 : 10;
#endif
      };
      struct{
         unsigned value : 30;
      };
   } tOverCurrentFaultCounts;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned User3V3 : 1;
         unsigned User5V : 1;
         unsigned User6V : 1;
#else
         unsigned User6V : 1;
         unsigned User5V : 1;
         unsigned User3V3 : 1;
#endif
      };
      struct{
         unsigned value : 3;
      };
   } tDisable;



   typedef enum
   {
   } tUserVoltage3V3_IfaceConstants;

   virtual unsigned short readUserVoltage3V3(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tStatus_IfaceConstants;

   virtual tStatus readStatus(tRioStatusCode *status) = 0;
   virtual unsigned char readStatus_User3V3(tRioStatusCode *status) = 0;
   virtual unsigned char readStatus_User5V(tRioStatusCode *status) = 0;
   virtual unsigned char readStatus_User6V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tUserVoltage6V_IfaceConstants;

   virtual unsigned short readUserVoltage6V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tOnChipTemperature_IfaceConstants;

   virtual unsigned short readOnChipTemperature(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tUserVoltage5V_IfaceConstants;

   virtual unsigned short readUserVoltage5V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tResetFaultCounts_IfaceConstants;

   virtual void strobeResetFaultCounts(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tOverCurrentFaultCounts_IfaceConstants;

   virtual tOverCurrentFaultCounts readOverCurrentFaultCounts(tRioStatusCode *status) = 0;
   virtual unsigned short readOverCurrentFaultCounts_OverCurrentFaultCount3V3(tRioStatusCode *status) = 0;
   virtual unsigned short readOverCurrentFaultCounts_OverCurrentFaultCount5V(tRioStatusCode *status) = 0;
   virtual unsigned short readOverCurrentFaultCounts_OverCurrentFaultCount6V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tIntegratedIO_IfaceConstants;

   virtual unsigned short readIntegratedIO(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tMXP_DIOVoltage_IfaceConstants;

   virtual unsigned short readMXP_DIOVoltage(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tUserCurrent3V3_IfaceConstants;

   virtual unsigned short readUserCurrent3V3(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tVinVoltage_IfaceConstants;

   virtual unsigned short readVinVoltage(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tUserCurrent6V_IfaceConstants;

   virtual unsigned short readUserCurrent6V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tUserCurrent5V_IfaceConstants;

   virtual unsigned short readUserCurrent5V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tAOVoltage_IfaceConstants;

   virtual unsigned short readAOVoltage(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tVinCurrent_IfaceConstants;

   virtual unsigned short readVinCurrent(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDisable_IfaceConstants;

   virtual void writeDisable(tDisable value, tRioStatusCode *status) = 0;
   virtual void writeDisable_User3V3(bool value, tRioStatusCode *status) = 0;
   virtual void writeDisable_User5V(bool value, tRioStatusCode *status) = 0;
   virtual void writeDisable_User6V(bool value, tRioStatusCode *status) = 0;
   virtual tDisable readDisable(tRioStatusCode *status) = 0;
   virtual bool readDisable_User3V3(tRioStatusCode *status) = 0;
   virtual bool readDisable_User5V(tRioStatusCode *status) = 0;
   virtual bool readDisable_User6V(tRioStatusCode *status) = 0;




private:
   tPower(const tPower&);
   void operator=(const tPower&);
};

}
}

#endif // __nFRC_2015_1_0_A_Power_h__
