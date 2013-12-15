// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_DIO_h__
#define __nFRC_2012_1_6_4_DIO_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
{

class tDIO
{
public:
   tDIO(){}
   virtual ~tDIO(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tDIO* create(unsigned char sys_index, tRioStatusCode *status);
   virtual unsigned char getSystemIndex() = 0;


   typedef enum
   {
      kNumSystems = 2,
   } tIfaceConstants;

   typedef
   union{
      struct{
         unsigned Period : 16;
         unsigned MinHigh : 16;
      };
      struct{
         unsigned value : 32;
      };
   } tPWMConfig;
   typedef
   union{
      struct{
         unsigned RelayFwd : 8;
         unsigned RelayRev : 8;
         unsigned I2CHeader : 4;
      };
      struct{
         unsigned value : 20;
      };
   } tSlowValue;
   typedef
   union{
      struct{
         unsigned Transaction : 1;
         unsigned Done : 1;
         unsigned Aborted : 1;
         unsigned DataReceivedHigh : 24;
      };
      struct{
         unsigned value : 27;
      };
   } tI2CStatus;
   typedef
   union{
      struct{
         unsigned PeriodPower : 4;
         unsigned OutputSelect_0 : 4;
         unsigned OutputSelect_1 : 4;
         unsigned OutputSelect_2 : 4;
         unsigned OutputSelect_3 : 4;
      };
      struct{
         unsigned value : 20;
      };
   } tDO_PWMConfig;
   typedef
   union{
      struct{
         unsigned Address : 8;
         unsigned BytesToRead : 3;
         unsigned BytesToWrite : 3;
         unsigned DataToSendHigh : 16;
         unsigned BitwiseHandshake : 1;
      };
      struct{
         unsigned value : 31;
      };
   } tI2CConfig;


   typedef enum
   {
   } tI2CDataToSend_IfaceConstants;

   virtual void writeI2CDataToSend(unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readI2CDataToSend(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDO_IfaceConstants;

   virtual void writeDO(unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readDO(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterSelectElements = 16,
   } tFilterSelect_IfaceConstants;

   virtual void writeFilterSelect(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readFilterSelect(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumFilterPeriodElements = 3,
   } tFilterPeriod_IfaceConstants;

   virtual void writeFilterPeriod(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readFilterPeriod(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
   } tOutputEnable_IfaceConstants;

   virtual void writeOutputEnable(unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readOutputEnable(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tSlowValue_IfaceConstants;

   virtual void writeSlowValue(tSlowValue value, tRioStatusCode *status) = 0;
   virtual void writeSlowValue_RelayFwd(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeSlowValue_RelayRev(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeSlowValue_I2CHeader(unsigned char value, tRioStatusCode *status) = 0;
   virtual tSlowValue readSlowValue(tRioStatusCode *status) = 0;
   virtual unsigned char readSlowValue_RelayFwd(tRioStatusCode *status) = 0;
   virtual unsigned char readSlowValue_RelayRev(tRioStatusCode *status) = 0;
   virtual unsigned char readSlowValue_I2CHeader(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tI2CStatus_IfaceConstants;

   virtual tI2CStatus readI2CStatus(tRioStatusCode *status) = 0;
   virtual unsigned char readI2CStatus_Transaction(tRioStatusCode *status) = 0;
   virtual bool readI2CStatus_Done(tRioStatusCode *status) = 0;
   virtual bool readI2CStatus_Aborted(tRioStatusCode *status) = 0;
   virtual unsigned int readI2CStatus_DataReceivedHigh(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tI2CDataReceived_IfaceConstants;

   virtual unsigned int readI2CDataReceived(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDI_IfaceConstants;

   virtual unsigned short readDI(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tPulse_IfaceConstants;

   virtual void writePulse(unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readPulse(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumPWMPeriodScaleElements = 10,
   } tPWMPeriodScale_IfaceConstants;

   virtual void writePWMPeriodScale(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPWMPeriodScale(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumDO_PWMDutyCycleElements = 4,
   } tDO_PWMDutyCycle_IfaceConstants;

   virtual void writeDO_PWMDutyCycle(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readDO_PWMDutyCycle(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
   } tBFL_IfaceConstants;

   virtual void writeBFL(bool value, tRioStatusCode *status) = 0;
   virtual bool readBFL(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tI2CStart_IfaceConstants;

   virtual void strobeI2CStart(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDO_PWMConfig_IfaceConstants;

   virtual void writeDO_PWMConfig(tDO_PWMConfig value, tRioStatusCode *status) = 0;
   virtual void writeDO_PWMConfig_PeriodPower(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeDO_PWMConfig_OutputSelect_0(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeDO_PWMConfig_OutputSelect_1(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeDO_PWMConfig_OutputSelect_2(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeDO_PWMConfig_OutputSelect_3(unsigned char value, tRioStatusCode *status) = 0;
   virtual tDO_PWMConfig readDO_PWMConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readDO_PWMConfig_PeriodPower(tRioStatusCode *status) = 0;
   virtual unsigned char readDO_PWMConfig_OutputSelect_0(tRioStatusCode *status) = 0;
   virtual unsigned char readDO_PWMConfig_OutputSelect_1(tRioStatusCode *status) = 0;
   virtual unsigned char readDO_PWMConfig_OutputSelect_2(tRioStatusCode *status) = 0;
   virtual unsigned char readDO_PWMConfig_OutputSelect_3(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tPulseLength_IfaceConstants;

   virtual void writePulseLength(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPulseLength(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tI2CConfig_IfaceConstants;

   virtual void writeI2CConfig(tI2CConfig value, tRioStatusCode *status) = 0;
   virtual void writeI2CConfig_Address(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeI2CConfig_BytesToRead(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeI2CConfig_BytesToWrite(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeI2CConfig_DataToSendHigh(unsigned short value, tRioStatusCode *status) = 0;
   virtual void writeI2CConfig_BitwiseHandshake(bool value, tRioStatusCode *status) = 0;
   virtual tI2CConfig readI2CConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readI2CConfig_Address(tRioStatusCode *status) = 0;
   virtual unsigned char readI2CConfig_BytesToRead(tRioStatusCode *status) = 0;
   virtual unsigned char readI2CConfig_BytesToWrite(tRioStatusCode *status) = 0;
   virtual unsigned short readI2CConfig_DataToSendHigh(tRioStatusCode *status) = 0;
   virtual bool readI2CConfig_BitwiseHandshake(tRioStatusCode *status) = 0;



   typedef enum
   {
   } tLoopTiming_IfaceConstants;

   virtual unsigned short readLoopTiming(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tPWMConfig_IfaceConstants;

   virtual void writePWMConfig(tPWMConfig value, tRioStatusCode *status) = 0;
   virtual void writePWMConfig_Period(unsigned short value, tRioStatusCode *status) = 0;
   virtual void writePWMConfig_MinHigh(unsigned short value, tRioStatusCode *status) = 0;
   virtual tPWMConfig readPWMConfig(tRioStatusCode *status) = 0;
   virtual unsigned short readPWMConfig_Period(tRioStatusCode *status) = 0;
   virtual unsigned short readPWMConfig_MinHigh(tRioStatusCode *status) = 0;



   typedef enum
   {
      kNumPWMValueRegisters = 10,
   } tPWMValue_IfaceConstants;

   virtual void writePWMValue(unsigned char reg_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readPWMValue(unsigned char reg_index, tRioStatusCode *status) = 0;



private:
   tDIO(const tDIO&);
   void operator=(const tDIO&);
};

}
}

#endif // __nFRC_2012_1_6_4_DIO_h__
