#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary.h"
#include "HAL/HAL.hpp"
//#include "NetworkCommunication/FRCComm.h"
//#include "NetworkCommunication/UsageReporting.h"


/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_nAICalibration_getLSBWeight
 * Signature: (IILjava/lang/Integer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationAICalibrationGetLSBWeight
  (JNIEnv *, jclass, jint, jint, jobject)
{
	assert(false);
	return 0;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_nAICalibration_getOffset
 * Signature: (IILjava/lang/Integer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationAICalibrationGetOffset
  (JNIEnv *, jclass, jint, jint, jobject)
{
	assert(false);
	return 0;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    getModulePresence
 * Signature: (IB)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_getModulePresence
  (JNIEnv *, jclass, jint, jbyte)
{
	assert(false);
	return 0;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    getTargetClass
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_getTargetClass
  (JNIEnv *, jclass)
{
	assert(false);
	return 0;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_nLoadOut_getModulePresence
 * Signature: (IB)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationLoadOutGetModulePresence
  (JNIEnv *, jclass, jint, jbyte)
{
	assert(false);
	return 0;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_nLoadOut_getTargetClass
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationLoadOutGetTargetClass
  (JNIEnv *, jclass)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    report
 * Signature: (IBBLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_report
  (JNIEnv *, jclass, jint, jbyte, jbyte, jstring)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_nUsageReporting_report
 * Signature: (BBBLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationUsageReportingReport
  (JNIEnv * paramEnv, jclass, jbyte paramResource, jbyte paramInstanceNumber, jbyte paramContext, jstring paramFeature)
{
	FILE_LOG(logDEBUG) << "Calling FRCNetworkCommunicationsLibrary report " << "res:"<< (unsigned int)paramResource << " instance:" << (unsigned int)paramInstanceNumber << " context:" << (unsigned int)paramContext << " feature:" << paramFeature;
	const char * featureStr = paramEnv->GetStringUTFChars(paramFeature, NULL);
	jint returnValue = HALReport(paramResource, paramInstanceNumber, paramContext, featureStr);
	paramEnv->ReleaseStringUTFChars(paramFeature,featureStr);
	return returnValue;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    getFPGAHardwareVersion
 * Signature: (Ljava/nio/ShortBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_getFPGAHardwareVersion
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

//
// field ids
//
bool initializeComplete = false;
jclass dataClass;
jfieldID packetIndexFieldID;
jfieldID controlFieldID;
jfieldID dsDigitalInFieldID;
jfieldID teamIDFieldID;
jfieldID dsID_AllianceFieldID;
jfieldID dsID_PositionFieldID;
jfieldID stick0AxesFieldID;
jfieldID stick0ButtonsFieldID;
jfieldID stick1AxesFieldID;
jfieldID stick1ButtonsFieldID;
jfieldID stick2AxesFieldID;
jfieldID stick2ButtonsFieldID;
jfieldID stick3AxesFieldID;
jfieldID stick3ButtonsFieldID;
jfieldID analog1FieldID;
jfieldID analog2FieldID;
jfieldID analog3FieldID;
jfieldID analog4FieldID;
jfieldID cRIOChecksumFieldID;
jfieldID FPGAChecksum0FieldID;
jfieldID FPGAChecksum1FieldID;
jfieldID FPGAChecksum2FieldID;
jfieldID FPGAChecksum3FieldID;
jfieldID versionDataFieldID;

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    getCommonControlData
 * Signature: (Ledu/wpi/first/wpilibj/communication/FRCCommonControlData;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_getCommonControlData
  (JNIEnv * env, jclass, jobject controlData)
{
	HALCommonControlData controlDataNative;

	controlDataNative.packetIndex = 0xFF;
	controlDataNative.control = 0xFF;

	int returnValue = HALGetCommonControlData(&controlDataNative, HAL_WAIT_FOREVER);

	//FILE_LOG(logDEBUG) << "PacketIndex: " << (void*)controlDataNative.packetIndex << " Control: " << (void*)controlDataNative.control << " ReturnValue: " << (void*)returnValue;

	if(!initializeComplete)
	{
		// control data class
		dataClass = (jclass)env->NewGlobalRef(env->GetObjectClass(controlData));
		//dataClass = env->GetObjectClass(controlData);
		// public short packetIndex;
		packetIndexFieldID = env->GetFieldID(dataClass, "packetIndex", "S" );
		//	public byte control;
		controlFieldID = env->GetFieldID(dataClass, "control", "B" );
		//	public byte dsDigitalIn;
		dsDigitalInFieldID = env->GetFieldID(dataClass, "dsDigitalIn", "B" );
		//	public short teamID;
		teamIDFieldID = env->GetFieldID(dataClass, "teamID", "S" );
		//	public byte dsID_Alliance;
		dsID_AllianceFieldID = env->GetFieldID(dataClass, "dsID_Alliance", "B" );
		//	public byte dsID_Position;
		dsID_PositionFieldID = env->GetFieldID(dataClass, "dsID_Position", "B" );
		//	public byte[] stick0Axes;
		stick0AxesFieldID = env->GetFieldID(dataClass, "stick0Axes", "[B" );
		//	public short stick0Buttons;
		stick0ButtonsFieldID = env->GetFieldID(dataClass, "stick0Buttons", "S" );
		//	public byte[] stick1Axes;
		stick1AxesFieldID = env->GetFieldID(dataClass, "stick1Axes", "[B" );
		//	public short stick1Buttons;
		stick1ButtonsFieldID = env->GetFieldID(dataClass, "stick1Buttons", "S" );
		//	public byte[] stick2Axes;
		stick2AxesFieldID = env->GetFieldID(dataClass, "stick2Axes", "[B" );
		//	public short stick2Buttons;
		stick2ButtonsFieldID = env->GetFieldID(dataClass, "stick2Buttons", "S" );
		//	public byte[] stick3Axes;
		stick3AxesFieldID = env->GetFieldID(dataClass, "stick3Axes", "[B" );
		//	public short stick3Buttons;
		stick3ButtonsFieldID = env->GetFieldID(dataClass, "stick3Buttons", "S" );
		//	public short analog1;
		analog1FieldID = env->GetFieldID(dataClass, "analog1", "S" );
		//	public short analog2;
		analog2FieldID = env->GetFieldID(dataClass, "analog2", "S" );
		//	public short analog3;
		analog3FieldID = env->GetFieldID(dataClass, "analog3", "S" );
		//	public short analog4;
		analog4FieldID = env->GetFieldID(dataClass, "analog4", "S" );
		//	public long cRIOChecksum;
		cRIOChecksumFieldID = env->GetFieldID(dataClass, "cRIOChecksum", "J" );
		//	public int FPGAChecksum0;
		FPGAChecksum0FieldID = env->GetFieldID(dataClass, "FPGAChecksum0", "I" );
		//	public int FPGAChecksum1;
		FPGAChecksum1FieldID = env->GetFieldID(dataClass, "FPGAChecksum1", "I" );
		//	public int FPGAChecksum2;
		FPGAChecksum2FieldID = env->GetFieldID(dataClass, "FPGAChecksum2", "I" );
		//	public int FPGAChecksum3;
		FPGAChecksum3FieldID = env->GetFieldID(dataClass, "FPGAChecksum3", "I" );
		//	public byte[] versionData;
		versionDataFieldID = env->GetFieldID(dataClass, "versionData", "[B" );

		initializeComplete = true;
	}

	//FILE_LOG(logDEBUG) << "PacketIndex  : " << (short)controlDataNative.packetIndex;
	env->SetShortField(controlData,packetIndexFieldID, controlDataNative.packetIndex);
	//FILE_LOG(logDEBUG) << "Control  : " << (short)controlDataNative.control;
	env->SetByteField(controlData,controlFieldID, controlDataNative.control);
	env->SetByteField(controlData,dsDigitalInFieldID, controlDataNative.dsDigitalIn);
	env->SetShortField(controlData,teamIDFieldID, controlDataNative.teamID);
	env->SetByteField(controlData,dsID_AllianceFieldID, controlDataNative.dsID_Alliance);
	env->SetByteField(controlData,dsID_PositionFieldID, controlDataNative.dsID_Position);

	// process the axes for joystick 0
	jobject stick0Axes = env->GetObjectField(controlData, stick0AxesFieldID);
	jbyteArray * stick0AxesPtr = reinterpret_cast<jbyteArray*>(&stick0Axes);
	jbyte * stick0AxesBytePtr = env->GetByteArrayElements(*stick0AxesPtr,NULL);

	for( short axisCount = 0; axisCount < 6; axisCount++)
	{
		stick0AxesBytePtr[axisCount]=controlDataNative.stick0Axes[axisCount];
	}

	env->ReleaseByteArrayElements(*stick0AxesPtr,stick0AxesBytePtr,0);

	//	/** C type : field2_union */
	//	public field2_union field2;
	//	/** Left-most 4 bits are unused */
	env->SetShortField(controlData,stick0ButtonsFieldID, controlDataNative.stick0Buttons);

	// process the axes for joystick 1
	jobject stick1Axes = env->GetObjectField(controlData, stick1AxesFieldID);
	jbyteArray * stick1AxesPtr = reinterpret_cast<jbyteArray*>(&stick1Axes);
	jbyte * stick1AxesBytePtr = env->GetByteArrayElements(*stick1AxesPtr,NULL);

	for( short axisCount = 0; axisCount < 6; axisCount++)
	{
		stick1AxesBytePtr[axisCount]=controlDataNative.stick1Axes[axisCount];
	}

	env->ReleaseByteArrayElements(*stick1AxesPtr,stick1AxesBytePtr,0);

	//	/** C type : field3_union */
	//	public field3_union field3;
	//	/** Left-most 4 bits are unused */
	env->SetShortField(controlData,stick1ButtonsFieldID, controlDataNative.stick1Buttons);

	// process the axes for joystick 2
	jobject stick2Axes = env->GetObjectField(controlData, stick2AxesFieldID);
	jbyteArray * stick2AxesPtr = reinterpret_cast<jbyteArray*>(&stick2Axes);
	jbyte * stick2AxesBytePtr = env->GetByteArrayElements(*stick2AxesPtr,NULL);

	for( short axisCount = 0; axisCount < 6; axisCount++)
	{
		stick2AxesBytePtr[axisCount]=controlDataNative.stick2Axes[axisCount];
	}

	env->ReleaseByteArrayElements(*stick2AxesPtr,stick2AxesBytePtr,0);

	//	/** C type : field4_union */
	//	public field4_union field4;
	//	/** Left-most 4 bits are unused */
	env->SetShortField(controlData,stick2ButtonsFieldID, controlDataNative.stick2Buttons);

	// process the axes for joystick 3
	jobject stick3Axes = env->GetObjectField(controlData, stick3AxesFieldID);
	jbyteArray * stick3AxesPtr = reinterpret_cast<jbyteArray*>(&stick3Axes);
	jbyte * stick3AxesBytePtr = env->GetByteArrayElements(*stick3AxesPtr,NULL);

	for( short axisCount = 0; axisCount < 6; axisCount++)
	{
		stick3AxesBytePtr[axisCount]=controlDataNative.stick3Axes[axisCount];
	}

	env->ReleaseByteArrayElements(*stick3AxesPtr,stick3AxesBytePtr,0);

	//	/** C type : field5_union */
	//	public field5_union field5;
	//	/** Left-most 4 bits are unused */
	env->SetShortField(controlData,stick3ButtonsFieldID, controlDataNative.stick3Buttons);
	//	/** Analog inputs are 10 bit right-justified */
	env->SetShortField(controlData,analog1FieldID, controlDataNative.analog1);
	env->SetShortField(controlData,analog2FieldID, controlDataNative.analog2);
	env->SetShortField(controlData,analog3FieldID, controlDataNative.analog3);
	env->SetShortField(controlData,analog4FieldID, controlDataNative.analog4);
	env->SetLongField(controlData,cRIOChecksumFieldID, controlDataNative.cRIOChecksum);
	env->SetIntField(controlData,FPGAChecksum0FieldID, controlDataNative.FPGAChecksum0);
	env->SetIntField(controlData,FPGAChecksum1FieldID, controlDataNative.FPGAChecksum1);
	env->SetIntField(controlData,FPGAChecksum2FieldID, controlDataNative.FPGAChecksum2);
	env->SetIntField(controlData,FPGAChecksum3FieldID, controlDataNative.FPGAChecksum3);
	//	/** C type : char[8] */
	//	public byte[] versionData;
	jobject versionData = env->GetObjectField(controlData, versionDataFieldID);
	jbyteArray * versionDataPtr = reinterpret_cast<jbyteArray*>(&versionData);
	jbyte * versionDataBytePtr = env->GetByteArrayElements(*versionDataPtr,NULL);

	for( short byteCount = 0; byteCount < 8; byteCount++)
	{
		versionDataBytePtr[byteCount]=controlDataNative.versionData[byteCount];
	}

	env->ReleaseByteArrayElements(*versionDataPtr,versionDataBytePtr,0);

	return returnValue;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    getRecentCommonControlData
 * Signature: (Ledu/wpi/first/wpilibj/communication/FRCCommonControlData;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_getRecentCommonControlData
  (JNIEnv *, jclass, jobject, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    getRecentStatusData
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/ByteBuffer;Ljava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_getRecentStatusData
  (JNIEnv *, jclass, jobject, jobject, jobject, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    getDynamicControlData
 * Signature: (BLjava/nio/ByteBuffer;II)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_getDynamicControlData
  (JNIEnv *, jclass, jbyte, jobject, jint, jint)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    setStatusData
 * Signature: (FBBLjava/lang/String;ILjava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setStatusData
  (JNIEnv * env, jclass, jfloat paramBattery, jbyte param1, jbyte param2, jstring paramUserDataHigh, jint param4, jstring paramUserDataLow, jint param6)
{
	//FILE_LOG(logDEBUG) << "Voltage - " << paramBattery;
	const char * userDataHighStr = env->GetStringUTFChars(paramUserDataHigh, NULL);
	const char * userDataLowStr = env->GetStringUTFChars(paramUserDataLow, NULL);
	jint returnValue = HALSetStatusData(paramBattery, param1, param2, userDataHighStr, param4, userDataLowStr, param6, HAL_WAIT_FOREVER );
	env->ReleaseStringUTFChars(paramUserDataHigh,userDataHighStr);
	env->ReleaseStringUTFChars(paramUserDataLow,userDataLowStr);
	return returnValue;
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    setStatusDataFloatAsInt
 * Signature: (IBBLjava/lang/String;ILjava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setStatusDataFloatAsInt
  (JNIEnv *, jclass, jint, jbyte, jbyte, jstring, jint, jstring, jint, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    setErrorData
 * Signature: (Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setErrorData
  (JNIEnv *, jclass, jstring, jint, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    setUserDsLcdData
 * Signature: (Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setUserDsLcdData
  (JNIEnv *, jclass, jstring, jint, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    overrideIOConfig
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_overrideIOConfig
  (JNIEnv *, jclass, jstring, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    signalResyncActionDone
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_signalResyncActionDone
  (JNIEnv *, jclass)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    setNewDataOccurRef
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setNewDataOccurRef
  (JNIEnv *, jclass, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    setResyncOccurRef
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setResyncOccurRef
  (JNIEnv *, jclass, jint)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_getVersionString
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationGetVersionString
  (JNIEnv *, jclass, jobject)
{
	assert(false);
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_observeUserProgramStarting
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationObserveUserProgramStarting
  (JNIEnv *, jclass)
{
	HALNetworkCommunicationObserveUserProgramStarting();
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_observeUserProgramDisabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationObserveUserProgramDisabled
  (JNIEnv *, jclass)
{
	HALNetworkCommunicationObserveUserProgramDisabled();
}
/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_observeUserProgramAutonomous
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationObserveUserProgramAutonomous
  (JNIEnv *, jclass)
{
	HALNetworkCommunicationObserveUserProgramAutonomous();
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_observeUserProgramTeleop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationObserveUserProgramTeleop
  (JNIEnv *, jclass)
{
	HALNetworkCommunicationObserveUserProgramTeleop();
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRC_NetworkCommunicationsLibrary
 * Method:    FRC_NetworkCommunication_observeUserProgramTest
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationObserveUserProgramTest
  (JNIEnv *, jclass)
{
	HALNetworkCommunicationObserveUserProgramTest();
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    FRCNetworkCommunicationReserve
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_FRCNetworkCommunicationReserve
  (JNIEnv *, jclass)
{
	assert(1 == HALInitialize(0));
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    setNewDataSem
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setNewDataSem
  (JNIEnv * env, jclass, jobject id )
{
	MUTEX_ID* javaId = (MUTEX_ID*)env->GetDirectBufferAddress(id);
	FILE_LOG(logDEBUG) << "Mutex Ptr = " << *javaId;
	HALSetNewDataSem(*javaId);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueParameterTest
 * Signature: (ZBCSIJFD)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueParameterTest
  (JNIEnv *, jclass, jboolean booleanParam, jbyte byteParam, jchar charParam, jshort shortParam, jint intParam, jlong longParam, jfloat floatParam, jdouble doubleParam)
{
	FILE_LOG(logDEBUG) << "Boolean: " << booleanParam;
	FILE_LOG(logDEBUG) << "Byte   : " << byteParam;
	FILE_LOG(logDEBUG) << "Char   : " << charParam;
	FILE_LOG(logDEBUG) << "Short  : " << shortParam;
	FILE_LOG(logDEBUG) << "Int    : " << intParam;
	FILE_LOG(logDEBUG) << "Long   : " << longParam;
	FILE_LOG(logDEBUG) << "Float  : " << floatParam;
	FILE_LOG(logDEBUG) << "Double : " << doubleParam;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnBooleanTest
 * Signature: (Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnBooleanTest
  (JNIEnv *, jclass, jboolean booleanParam )
{
	FILE_LOG(logDEBUG) << "Boolean: " << booleanParam;
	return !booleanParam;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnByteTest
 * Signature: (B)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnByteTest
  (JNIEnv *, jclass, jbyte byteParam)
{
	FILE_LOG(logDEBUG) << "Byte: " << byteParam;
	return byteParam+1;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnCharTest
 * Signature: (C)C
 */
JNIEXPORT jchar JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnCharTest
  (JNIEnv *, jclass, jchar charParam)
{
	FILE_LOG(logDEBUG) << "Char: " << charParam;
	return charParam+1;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnShortTest
 * Signature: (S)S
 */
JNIEXPORT jshort JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnShortTest
  (JNIEnv *, jclass, jshort shortParam)
{
	FILE_LOG(logDEBUG) << "Short: " << shortParam;
	return shortParam+1;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnIntTest
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnIntTest
  (JNIEnv *, jclass, jint intParam)
{
	FILE_LOG(logDEBUG) << "Int: " << intParam;
	return intParam+1;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnLongTest
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnLongTest
  (JNIEnv *, jclass, jlong longParam)
{
	FILE_LOG(logDEBUG) << "Long: " << longParam;
	return longParam+1;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnFloatTest
 * Signature: (F)F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnFloatTest
  (JNIEnv *, jclass, jfloat floatParam)
{
	FILE_LOG(logDEBUG) << "Float: " << floatParam;
	return floatParam/100.0f;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnDoubleTest
 * Signature: (D)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnDoubleTest
  (JNIEnv *, jclass, jdouble doubleParam)
{
	FILE_LOG(logDEBUG) << "Double: " << doubleParam;
	return doubleParam * 100.0;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIObjectReturnString
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIObjectReturnStringTest
  (JNIEnv * env, jclass, jstring stringParam)
{
	const char * stringParamLocal = env->GetStringUTFChars(stringParam, NULL);
	FILE_LOG(logDEBUG) << "String: " << stringParamLocal;
	env->ReleaseStringUTFChars(stringParam,stringParamLocal);

	char returnStringLocal[] = "this is the return string";

	return env->NewStringUTF(returnStringLocal);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIObjectReturnByteBufferTest
 * Signature: (Ljava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIObjectReturnByteBufferTest
  (JNIEnv * env, jclass, jobject byteArrayIn )
{
	jbyte * byteArray = (jbyte*)env->GetDirectBufferAddress(byteArrayIn);
	FILE_LOG(logDEBUG) << "Ptr: " << (long)byteArray;
	jlong byteArrayLength = env->GetDirectBufferCapacity(byteArrayIn);
	FILE_LOG(logDEBUG) << "Capacity: " << byteArrayLength;
	FILE_LOG(logDEBUG) << "Byte0: " << (short)byteArray[0];
	FILE_LOG(logDEBUG) << "Byte1: " << (short)byteArray[1];
	FILE_LOG(logDEBUG) << "Byte2: " << (short)byteArray[2];
	FILE_LOG(logDEBUG) << "Byte3: " << (short)byteArray[3];

	jbyte * returnByteArray = new jbyte[4];
	returnByteArray[0] = byteArray[0];
	returnByteArray[1] = byteArray[1];
	returnByteArray[2] = byteArray[2];
	returnByteArray[3] = byteArray[3];

	return env->NewDirectByteBuffer(returnByteArray, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIObjectAndParamReturnIntBufferTest
 * Signature: (Ljava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIObjectAndParamReturnIntBufferTest
  (JNIEnv * env, jclass, jobject intArrayIn)
{
	jint * intArray = (jint*)env->GetDirectBufferAddress(intArrayIn);
	jbyte * byteArray = (jbyte*)env->GetDirectBufferAddress(intArrayIn);
	FILE_LOG(logDEBUG) << "Ptr: " << (long)intArray;
	jlong byteArrayLength = env->GetDirectBufferCapacity(intArrayIn);
	FILE_LOG(logDEBUG) << "Capacity: " << byteArrayLength;
	FILE_LOG(logDEBUG) << "Int0: " << intArray[0];
	FILE_LOG(logDEBUG) << "Byte0: " << (short)byteArray[0];
	FILE_LOG(logDEBUG) << "Byte1: " << (short)byteArray[1];
	FILE_LOG(logDEBUG) << "Byte2: " << (short)byteArray[2];
	FILE_LOG(logDEBUG) << "Byte3: " << (short)byteArray[3];

	// increment the param
	intArray[0]++;

	jbyte * returnByteArray = new jbyte[4];
	returnByteArray[0] = byteArray[0];
	returnByteArray[1] = byteArray[1];
	returnByteArray[2] = byteArray[2];
	returnByteArray[3] = byteArray[3];

	return env->NewDirectByteBuffer(returnByteArray, 4);
}
