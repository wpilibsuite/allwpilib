#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary.h"
#include "HAL/HAL.hpp"
//#include "NetworkCommunication/FRCComm.h"
//#include "NetworkCommunication/UsageReporting.h"

// set the logging level
TLogLevel netCommLogLevel = logWARNING;

#define NETCOMM_LOG(level) \
    if (level > netCommLogLevel) ; \
    else Log().Get(level)



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
	const char * featureStr = paramEnv->GetStringUTFChars(paramFeature, NULL);
	NETCOMM_LOG(logDEBUG) << "Calling FRCNetworkCommunicationsLibrary report " << "res:"<< (unsigned int)paramResource << " instance:" << (unsigned int)paramInstanceNumber << " context:" << (unsigned int)paramContext << " feature:" << featureStr;
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
 * Method:    NativeHALGetControlWord
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_NativeHALGetControlWord
  (JNIEnv *, jclass)
{
	NETCOMM_LOG(logDEBUG) << "Calling HAL Control Word";
    jint controlWord;
    HALGetControlWord((HALControlWord*)&controlWord);
    return controlWord;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    NativeHALGetAllianceStation
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_NativeHALGetAllianceStation
  (JNIEnv *, jclass)
{
	NETCOMM_LOG(logDEBUG) << "Calling HAL Alliance Station";
    jint allianceStation;
    HALGetAllianceStation((HALAllianceStationID*)&allianceStation);
    return allianceStation;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetJoystickAxes
 * Signature: (B)[S
 */
JNIEXPORT jshortArray JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetJoystickAxes
  (JNIEnv * env, jclass, jbyte joystickNum)
{
	NETCOMM_LOG(logDEBUG) << "Calling HALJoystickAxes";
    HALJoystickAxes axes;
    HALGetJoystickAxes(joystickNum, &axes);

    jshortArray axesArray = env->NewShortArray(axes.count);
    env->SetShortArrayRegion(axesArray, 0, axes.count, axes.axes);

    return axesArray;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetJoystickPOVs
 * Signature: (B)[S
 */
JNIEXPORT jshortArray JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetJoystickPOVs
  (JNIEnv * env, jclass, jbyte joystickNum)
{
	NETCOMM_LOG(logDEBUG) << "Calling HALJoystickPOVs";
    HALJoystickPOVs povs;
    HALGetJoystickPOVs(joystickNum, &povs);

    jshortArray povsArray = env->NewShortArray(povs.count);
    env->SetShortArrayRegion(povsArray, 0, povs.count, povs.povs);

    return povsArray;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetJoystickButtons
 * Signature: (B)S
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetJoystickButtons
  (JNIEnv * env, jclass, jbyte joystickNum, jobject count)
{
	NETCOMM_LOG(logDEBUG) << "Calling HALJoystickButtons";
    HALJoystickButtons joystickButtons;
    HALGetJoystickButtons(joystickNum, &joystickButtons);
	jbyte *countPtr = (jbyte*)env->GetDirectBufferAddress(count);
	NETCOMM_LOG(logDEBUG) << "Buttons = " << joystickButtons.buttons;
	NETCOMM_LOG(logDEBUG) << "Count = " << (jint)joystickButtons.count;
	*countPtr = joystickButtons.count;
	NETCOMM_LOG(logDEBUG) << "CountBuffer = " << (jint)*countPtr;
    return joystickButtons.buttons;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALSetJoystickOutputs
 * Signature: (BISS)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALSetJoystickOutputs
  (JNIEnv *, jclass, jbyte port, jint outputs, jshort leftRumble, jshort rightRumble)
{
	NETCOMM_LOG(logDEBUG) << "Calling HALSetJoystickOutputs on port " << port;
	NETCOMM_LOG(logDEBUG) << "Outputs: " << outputs;
	NETCOMM_LOG(logDEBUG) << "Left Rumble: " << leftRumble << " Right Rumble: " << rightRumble;
	return HALSetJoystickOutputs(port, outputs, leftRumble, rightRumble);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetJoystickIsXbox
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetJoystickIsXbox
  (JNIEnv *, jclass, jbyte port)
  {
	NETCOMM_LOG(logDEBUG) << "Calling HALGetJoystickIsXbox";
	return HALGetJoystickIsXbox(port);
  }

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetJoystickType
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetJoystickType
  (JNIEnv *, jclass, jbyte port)
  {
	NETCOMM_LOG(logDEBUG) << "Calling HALGetJoystickType";
	return HALGetJoystickType(port);
  }

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetJoystickName
 * Signature: (B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetJoystickName
  (JNIEnv * env, jclass, jbyte port)
  {
	NETCOMM_LOG(logDEBUG) << "Calling HALGetJoystickName";
	return env->NewStringUTF(HALGetJoystickName(port));
  }

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    setNewDataSem
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_setNewDataSem
  (JNIEnv * env, jclass, jobject id )
{
	MULTIWAIT_ID javaId = (MULTIWAIT_ID)env->GetDirectBufferAddress(id);
	NETCOMM_LOG(logDEBUG) << "Mutex Ptr = " << javaId;
	HALSetNewDataSem(javaId->native_handle());
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueParameterTest
 * Signature: (ZBCSIJFD)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueParameterTest
  (JNIEnv *, jclass, jboolean booleanParam, jbyte byteParam, jchar charParam, jshort shortParam, jint intParam, jlong longParam, jfloat floatParam, jdouble doubleParam)
{
	NETCOMM_LOG(logDEBUG) << "Boolean: " << booleanParam;
	NETCOMM_LOG(logDEBUG) << "Byte   : " << byteParam;
	NETCOMM_LOG(logDEBUG) << "Char   : " << charParam;
	NETCOMM_LOG(logDEBUG) << "Short  : " << shortParam;
	NETCOMM_LOG(logDEBUG) << "Int    : " << intParam;
	NETCOMM_LOG(logDEBUG) << "Long   : " << longParam;
	NETCOMM_LOG(logDEBUG) << "Float  : " << floatParam;
	NETCOMM_LOG(logDEBUG) << "Double : " << doubleParam;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    JNIValueReturnBooleanTest
 * Signature: (Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_JNIValueReturnBooleanTest
  (JNIEnv *, jclass, jboolean booleanParam )
{
	NETCOMM_LOG(logDEBUG) << "Boolean: " << booleanParam;
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
	NETCOMM_LOG(logDEBUG) << "Byte: " << byteParam;
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
	NETCOMM_LOG(logDEBUG) << "Char: " << charParam;
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
	NETCOMM_LOG(logDEBUG) << "Short: " << shortParam;
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
	NETCOMM_LOG(logDEBUG) << "Int: " << intParam;
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
	NETCOMM_LOG(logDEBUG) << "Long: " << longParam;
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
	NETCOMM_LOG(logDEBUG) << "Float: " << floatParam;
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
	NETCOMM_LOG(logDEBUG) << "Double: " << doubleParam;
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
	NETCOMM_LOG(logDEBUG) << "String: " << stringParamLocal;
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
	NETCOMM_LOG(logDEBUG) << "Ptr: " << (long)byteArray;
	jlong byteArrayLength = env->GetDirectBufferCapacity(byteArrayIn);
	NETCOMM_LOG(logDEBUG) << "Capacity: " << byteArrayLength;
	NETCOMM_LOG(logDEBUG) << "Byte0: " << (short)byteArray[0];
	NETCOMM_LOG(logDEBUG) << "Byte1: " << (short)byteArray[1];
	NETCOMM_LOG(logDEBUG) << "Byte2: " << (short)byteArray[2];
	NETCOMM_LOG(logDEBUG) << "Byte3: " << (short)byteArray[3];

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
	NETCOMM_LOG(logDEBUG) << "Ptr: " << (long)intArray;
	jlong byteArrayLength = env->GetDirectBufferCapacity(intArrayIn);
	NETCOMM_LOG(logDEBUG) << "Capacity: " << byteArrayLength;
	NETCOMM_LOG(logDEBUG) << "Int0: " << intArray[0];
	NETCOMM_LOG(logDEBUG) << "Byte0: " << (short)byteArray[0];
	NETCOMM_LOG(logDEBUG) << "Byte1: " << (short)byteArray[1];
	NETCOMM_LOG(logDEBUG) << "Byte2: " << (short)byteArray[2];
	NETCOMM_LOG(logDEBUG) << "Byte3: " << (short)byteArray[3];

	// increment the param
	intArray[0]++;

	jbyte * returnByteArray = new jbyte[4];
	returnByteArray[0] = byteArray[0];
	returnByteArray[1] = byteArray[1];
	returnByteArray[2] = byteArray[2];
	returnByteArray[3] = byteArray[3];

	return env->NewDirectByteBuffer(returnByteArray, 4);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary

 * Method:    HALGetMatchTime
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetMatchTime
  (JNIEnv * env, jclass)
{
	jfloat matchTime;
	HALGetMatchTime((float*)&matchTime);
	return matchTime;
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetSystemActive
 * Signature: (Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetSystemActive
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return HALGetSystemActive((int32_t*)statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALGetBrownedOut
 * Signature: (Ljava/nio/IntBuffer;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALGetBrownedOut
  (JNIEnv * env, jclass, jobject status)
{
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	return HALGetBrownedOut((int32_t*)statusPtr);
}

/*
 * Class:     edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary
 * Method:    HALSetErrorData
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_communication_FRCNetworkCommunicationsLibrary_HALSetErrorData
  (JNIEnv * env, jclass, jstring error)
{
  const char * errorStr = env->GetStringUTFChars(error, NULL);
  jsize length = env->GetStringUTFLength(error);

  NETCOMM_LOG(logDEBUG) << "Set Error: " << errorStr;
  NETCOMM_LOG(logDEBUG) << "Length: " << length;
  jint returnValue = HALSetErrorData(errorStr, (jint) length, 0);
  env->ReleaseStringUTFChars(error,errorStr);
  return returnValue;
}

