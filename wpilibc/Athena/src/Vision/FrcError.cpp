/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nivision.h"
#include "Vision/FrcError.h"

/**
 * Get the error code returned from the NI Vision library
 * @return The last error code.
 */
int GetLastVisionError() {
  // int errorCode = imaqGetLastVisionError();     // error code: 0 = no error
  // char* errorText = GetVisionErrorText(errorCode);
  // dprintf (LOG_DEBUG, "Error = %i  %s ", errorCode, errorText);
  return imaqGetLastError();
}

/**
* Get the error text for an NI Vision error code.
* Note: imaqGetErrorText() is not supported on real time system, so
* so relevant strings are hardcoded here - the maintained version is
* in the LabWindows/CVI help file.
* @param errorCode The error code to find the text for.
* @return The error text
*/
const char* GetVisionErrorText(int errorCode) {
  const char* errorText;

  switch (errorCode) {
    default: {
      errorText = "UNKNOWN_ERROR";
      break;
    }
    case -1074395138: {
      errorText = "ERR_OCR_REGION_TOO_SMALL";
      break;
    }
    case -1074395139: {
      errorText = "ERR_IMAQ_QR_DIMENSION_INVALID";
      break;
    }
    case -1074395140: {
      errorText = "ERR_OCR_CHAR_REPORT_CORRUPTED";
      break;
    }
    case -1074395141: {
      errorText = "ERR_OCR_NO_TEXT_FOUND";
      break;
    }
    case -1074395142: {
      errorText = "ERR_QR_DETECTION_MODELTYPE";
      break;
    }
    case -1074395143: {
      errorText = "ERR_QR_DETECTION_MODE";
      break;
    }
    case -1074395144: {
      errorText = "ERR_QR_INVALID_BARCODE";
      break;
    }
    case -1074395145: {
      errorText = "ERR_QR_INVALID_READ";
      break;
    }
    case -1074395146: {
      errorText = "ERR_QR_DETECTION_VERSION";
      break;
    }
    case -1074395147: {
      errorText = "ERR_BARCODE_RSSLIMITED";
      break;
    }
    case -1074395148: {
      errorText = "ERR_OVERLAY_GROUP_NOT_FOUND";
      break;
    }
    case -1074395149: {
      errorText = "ERR_DUPLICATE_TRANSFORM_TYPE";
      break;
    }
    case -1074395151: {
      errorText = "ERR_OCR_CORRECTION_FAILED";
      break;
    }
    case -1074395155: {
      errorText = "ERR_OCR_ORIENT_DETECT_FAILED";
      break;
    }
    case -1074395156: {
      errorText = "ERR_OCR_SKEW_DETECT_FAILED";
      break;
    }
    case -1074395158: {
      errorText = "ERR_OCR_INVALID_CONTRASTMODE";
      break;
    }
    case -1074395159: {
      errorText = "ERR_OCR_INVALID_TOLERANCE";
      break;
    }
    case -1074395160: {
      errorText = "ERR_OCR_INVALID_MAXPOINTSIZE";
      break;
    }
    case -1074395161: {
      errorText = "ERR_OCR_INVALID_CORRECTIONLEVEL";
      break;
    }
    case -1074395162: {
      errorText = "ERR_OCR_INVALID_CORRECTIONMODE";
      break;
    }
    case -1074395163: {
      errorText = "ERR_OCR_INVALID_CHARACTERPREFERENCE";
      break;
    }
    case -1074395164: {
      errorText = "ERR_OCR_ADD_WORD_FAILED";
      break;
    }
    case -1074395165: {
      errorText = "ERR_OCR_WTS_DIR_NOT_FOUND";
      break;
    }
    case -1074395166: {
      errorText = "ERR_OCR_BIN_DIR_NOT_FOUND";
      break;
    }
    case -1074395167: {
      errorText = "ERR_OCR_INVALID_OUTPUTDELIMITER";
      break;
    }
    case -1074395168: {
      errorText = "ERR_OCR_INVALID_AUTOCORRECTIONMODE";
      break;
    }
    case -1074395169: {
      errorText = "ERR_OCR_INVALID_RECOGNITIONMODE";
      break;
    }
    case -1074395170: {
      errorText = "ERR_OCR_INVALID_CHARACTERTYPE";
      break;
    }
    case -1074395171: {
      errorText = "ERR_OCR_INI_FILE_NOT_FOUND";
      break;
    }
    case -1074395172: {
      errorText = "ERR_OCR_INVALID_CHARACTERSET";
      break;
    }
    case -1074395173: {
      errorText = "ERR_OCR_INVALID_LANGUAGE";
      break;
    }
    case -1074395174: {
      errorText = "ERR_OCR_INVALID_AUTOORIENTMODE";
      break;
    }
    case -1074395175: {
      errorText = "ERR_OCR_BAD_USER_DICTIONARY";
      break;
    }
    case -1074395178: {
      errorText = "ERR_OCR_RECOGNITION_FAILED";
      break;
    }
    case -1074395179: {
      errorText = "ERR_OCR_PREPROCESSING_FAILED";
      break;
    }
    case -1074395200: {
      errorText = "ERR_OCR_INVALID_PARAMETER";
      break;
    }
    case -1074395201: {
      errorText = "ERR_OCR_LOAD_LIBRARY";
      break;
    }
    case -1074395203: {
      errorText = "ERR_OCR_LIB_INIT";
      break;
    }
    case -1074395210: {
      errorText = "ERR_OCR_CANNOT_MATCH_TEXT_TEMPLATE";
      break;
    }
    case -1074395211: {
      errorText = "ERR_OCR_BAD_TEXT_TEMPLATE";
      break;
    }
    case -1074395212: {
      errorText = "ERR_OCR_TEMPLATE_WRONG_SIZE";
      break;
    }
    case -1074395233: {
      errorText = "ERR_TEMPLATE_IMAGE_TOO_LARGE";
      break;
    }
    case -1074395234: {
      errorText = "ERR_TEMPLATE_IMAGE_TOO_SMALL";
      break;
    }
    case -1074395235: {
      errorText = "ERR_TEMPLATE_IMAGE_CONTRAST_TOO_LOW";
      break;
    }
    case -1074395237: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_SHIFT_1";
      break;
    }
    case -1074395238: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_NOSHIFT";
      break;
    }
    case -1074395239: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_SHIFT";
      break;
    }
    case -1074395240: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_ROTATION_1";
      break;
    }
    case -1074395241: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_NOROTATION";
      break;
    }
    case -1074395242: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_ROTATION";
      break;
    }
    case -1074395243: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_4";
      break;
    }
    case -1074395244: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_3";
      break;
    }
    case -1074395245: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_2";
      break;
    }
    case -1074395246: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR_1";
      break;
    }
    case -1074395247: {
      errorText = "ERR_TEMPLATE_DESCRIPTOR";
      break;
    }
    case -1074395248: {
      errorText = "ERR_TOO_MANY_ROTATION_ANGLE_RANGES";
      break;
    }
    case -1074395249: {
      errorText = "ERR_ROTATION_ANGLE_RANGE_TOO_LARGE";
      break;
    }
    case -1074395250: {
      errorText = "ERR_MATCH_SETUP_DATA";
      break;
    }
    case -1074395251: {
      errorText = "ERR_INVALID_MATCH_MODE";
      break;
    }
    case -1074395252: {
      errorText = "ERR_LEARN_SETUP_DATA";
      break;
    }
    case -1074395253: {
      errorText = "ERR_INVALID_LEARN_MODE";
      break;
    }
    case -1074395256: {
      errorText = "ERR_EVEN_WINDOW_SIZE";
      break;
    }
    case -1074395257: {
      errorText = "ERR_INVALID_EDGE_DIR";
      break;
    }
    case -1074395258: {
      errorText = "ERR_BAD_FILTER_WIDTH";
      break;
    }
    case -1074395260: {
      errorText = "ERR_HEAP_TRASHED";
      break;
    }
    case -1074395261: {
      errorText = "ERR_GIP_RANGE";
      break;
    }
    case -1074395262: {
      errorText = "ERR_LCD_BAD_MATCH";
      break;
    }
    case -1074395263: {
      errorText = "ERR_LCD_NO_SEGMENTS";
      break;
    }
    case -1074395265: {
      errorText = "ERR_BARCODE";
      break;
    }
    case -1074395267: {
      errorText = "ERR_COMPLEX_ROOT";
      break;
    }
    case -1074395268: {
      errorText = "ERR_LINEAR_COEFF";
      break;
    }
    case -1074395269: {
      errorText = "ERR_nullptr_POINTER";
      break;
    }
    case -1074395270: {
      errorText = "ERR_DIV_BY_ZERO";
      break;
    }
    case -1074395275: {
      errorText = "ERR_INVALID_BROWSER_IMAGE";
      break;
    }
    case -1074395276: {
      errorText = "ERR_LINES_PARALLEL";
      break;
    }
    case -1074395277: {
      errorText = "ERR_BARCODE_CHECKSUM";
      break;
    }
    case -1074395278: {
      errorText = "ERR_LCD_NOT_NUMERIC";
      break;
    }
    case -1074395279: {
      errorText = "ERR_ROI_NOT_POLYGON";
      break;
    }
    case -1074395280: {
      errorText = "ERR_ROI_NOT_RECT";
      break;
    }
    case -1074395281: {
      errorText = "ERR_IMAGE_SMALLER_THAN_BORDER";
      break;
    }
    case -1074395282: {
      errorText = "ERR_CANT_DRAW_INTO_VIEWER";
      break;
    }
    case -1074395283: {
      errorText = "ERR_INVALID_RAKE_DIRECTION";
      break;
    }
    case -1074395284: {
      errorText = "ERR_INVALID_EDGE_PROCESS";
      break;
    }
    case -1074395285: {
      errorText = "ERR_INVALID_SPOKE_DIRECTION";
      break;
    }
    case -1074395286: {
      errorText = "ERR_INVALID_CONCENTRIC_RAKE_DIRECTION";
      break;
    }
    case -1074395287: {
      errorText = "ERR_INVALID_LINE";
      break;
    }
    case -1074395290: {
      errorText = "ERR_SHAPEMATCH_BADTEMPLATE";
      break;
    }
    case -1074395291: {
      errorText = "ERR_SHAPEMATCH_BADIMAGEDATA";
      break;
    }
    case -1074395292: {
      errorText = "ERR_POINTS_ARE_COLLINEAR";
      break;
    }
    case -1074395293: {
      errorText = "ERR_CONTOURID_NOT_FOUND";
      break;
    }
    case -1074395294: {
      errorText = "ERR_CONTOUR_INDEX_OUT_OF_RANGE";
      break;
    }
    case -1074395295: {
      errorText = "ERR_INVALID_INTERPOLATIONMETHOD_INTERPOLATEPOINTS";
      break;
    }
    case -1074395296: {
      errorText = "ERR_INVALID_BARCODETYPE";
      break;
    }
    case -1074395297: {
      errorText = "ERR_INVALID_PARTICLEINFOMODE";
      break;
    }
    case -1074395298: {
      errorText = "ERR_COMPLEXPLANE_NOT_REAL_OR_IMAGINARY";
      break;
    }
    case -1074395299: {
      errorText = "ERR_INVALID_COMPLEXPLANE";
      break;
    }
    case -1074395300: {
      errorText = "ERR_INVALID_METERARCMODE";
      break;
    }
    case -1074395301: {
      errorText = "ERR_ROI_NOT_2_LINES";
      break;
    }
    case -1074395302: {
      errorText = "ERR_INVALID_THRESHOLDMETHOD";
      break;
    }
    case -1074395303: {
      errorText = "ERR_INVALID_NUM_OF_CLASSES";
      break;
    }
    case -1074395304: {
      errorText = "ERR_INVALID_MATHTRANSFORMMETHOD";
      break;
    }
    case -1074395305: {
      errorText = "ERR_INVALID_REFERENCEMODE";
      break;
    }
    case -1074395306: {
      errorText = "ERR_INVALID_TOOL";
      break;
    }
    case -1074395307: {
      errorText = "ERR_PRECISION_NOT_GTR_THAN_0";
      break;
    }
    case -1074395308: {
      errorText = "ERR_INVALID_COLORSENSITIVITY";
      break;
    }
    case -1074395309: {
      errorText = "ERR_INVALID_WINDOW_THREAD_POLICY";
      break;
    }
    case -1074395310: {
      errorText = "ERR_INVALID_PALETTE_TYPE";
      break;
    }
    case -1074395311: {
      errorText = "ERR_INVALID_COLOR_SPECTRUM";
      break;
    }
    case -1074395312: {
      errorText = "ERR_LCD_CALIBRATE";
      break;
    }
    case -1074395313: {
      errorText = "ERR_WRITE_FILE_NOT_SUPPORTED";
      break;
    }
    case -1074395316: {
      errorText = "ERR_INVALID_KERNEL_CODE";
      break;
    }
    case -1074395317: {
      errorText = "ERR_UNDEF_POINT";
      break;
    }
    case -1074395318: {
      errorText = "ERR_INSF_POINTS";
      break;
    }
    case -1074395319: {
      errorText = "ERR_INVALID_SUBPIX_TYPE";
      break;
    }
    case -1074395320: {
      errorText = "ERR_TEMPLATE_EMPTY";
      break;
    }
    case -1074395321: {
      errorText = "ERR_INVALID_MORPHOLOGYMETHOD";
      break;
    }
    case -1074395322: {
      errorText = "ERR_INVALID_TEXTALIGNMENT";
      break;
    }
    case -1074395323: {
      errorText = "ERR_INVALID_FONTCOLOR";
      break;
    }
    case -1074395324: {
      errorText = "ERR_INVALID_SHAPEMODE";
      break;
    }
    case -1074395325: {
      errorText = "ERR_INVALID_DRAWMODE";
      break;
    }
    case -1074395326: {
      errorText = "ERR_INVALID_DRAWMODE_FOR_LINE";
      break;
    }
    case -1074395327: {
      errorText = "ERR_INVALID_SCALINGMODE";
      break;
    }
    case -1074395328: {
      errorText = "ERR_INVALID_INTERPOLATIONMETHOD";
      break;
    }
    case -1074395329: {
      errorText = "ERR_INVALID_OUTLINEMETHOD";
      break;
    }
    case -1074395330: {
      errorText = "ERR_INVALID_BORDER_SIZE";
      break;
    }
    case -1074395331: {
      errorText = "ERR_INVALID_BORDERMETHOD";
      break;
    }
    case -1074395332: {
      errorText = "ERR_INVALID_COMPAREFUNCTION";
      break;
    }
    case -1074395333: {
      errorText = "ERR_INVALID_VERTICAL_TEXT_ALIGNMENT";
      break;
    }
    case -1074395334: {
      errorText = "ERR_INVALID_CONVERSIONSTYLE";
      break;
    }
    case -1074395335: {
      errorText = "ERR_DISPATCH_STATUS_CONFLICT";
      break;
    }
    case -1074395336: {
      errorText = "ERR_UNKNOWN_ALGORITHM";
      break;
    }
    case -1074395340: {
      errorText = "ERR_INVALID_SIZETYPE";
      break;
    }
    case -1074395343: {
      errorText = "ERR_FILE_FILENAME_nullptr";
      break;
    }
    case -1074395345: {
      errorText = "ERR_INVALID_FLIPAXIS";
      break;
    }
    case -1074395346: {
      errorText = "ERR_INVALID_INTERPOLATIONMETHOD_FOR_ROTATE";
      break;
    }
    case -1074395347: {
      errorText = "ERR_INVALID_3DDIRECTION";
      break;
    }
    case -1074395348: {
      errorText = "ERR_INVALID_3DPLANE";
      break;
    }
    case -1074395349: {
      errorText = "ERR_INVALID_SKELETONMETHOD";
      break;
    }
    case -1074395350: {
      errorText = "ERR_INVALID_VISION_INFO";
      break;
    }
    case -1074395351: {
      errorText = "ERR_INVALID_RECT";
      break;
    }
    case -1074395352: {
      errorText = "ERR_INVALID_FEATURE_MODE";
      break;
    }
    case -1074395353: {
      errorText = "ERR_INVALID_SEARCH_STRATEGY";
      break;
    }
    case -1074395354: {
      errorText = "ERR_INVALID_COLOR_WEIGHT";
      break;
    }
    case -1074395355: {
      errorText = "ERR_INVALID_NUM_MATCHES_REQUESTED";
      break;
    }
    case -1074395356: {
      errorText = "ERR_INVALID_MIN_MATCH_SCORE";
      break;
    }
    case -1074395357: {
      errorText = "ERR_INVALID_COLOR_IGNORE_MODE";
      break;
    }
    case -1074395360: {
      errorText = "ERR_COMPLEX_PLANE";
      break;
    }
    case -1074395361: {
      errorText = "ERR_INVALID_STEEPNESS";
      break;
    }
    case -1074395362: {
      errorText = "ERR_INVALID_WIDTH";
      break;
    }
    case -1074395363: {
      errorText = "ERR_INVALID_SUBSAMPLING_RATIO";
      break;
    }
    case -1074395364: {
      errorText = "ERR_IGNORE_COLOR_SPECTRUM_SET";
      break;
    }
    case -1074395365: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_NOSPECTRUM";
      break;
    }
    case -1074395366: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_NOSHAPE";
      break;
    }
    case -1074395367: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_ROTATION_5";
      break;
    }
    case -1074395368: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_ROTATION_4";
      break;
    }
    case -1074395369: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_ROTATION_3";
      break;
    }
    case -1074395370: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_ROTATION_2";
      break;
    }
    case -1074395371: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_ROTATION_1";
      break;
    }
    case -1074395372: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_NOROTATION";
      break;
    }
    case -1074395373: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_ROTATION";
      break;
    }
    case -1074395374: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_SHIFT_2";
      break;
    }
    case -1074395375: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_SHIFT_1";
      break;
    }
    case -1074395376: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_NOSHIFT";
      break;
    }
    case -1074395377: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_SHIFT";
      break;
    }
    case -1074395378: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_6";
      break;
    }
    case -1074395379: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_5";
      break;
    }
    case -1074395380: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_4";
      break;
    }
    case -1074395381: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_3";
      break;
    }
    case -1074395382: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_2";
      break;
    }
    case -1074395383: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR_1";
      break;
    }
    case -1074395384: {
      errorText = "ERR_COLOR_TEMPLATE_DESCRIPTOR";
      break;
    }
    case -1074395385: {
      errorText = "ERR_COLOR_ROTATION_REQUIRES_SHAPE_FEATURE";
      break;
    }
    case -1074395386: {
      errorText = "ERR_COLOR_MATCH_SETUP_DATA_SHAPE";
      break;
    }
    case -1074395387: {
      errorText = "ERR_COLOR_MATCH_SETUP_DATA";
      break;
    }
    case -1074395388: {
      errorText = "ERR_COLOR_LEARN_SETUP_DATA_SHAPE";
      break;
    }
    case -1074395389: {
      errorText = "ERR_COLOR_LEARN_SETUP_DATA";
      break;
    }
    case -1074395390: {
      errorText = "ERR_COLOR_TEMPLATE_IMAGE_LUMINANCE_CONTRAST_TOO_LOW";
      break;
    }
    case -1074395391: {
      errorText = "ERR_COLOR_TEMPLATE_IMAGE_HUE_CONTRAST_TOO_LOW";
      break;
    }
    case -1074395392: {
      errorText = "ERR_COLOR_TEMPLATE_IMAGE_TOO_LARGE";
      break;
    }
    case -1074395393: {
      errorText = "ERR_COLOR_TEMPLATE_IMAGE_TOO_SMALL";
      break;
    }
    case -1074395394: {
      errorText = "ERR_COLOR_SPECTRUM_MASK";
      break;
    }
    case -1074395395: {
      errorText = "ERR_COLOR_IMAGE_REQUIRED";
      break;
    }
    case -1074395397: {
      errorText = "ERR_COMPLEX_IMAGE_REQUIRED";
      break;
    }
    case -1074395399: {
      errorText = "ERR_MULTICORE_INVALID_ARGUMENT";
      break;
    }
    case -1074395400: {
      errorText = "ERR_MULTICORE_OPERATION";
      break;
    }
    case -1074395401: {
      errorText = "ERR_INVALID_MATCHFACTOR";
      break;
    }
    case -1074395402: {
      errorText = "ERR_INVALID_MAXPOINTS";
      break;
    }
    case -1074395403: {
      errorText = "ERR_EXTRAINFO_VERSION";
      break;
    }
    case -1074395404: {
      errorText = "ERR_INVALID_INTERPOLATIONMETHOD_FOR_UNWRAP";
      break;
    }
    case -1074395405: {
      errorText = "ERR_INVALID_TEXTORIENTATION";
      break;
    }
    case -1074395406: {
      errorText = "ERR_COORDSYS_NOT_FOUND";
      break;
    }
    case -1074395407: {
      errorText = "ERR_INVALID_CONTRAST";
      break;
    }
    case -1074395408: {
      errorText = "ERR_INVALID_DETECTION_MODE";
      break;
    }
    case -1074395409: {
      errorText = "ERR_INVALID_SUBPIXEL_DIVISIONS";
      break;
    }
    case -1074395410: {
      errorText = "ERR_INVALID_ICONS_PER_LINE";
      break;
    }
    case -1074395549: {
      errorText = "ERR_NIOCR_INVALID_NUMBER_OF_OBJECTS_TO_VERIFY";
      break;
    }
    case -1074395550: {
      errorText = "ERR_NIOCR_INVALID_CHARACTER_VALUE";
      break;
    }
    case -1074395551: {
      errorText = "ERR_NIOCR_RENAME_REFCHAR";
      break;
    }
    case -1074395552: {
      errorText = "ERR_NIOCR_NOT_A_VALID_CHARACTER_SET";
      break;
    }
    case -1074395553: {
      errorText = "ERR_NIOCR_INVALID_MIN_BOUNDING_RECT_HEIGHT";
      break;
    }
    case -1074395554: {
      errorText = "ERR_NIOCR_INVALID_READ_RESOLUTION";
      break;
    }
    case -1074395555: {
      errorText = "ERR_NIOCR_INVALID_SPACING_RANGE";
      break;
    }
    case -1074395556: {
      errorText = "ERR_NIOCR_INVALID_BOUNDING_RECT_HEIGHT_RANGE";
      break;
    }
    case -1074395557: {
      errorText = "ERR_NIOCR_INVALID_BOUNDING_RECT_WIDTH_RANGE";
      break;
    }
    case -1074395558: {
      errorText = "ERR_NIOCR_INVALID_CHARACTER_SIZE_RANGE";
      break;
    }
    case -1074395559: {
      errorText = "ERR_NIOCR_INVALID_READ_OPTION";
      break;
    }
    case -1074395560: {
      errorText = "ERR_NIOCR_INVALID_OBJECT_INDEX";
      break;
    }
    case -1074395561: {
      errorText = "ERR_NIOCR_INVALID_NUMBER_OF_CHARACTERS";
      break;
    }
    case -1074395562: {
      errorText = "ERR_NIOCR_BOOLEAN_VALUE_FOR_STRING_ATTRIBUTE";
      break;
    }
    case -1074395563: {
      errorText = "ERR_NIOCR_UNLICENSED";
      break;
    }
    case -1074395564: {
      errorText = "ERR_NIOCR_INVALID_PREDEFINED_CHARACTER";
      break;
    }
    case -1074395565: {
      errorText = "ERR_NIOCR_MUST_BE_SINGLE_CHARACTER";
      break;
    }
    case -1074395566: {
      errorText = "ERR_NIOCR_BOOLEAN_VALUE_FOR_INTEGER_ATTRIBUTE";
      break;
    }
    case -1074395567: {
      errorText = "ERR_NIOCR_STRING_VALUE_FOR_BOOLEAN_ATTRIBUTE";
      break;
    }
    case -1074395568: {
      errorText = "ERR_NIOCR_STRING_VALUE_FOR_INTEGER_ATTRIBUTE";
      break;
    }
    case -1074395569: {
      errorText = "ERR_NIOCR_INVALID_ATTRIBUTE";
      break;
    }
    case -1074395570: {
      errorText = "ERR_NIOCR_INTEGER_VALUE_FOR_BOOLEAN_ATTRIBUTE";
      break;
    }
    case -1074395571: {
      errorText = "ERR_NIOCR_GET_ONLY_ATTRIBUTE";
      break;
    }
    case -1074395572: {
      errorText = "ERR_NIOCR_INTEGER_VALUE_FOR_STRING_ATTRIBUTE";
      break;
    }
    case -1074395573: {
      errorText = "ERR_NIOCR_INVALID_CHARACTER_SET_FILE_VERSION";
      break;
    }
    case -1074395574: {
      errorText = "ERR_NIOCR_CHARACTER_SET_DESCRIPTION_TOO_LONG";
      break;
    }
    case -1074395575: {
      errorText = "ERR_NIOCR_INVALID_NUMBER_OF_EROSIONS";
      break;
    }
    case -1074395576: {
      errorText = "ERR_NIOCR_CHARACTER_VALUE_TOO_LONG";
      break;
    }
    case -1074395577: {
      errorText = "ERR_NIOCR_CHARACTER_VALUE_CANNOT_BE_EMPTYSTRING";
      break;
    }
    case -1074395578: {
      errorText = "ERR_NIOCR_INVALID_CHARACTER_SET_FILE";
      break;
    }
    case -1074395579: {
      errorText = "ERR_NIOCR_INVALID_ASPECT_RATIO";
      break;
    }
    case -1074395580: {
      errorText = "ERR_NIOCR_INVALID_MIN_BOUNDING_RECT_WIDTH";
      break;
    }
    case -1074395581: {
      errorText = "ERR_NIOCR_INVALID_MAX_VERT_ELEMENT_SPACING";
      break;
    }
    case -1074395582: {
      errorText = "ERR_NIOCR_INVALID_MAX_HORIZ_ELEMENT_SPACING";
      break;
    }
    case -1074395583: {
      errorText = "ERR_NIOCR_INVALID_MIN_CHAR_SPACING";
      break;
    }
    case -1074395584: {
      errorText = "ERR_NIOCR_INVALID_THRESHOLD_LIMITS";
      break;
    }
    case -1074395585: {
      errorText = "ERR_NIOCR_INVALID_UPPER_THRESHOLD_LIMIT";
      break;
    }
    case -1074395586: {
      errorText = "ERR_NIOCR_INVALID_LOWER_THRESHOLD_LIMIT";
      break;
    }
    case -1074395587: {
      errorText = "ERR_NIOCR_INVALID_THRESHOLD_RANGE";
      break;
    }
    case -1074395588: {
      errorText = "ERR_NIOCR_INVALID_HIGH_THRESHOLD_VALUE";
      break;
    }
    case -1074395589: {
      errorText = "ERR_NIOCR_INVALID_LOW_THRESHOLD_VALUE";
      break;
    }
    case -1074395590: {
      errorText = "ERR_NIOCR_INVALID_NUMBER_OF_VALID_CHARACTER_POSITIONS";
      break;
    }
    case -1074395591: {
      errorText = "ERR_NIOCR_INVALID_CHARACTER_INDEX";
      break;
    }
    case -1074395592: {
      errorText = "ERR_NIOCR_INVALID_READ_STRATEGY";
      break;
    }
    case -1074395593: {
      errorText = "ERR_NIOCR_INVALID_NUMBER_OF_BLOCKS";
      break;
    }
    case -1074395594: {
      errorText = "ERR_NIOCR_INVALID_SUBSTITUTION_CHARACTER";
      break;
    }
    case -1074395595: {
      errorText = "ERR_NIOCR_INVALID_THRESHOLD_MODE";
      break;
    }
    case -1074395596: {
      errorText = "ERR_NIOCR_INVALID_CHARACTER_SIZE";
      break;
    }
    case -1074395597: {
      errorText = "ERR_NIOCR_NOT_A_VALID_SESSION";
      break;
    }
    case -1074395598: {
      errorText = "ERR_NIOCR_INVALID_ACCEPTANCE_LEVEL";
      break;
    }
    case -1074395600: {
      errorText = "ERR_INFO_NOT_FOUND";
      break;
    }
    case -1074395601: {
      errorText = "ERR_INVALID_EDGE_THRESHOLD";
      break;
    }
    case -1074395602: {
      errorText = "ERR_INVALID_MINIMUM_CURVE_LENGTH";
      break;
    }
    case -1074395603: {
      errorText = "ERR_INVALID_ROW_STEP";
      break;
    }
    case -1074395604: {
      errorText = "ERR_INVALID_COLUMN_STEP";
      break;
    }
    case -1074395605: {
      errorText = "ERR_INVALID_MAXIMUM_END_POINT_GAP";
      break;
    }
    case -1074395606: {
      errorText = "ERR_INVALID_MINIMUM_FEATURES_TO_MATCH";
      break;
    }
    case -1074395607: {
      errorText = "ERR_INVALID_MAXIMUM_FEATURES_PER_MATCH";
      break;
    }
    case -1074395608: {
      errorText = "ERR_INVALID_SUBPIXEL_ITERATIONS";
      break;
    }
    case -1074395609: {
      errorText = "ERR_INVALID_SUBPIXEL_TOLERANCE";
      break;
    }
    case -1074395610: {
      errorText = "ERR_INVALID_INITIAL_MATCH_LIST_LENGTH";
      break;
    }
    case -1074395611: {
      errorText = "ERR_INVALID_MINIMUM_RECTANGLE_DIMENSION";
      break;
    }
    case -1074395612: {
      errorText = "ERR_INVALID_MINIMUM_FEATURE_RADIUS";
      break;
    }
    case -1074395613: {
      errorText = "ERR_INVALID_MINIMUM_FEATURE_LENGTH";
      break;
    }
    case -1074395614: {
      errorText = "ERR_INVALID_MINIMUM_FEATURE_ASPECT_RATIO";
      break;
    }
    case -1074395615: {
      errorText = "ERR_INVALID_MINIMUM_FEATURE_STRENGTH";
      break;
    }
    case -1074395616: {
      errorText = "ERR_INVALID_EDGE_FILTER_SIZE";
      break;
    }
    case -1074395617: {
      errorText = "ERR_INVALID_NUMBER_OF_FEATURES_RANGE";
      break;
    }
    case -1074395618: {
      errorText = "ERR_TOO_MANY_SCALE_RANGES";
      break;
    }
    case -1074395619: {
      errorText = "ERR_TOO_MANY_OCCLUSION_RANGES";
      break;
    }
    case -1074395620: {
      errorText = "ERR_INVALID_CURVE_EXTRACTION_MODE";
      break;
    }
    case -1074395621: {
      errorText = "ERR_INVALID_LEARN_GEOMETRIC_PATTERN_SETUP_DATA";
      break;
    }
    case -1074395622: {
      errorText = "ERR_INVALID_MATCH_GEOMETRIC_PATTERN_SETUP_DATA";
      break;
    }
    case -1074395623: {
      errorText = "ERR_INVALID_SCALE_RANGE";
      break;
    }
    case -1074395624: {
      errorText = "ERR_INVALID_OCCLUSION_RANGE";
      break;
    }
    case -1074395625: {
      errorText = "ERR_INVALID_MATCH_CONSTRAINT_TYPE";
      break;
    }
    case -1074395626: {
      errorText = "ERR_NOT_ENOUGH_TEMPLATE_FEATURES";
      break;
    }
    case -1074395627: {
      errorText = "ERR_NOT_ENOUGH_TEMPLATE_FEATURES_1";
      break;
    }
    case -1074395628: {
      errorText = "ERR_INVALID_GEOMETRIC_MATCHING_TEMPLATE";
      break;
    }
    case -1074395629: {
      errorText = "ERR_INVALID_MAXIMUM_PIXEL_DISTANCE_FROM_LINE";
      break;
    }
    case -1074395630: {
      errorText = "ERR_INVALID_MAXIMUM_FEATURES_LEARNED";
      break;
    }
    case -1074395631: {
      errorText = "ERR_INVALID_MIN_MATCH_SEPARATION_DISTANCE";
      break;
    }
    case -1074395632: {
      errorText = "ERR_INVALID_MIN_MATCH_SEPARATION_ANGLE";
      break;
    }
    case -1074395633: {
      errorText = "ERR_INVALID_MIN_MATCH_SEPARATION_SCALE";
      break;
    }
    case -1074395634: {
      errorText = "ERR_INVALID_MAX_MATCH_OVERLAP";
      break;
    }
    case -1074395635: {
      errorText = "ERR_INVALID_SHAPE_DESCRIPTOR";
      break;
    }
    case -1074395636: {
      errorText = "ERR_DIRECTX_NOT_FOUND";
      break;
    }
    case -1074395637: {
      errorText = "ERR_HARDWARE_DOESNT_SUPPORT_NONTEARING";
      break;
    }
    case -1074395638: {
      errorText = "ERR_INVALID_FILL_STYLE";
      break;
    }
    case -1074395639: {
      errorText = "ERR_INVALID_HATCH_STYLE";
      break;
    }
    case -1074395640: {
      errorText = "ERR_TOO_MANY_ZONES";
      break;
    }
    case -1074395641: {
      errorText = "ERR_DUPLICATE_LABEL";
      break;
    }
    case -1074395642: {
      errorText = "ERR_LABEL_NOT_FOUND";
      break;
    }
    case -1074395643: {
      errorText = "ERR_INVALID_NUMBER_OF_MATCH_OPTIONS";
      break;
    }
    case -1074395644: {
      errorText = "ERR_LABEL_TOO_LONG";
      break;
    }
    case -1074395645: {
      errorText = "ERR_INVALID_NUMBER_OF_LABELS";
      break;
    }
    case -1074395646: {
      errorText = "ERR_NO_TEMPLATE_TO_LEARN";
      break;
    }
    case -1074395647: {
      errorText = "ERR_INVALID_MULTIPLE_GEOMETRIC_TEMPLATE";
      break;
    }
    case -1074395648: {
      errorText = "ERR_TEMPLATE_NOT_LEARNED";
      break;
    }
    case -1074395649: {
      errorText = "ERR_INVALID_GEOMETRIC_FEATURE_TYPE";
      break;
    }
    case -1074395650: {
      errorText = "ERR_CURVE_EXTRACTION_MODE_MUST_BE_SAME";
      break;
    }
    case -1074395651: {
      errorText = "ERR_EDGE_FILTER_SIZE_MUST_BE_SAME";
      break;
    }
    case -1074395652: {
      errorText = "ERR_OPENING_NEWER_GEOMETRIC_MATCHING_TEMPLATE";
      break;
    }
    case -1074395653: {
      errorText = "ERR_OPENING_NEWER_MULTIPLE_GEOMETRIC_TEMPLATE";
      break;
    }
    case -1074395654: {
      errorText = "ERR_GRADING_INFORMATION_NOT_FOUND";
      break;
    }
    case -1074395655: {
      errorText = "ERR_ENABLE_CALIBRATION_SUPPORT_MUST_BE_SAME";
      break;
    }
    case -1074395656: {
      errorText = "ERR_SMOOTH_CONTOURS_MUST_BE_SAME";
      break;
    }
    case -1074395700: {
      errorText = "ERR_REQUIRES_WIN2000_OR_NEWER";
      break;
    }
    case -1074395701: {
      errorText = "ERR_INVALID_MATRIX_SIZE_RANGE";
      break;
    }
    case -1074395702: {
      errorText = "ERR_INVALID_LENGTH";
      break;
    }
    case -1074395703: {
      errorText = "ERR_INVALID_TYPE_OF_FLATTEN";
      break;
    }
    case -1074395704: {
      errorText = "ERR_INVALID_COMPRESSION_TYPE";
      break;
    }
    case -1074395705: {
      errorText = "ERR_DATA_CORRUPTED";
      break;
    }
    case -1074395706: {
      errorText = "ERR_AVI_SESSION_ALREADY_OPEN";
      break;
    }
    case -1074395707: {
      errorText = "ERR_AVI_WRITE_SESSION_REQUIRED";
      break;
    }
    case -1074395708: {
      errorText = "ERR_AVI_READ_SESSION_REQUIRED";
      break;
    }
    case -1074395709: {
      errorText = "ERR_AVI_UNOPENED_SESSION";
      break;
    }
    case -1074395710: {
      errorText = "ERR_TOO_MANY_PARTICLES";
      break;
    }
    case -1074395711: {
      errorText = "ERR_NOT_ENOUGH_REGIONS";
      break;
    }
    case -1074395712: {
      errorText = "ERR_WRONG_REGION_TYPE";
      break;
    }
    case -1074395713: {
      errorText = "ERR_VALUE_NOT_IN_ENUM";
      break;
    }
    case -1074395714: {
      errorText = "ERR_INVALID_AXIS_ORIENTATION";
      break;
    }
    case -1074395715: {
      errorText = "ERR_INVALID_CALIBRATION_UNIT";
      break;
    }
    case -1074395716: {
      errorText = "ERR_INVALID_SCALING_METHOD";
      break;
    }
    case -1074395717: {
      errorText = "ERR_INVALID_RANGE";
      break;
    }
    case -1074395718: {
      errorText = "ERR_LAB_VERSION";
      break;
    }
    case -1074395719: {
      errorText = "ERR_BAD_ROI_BOX";
      break;
    }
    case -1074395720: {
      errorText = "ERR_BAD_ROI";
      break;
    }
    case -1074395721: {
      errorText = "ERR_INVALID_BIT_DEPTH";
      break;
    }
    case -1074395722: {
      errorText = "ERR_CLASSIFIER_CLASSIFY_IMAGE_WITH_CUSTOM_SESSION";
      break;
    }
    case -1074395723: {
      errorText = "ERR_CUSTOMDATA_KEY_NOT_FOUND";
      break;
    }
    case -1074395724: {
      errorText = "ERR_CUSTOMDATA_INVALID_SIZE";
      break;
    }
    case -1074395725: {
      errorText = "ERR_DATA_VERSION";
      break;
    }
    case -1074395726: {
      errorText = "ERR_MATCHFACTOR_OBSOLETE";
      break;
    }
    case -1074395727: {
      errorText = "ERR_UNSUPPORTED_2D_BARCODE_SEARCH_MODE";
      break;
    }
    case -1074395728: {
      errorText = "ERR_INVALID_2D_BARCODE_SEARCH_MODE";
      break;
    }
    case -1074395754: {
      errorText = "ERR_TRIG_TIMEOUT";
      break;
    }
    case -1074395756: {
      errorText = "ERR_DLL_FUNCTION_NOT_FOUND";
      break;
    }
    case -1074395757: {
      errorText = "ERR_DLL_NOT_FOUND";
      break;
    }
    case -1074395758: {
      errorText = "ERR_BOARD_NOT_OPEN";
      break;
    }
    case -1074395760: {
      errorText = "ERR_BOARD_NOT_FOUND";
      break;
    }
    case -1074395762: {
      errorText = "ERR_INVALID_NIBLACK_DEVIATION_FACTOR";
      break;
    }
    case -1074395763: {
      errorText = "ERR_INVALID_NORMALIZATION_METHOD";
      break;
    }
    case -1074395766: {
      errorText = "ERR_DEPRECATED_FUNCTION";
      break;
    }
    case -1074395767: {
      errorText = "ERR_INVALID_ALIGNMENT";
      break;
    }
    case -1074395768: {
      errorText = "ERR_INVALID_SCALE";
      break;
    }
    case -1074395769: {
      errorText = "ERR_INVALID_EDGE_THICKNESS";
      break;
    }
    case -1074395770: {
      errorText = "ERR_INVALID_INSPECTION_TEMPLATE";
      break;
    }
    case -1074395771: {
      errorText = "ERR_OPENING_NEWER_INSPECTION_TEMPLATE";
      break;
    }
    case -1074395772: {
      errorText = "ERR_INVALID_REGISTRATION_METHOD";
      break;
    }
    case -1074395773: {
      errorText = "ERR_NO_DEST_IMAGE";
      break;
    }
    case -1074395774: {
      errorText = "ERR_NO_LABEL";
      break;
    }
    case -1074395775: {
      errorText = "ERR_ROI_HAS_OPEN_CONTOURS";
      break;
    }
    case -1074395776: {
      errorText = "ERR_INVALID_USE_OF_COMPACT_SESSION_FILE";
      break;
    }
    case -1074395777: {
      errorText = "ERR_INCOMPATIBLE_CLASSIFIER_TYPES";
      break;
    }
    case -1074395778: {
      errorText = "ERR_INVALID_KERNEL_SIZE";
      break;
    }
    case -1074395779: {
      errorText = "ERR_CANNOT_COMPACT_UNTRAINED";
      break;
    }
    case -1074395780: {
      errorText = "ERR_INVALID_PARTICLE_TYPE";
      break;
    }
    case -1074395781: {
      errorText = "ERR_CLASSIFIER_INVALID_ENGINE_TYPE";
      break;
    }
    case -1074395782: {
      errorText = "ERR_DESCRIPTION_TOO_LONG";
      break;
    }
    case -1074395783: {
      errorText = "ERR_BAD_SAMPLE_INDEX";
      break;
    }
    case -1074395784: {
      errorText = "ERR_INVALID_LIMITS";
      break;
    }
    case -1074395785: {
      errorText = "ERR_NO_PARTICLE";
      break;
    }
    case -1074395786: {
      errorText = "ERR_INVALID_PARTICLE_OPTIONS";
      break;
    }
    case -1074395787: {
      errorText = "ERR_INVALID_CLASSIFIER_TYPE";
      break;
    }
    case -1074395788: {
      errorText = "ERR_NO_SAMPLES";
      break;
    }
    case -1074395789: {
      errorText = "ERR_OPENING_NEWER_CLASSIFIER_SESSION";
      break;
    }
    case -1074395790: {
      errorText = "ERR_INVALID_DISTANCE_METRIC";
      break;
    }
    case -1074395791: {
      errorText = "ERR_CLASSIFIER_INVALID_SESSION_TYPE";
      break;
    }
    case -1074395792: {
      errorText = "ERR_CLASSIFIER_SESSION_NOT_TRAINED";
      break;
    }
    case -1074395793: {
      errorText = "ERR_INVALID_OPERATION_ON_COMPACT_SESSION_ATTEMPTED";
      break;
    }
    case -1074395794: {
      errorText = "ERR_K_TOO_HIGH";
      break;
    }
    case -1074395795: {
      errorText = "ERR_K_TOO_LOW";
      break;
    }
    case -1074395796: {
      errorText = "ERR_INVALID_KNN_METHOD";
      break;
    }
    case -1074395797: {
      errorText = "ERR_INVALID_CLASSIFIER_SESSION";
      break;
    }
    case -1074395798: {
      errorText = "ERR_INVALID_CUSTOM_SAMPLE";
      break;
    }
    case -1074395799: {
      errorText = "ERR_INTERNAL";
      break;
    }
    case -1074395800: {
      errorText = "ERR_PROTECTION";
      break;
    }
    case -1074395801: {
      errorText = "ERR_TOO_MANY_CONTOURS";
      break;
    }
    case -1074395837: {
      errorText = "ERR_INVALID_COMPRESSION_RATIO";
      break;
    }
    case -1074395840: {
      errorText = "ERR_BAD_INDEX";
      break;
    }
    case -1074395875: {
      errorText = "ERR_BARCODE_PHARMACODE";
      break;
    }
    case -1074395876: {
      errorText = "ERR_UNSUPPORTED_COLOR_MODE";
      break;
    }
    case -1074395877: {
      errorText = "ERR_COLORMODE_REQUIRES_CHANGECOLORSPACE2";
      break;
    }
    case -1074395878: {
      errorText = "ERR_PROP_NODE_WRITE_NOT_SUPPORTED";
      break;
    }
    case -1074395879: {
      errorText = "ERR_BAD_MEASURE";
      break;
    }
    case -1074395880: {
      errorText = "ERR_PARTICLE";
      break;
    }
    case -1074395920: {
      errorText = "ERR_NUMBER_CLASS";
      break;
    }
    case -1074395953: {
      errorText = "ERR_INVALID_WAVELET_TRANSFORM_MODE";
      break;
    }
    case -1074395954: {
      errorText = "ERR_INVALID_QUANTIZATION_STEP_SIZE";
      break;
    }
    case -1074395955: {
      errorText = "ERR_INVALID_MAX_WAVELET_TRANSFORM_LEVEL";
      break;
    }
    case -1074395956: {
      errorText = "ERR_INVALID_QUALITY";
      break;
    }
    case -1074395957: {
      errorText = "ERR_ARRAY_SIZE_MISMATCH";
      break;
    }
    case -1074395958: {
      errorText = "ERR_WINDOW_ID";
      break;
    }
    case -1074395959: {
      errorText = "ERR_CREATE_WINDOW";
      break;
    }
    case -1074395960: {
      errorText = "ERR_INIT";
      break;
    }
    case -1074395971: {
      errorText = "ERR_INVALID_OFFSET";
      break;
    }
    case -1074395972: {
      errorText = "ERR_DIRECTX_ENUMERATE_FILTERS";
      break;
    }
    case -1074395973: {
      errorText = "ERR_JPEG2000_UNSUPPORTED_MULTIPLE_LAYERS";
      break;
    }
    case -1074395974: {
      errorText = "ERR_UNSUPPORTED_JPEG2000_COLORSPACE_METHOD";
      break;
    }
    case -1074395975: {
      errorText = "ERR_AVI_TIMEOUT";
      break;
    }
    case -1074395976: {
      errorText = "ERR_NUMBER_OF_PALETTE_COLORS";
      break;
    }
    case -1074395977: {
      errorText = "ERR_AVI_VERSION";
      break;
    }
    case -1074395978: {
      errorText = "ERR_INVALID_PARTICLE_NUMBER";
      break;
    }
    case -1074395979: {
      errorText = "ERR_INVALID_PARTICLE_INFO";
      break;
    }
    case -1074395980: {
      errorText = "ERR_COM_INITIALIZE";
      break;
    }
    case -1074395981: {
      errorText = "ERR_INSUFFICIENT_BUFFER_SIZE";
      break;
    }
    case -1074395982: {
      errorText = "ERR_INVALID_FRAMES_PER_SECOND";
      break;
    }
    case -1074395983: {
      errorText = "ERR_FILE_NO_SPACE";
      break;
    }
    case -1074395984: {
      errorText = "ERR_FILE_INVALID_DATA_TYPE";
      break;
    }
    case -1074395985: {
      errorText = "ERR_FILE_OPERATION";
      break;
    }
    case -1074395986: {
      errorText = "ERR_FILE_FORMAT";
      break;
    }
    case -1074395987: {
      errorText = "ERR_FILE_EOF";
      break;
    }
    case -1074395988: {
      errorText = "ERR_FILE_WRITE";
      break;
    }
    case -1074395989: {
      errorText = "ERR_FILE_READ";
      break;
    }
    case -1074395990: {
      errorText = "ERR_FILE_GET_INFO";
      break;
    }
    case -1074395991: {
      errorText = "ERR_FILE_INVALID_TYPE";
      break;
    }
    case -1074395992: {
      errorText = "ERR_FILE_PERMISSION";
      break;
    }
    case -1074395993: {
      errorText = "ERR_FILE_IO_ERR";
      break;
    }
    case -1074395994: {
      errorText = "ERR_FILE_TOO_MANY_OPEN";
      break;
    }
    case -1074395995: {
      errorText = "ERR_FILE_NOT_FOUND";
      break;
    }
    case -1074395996: {
      errorText = "ERR_FILE_OPEN";
      break;
    }
    case -1074395997: {
      errorText = "ERR_FILE_ARGERR";
      break;
    }
    case -1074395998: {
      errorText = "ERR_FILE_COLOR_TABLE";
      break;
    }
    case -1074395999: {
      errorText = "ERR_FILE_FILE_TYPE";
      break;
    }
    case -1074396000: {
      errorText = "ERR_FILE_FILE_HEADER";
      break;
    }
    case -1074396001: {
      errorText = "ERR_TOO_MANY_AVI_SESSIONS";
      break;
    }
    case -1074396002: {
      errorText = "ERR_INVALID_LINEGAUGEMETHOD";
      break;
    }
    case -1074396003: {
      errorText = "ERR_AVI_DATA_EXCEEDS_BUFFER_SIZE";
      break;
    }
    case -1074396004: {
      errorText = "ERR_DIRECTX_CERTIFICATION_FAILURE";
      break;
    }
    case -1074396005: {
      errorText = "ERR_INVALID_AVI_SESSION";
      break;
    }
    case -1074396006: {
      errorText = "ERR_DIRECTX_UNKNOWN_COMPRESSION_FILTER";
      break;
    }
    case -1074396007: {
      errorText = "ERR_DIRECTX_INCOMPATIBLE_COMPRESSION_FILTER";
      break;
    }
    case -1074396008: {
      errorText = "ERR_DIRECTX_NO_FILTER";
      break;
    }
    case -1074396009: {
      errorText = "ERR_DIRECTX";
      break;
    }
    case -1074396010: {
      errorText = "ERR_INVALID_FRAME_NUMBER";
      break;
    }
    case -1074396011: {
      errorText = "ERR_RPC_BIND";
      break;
    }
    case -1074396012: {
      errorText = "ERR_RPC_EXECUTE";
      break;
    }
    case -1074396013: {
      errorText = "ERR_INVALID_VIDEO_MODE";
      break;
    }
    case -1074396014: {
      errorText = "ERR_INVALID_VIDEO_BLIT";
      break;
    }
    case -1074396015: {
      errorText = "ERR_RPC_EXECUTE_IVB";
      break;
    }
    case -1074396016: {
      errorText = "ERR_NO_VIDEO_DRIVER";
      break;
    }
    case -1074396017: {
      errorText = "ERR_OPENING_NEWER_AIM_GRADING_DATA";
      break;
    }
    case -1074396018: {
      errorText = "ERR_INVALID_EDGE_POLARITY_SEARCH_MODE";
      break;
    }
    case -1074396019: {
      errorText = "ERR_INVALID_THRESHOLD_PERCENTAGE";
      break;
    }
    case -1074396020: {
      errorText = "ERR_INVALID_GRADING_MODE";
      break;
    }
    case -1074396021: {
      errorText = "ERR_INVALID_KERNEL_SIZE_FOR_EDGE_DETECTION";
      break;
    }
    case -1074396022: {
      errorText = "ERR_INVALID_SEARCH_MODE_FOR_STRAIGHT_EDGE";
      break;
    }
    case -1074396023: {
      errorText = "ERR_INVALID_ANGLE_TOL_FOR_STRAIGHT_EDGE";
      break;
    }
    case -1074396024: {
      errorText = "ERR_INVALID_MIN_COVERAGE_FOR_STRAIGHT_EDGE";
      break;
    }
    case -1074396025: {
      errorText = "ERR_INVALID_ANGLE_RANGE_FOR_STRAIGHT_EDGE";
      break;
    }
    case -1074396026: {
      errorText = "ERR_INVALID_PROCESS_TYPE_FOR_EDGE_DETECTION";
      break;
    }
    case -1074396032: {
      errorText = "ERR_TEMPLATEDESCRIPTOR_ROTATION_SEARCHSTRATEGY";
      break;
    }
    case -1074396033: {
      errorText = "ERR_TEMPLATEDESCRIPTOR_LEARNSETUPDATA";
      break;
    }
    case -1074396034: {
      errorText = "ERR_TEMPLATEIMAGE_EDGEINFO";
      break;
    }
    case -1074396035: {
      errorText = "ERR_TEMPLATEIMAGE_NOCIRCLE";
      break;
    }
    case -1074396036: {
      errorText = "ERR_INVALID_SKELETONMODE";
      break;
    }
    case -1074396037: {
      errorText = "ERR_TIMEOUT";
      break;
    }
    case -1074396038: {
      errorText = "ERR_FIND_COORDSYS_MORE_THAN_ONE_EDGE";
      break;
    }
    case -1074396039: {
      errorText = "ERR_IO_ERROR";
      break;
    }
    case -1074396040: {
      errorText = "ERR_DRIVER";
      break;
    }
    case -1074396041: {
      errorText = "ERR_INVALID_2D_BARCODE_TYPE";
      break;
    }
    case -1074396042: {
      errorText = "ERR_INVALID_2D_BARCODE_CONTRAST";
      break;
    }
    case -1074396043: {
      errorText = "ERR_INVALID_2D_BARCODE_CELL_SHAPE";
      break;
    }
    case -1074396044: {
      errorText = "ERR_INVALID_2D_BARCODE_SHAPE";
      break;
    }
    case -1074396045: {
      errorText = "ERR_INVALID_2D_BARCODE_SUBTYPE";
      break;
    }
    case -1074396046: {
      errorText = "ERR_INVALID_2D_BARCODE_CONTRAST_FOR_ROI";
      break;
    }
    case -1074396047: {
      errorText = "ERR_INVALID_LINEAR_AVERAGE_MODE";
      break;
    }
    case -1074396048: {
      errorText = "ERR_INVALID_CELL_SAMPLE_SIZE";
      break;
    }
    case -1074396049: {
      errorText = "ERR_INVALID_MATRIX_POLARITY";
      break;
    }
    case -1074396050: {
      errorText = "ERR_INVALID_ECC_TYPE";
      break;
    }
    case -1074396051: {
      errorText = "ERR_INVALID_CELL_FILTER_MODE";
      break;
    }
    case -1074396052: {
      errorText = "ERR_INVALID_DEMODULATION_MODE";
      break;
    }
    case -1074396053: {
      errorText = "ERR_INVALID_BORDER_INTEGRITY";
      break;
    }
    case -1074396054: {
      errorText = "ERR_INVALID_CELL_FILL_TYPE";
      break;
    }
    case -1074396055: {
      errorText = "ERR_INVALID_ASPECT_RATIO";
      break;
    }
    case -1074396056: {
      errorText = "ERR_INVALID_MATRIX_MIRROR_MODE";
      break;
    }
    case -1074396057: {
      errorText = "ERR_INVALID_SEARCH_VECTOR_WIDTH";
      break;
    }
    case -1074396058: {
      errorText = "ERR_INVALID_ROTATION_MODE";
      break;
    }
    case -1074396059: {
      errorText = "ERR_INVALID_MAX_ITERATIONS";
      break;
    }
    case -1074396060: {
      errorText = "ERR_JPEG2000_LOSSLESS_WITH_FLOATING_POINT";
      break;
    }
    case -1074396061: {
      errorText = "ERR_INVALID_WINDOW_SIZE";
      break;
    }
    case -1074396062: {
      errorText = "ERR_INVALID_TOLERANCE";
      break;
    }
    case -1074396063: {
      errorText = "ERR_EXTERNAL_ALIGNMENT";
      break;
    }
    case -1074396064: {
      errorText = "ERR_EXTERNAL_NOT_SUPPORTED";
      break;
    }
    case -1074396065: {
      errorText = "ERR_CANT_RESIZE_EXTERNAL";
      break;
    }
    case -1074396066: {
      errorText = "ERR_INVALID_POINTSYMBOL";
      break;
    }
    case -1074396067: {
      errorText = "ERR_IMAGES_NOT_DIFF";
      break;
    }
    case -1074396068: {
      errorText = "ERR_INVALID_ACTION";
      break;
    }
    case -1074396069: {
      errorText = "ERR_INVALID_COLOR_MODE";
      break;
    }
    case -1074396070: {
      errorText = "ERR_INVALID_FUNCTION";
      break;
    }
    case -1074396071: {
      errorText = "ERR_INVALID_SCAN_DIRECTION";
      break;
    }
    case -1074396072: {
      errorText = "ERR_INVALID_BORDER";
      break;
    }
    case -1074396073: {
      errorText = "ERR_MASK_OUTSIDE_IMAGE";
      break;
    }
    case -1074396074: {
      errorText = "ERR_INCOMP_SIZE";
      break;
    }
    case -1074396075: {
      errorText = "ERR_COORD_SYS_SECOND_AXIS";
      break;
    }
    case -1074396076: {
      errorText = "ERR_COORD_SYS_FIRST_AXIS";
      break;
    }
    case -1074396077: {
      errorText = "ERR_INCOMP_TYPE";
      break;
    }
    case -1074396079: {
      errorText = "ERR_INVALID_METAFILE_HANDLE";
      break;
    }
    case -1074396080: {
      errorText = "ERR_INVALID_IMAGE_TYPE";
      break;
    }
    case -1074396081: {
      errorText = "ERR_BAD_PASSWORD";
      break;
    }
    case -1074396082: {
      errorText = "ERR_PALETTE_NOT_SUPPORTED";
      break;
    }
    case -1074396083: {
      errorText = "ERR_ROLLBACK_TIMEOUT";
      break;
    }
    case -1074396084: {
      errorText = "ERR_ROLLBACK_DELETE_TIMER";
      break;
    }
    case -1074396085: {
      errorText = "ERR_ROLLBACK_INIT_TIMER";
      break;
    }
    case -1074396086: {
      errorText = "ERR_ROLLBACK_START_TIMER";
      break;
    }
    case -1074396087: {
      errorText = "ERR_ROLLBACK_STOP_TIMER";
      break;
    }
    case -1074396088: {
      errorText = "ERR_ROLLBACK_RESIZE";
      break;
    }
    case -1074396089: {
      errorText = "ERR_ROLLBACK_RESOURCE_REINITIALIZE";
      break;
    }
    case -1074396090: {
      errorText = "ERR_ROLLBACK_RESOURCE_ENABLED";
      break;
    }
    case -1074396091: {
      errorText = "ERR_ROLLBACK_RESOURCE_UNINITIALIZED_ENABLE";
      break;
    }
    case -1074396092: {
      errorText = "ERR_ROLLBACK_RESOURCE_NON_EMPTY_INITIALIZE";
      break;
    }
    case -1074396093: {
      errorText = "ERR_ROLLBACK_RESOURCE_LOCKED";
      break;
    }
    case -1074396094: {
      errorText = "ERR_ROLLBACK_RESOURCE_CANNOT_UNLOCK";
      break;
    }
    case -1074396095: {
      errorText = "ERR_CALIBRATION_DUPLICATE_REFERENCE_POINT";
      break;
    }
    case -1074396096: {
      errorText = "ERR_NOT_AN_OBJECT";
      break;
    }
    case -1074396097: {
      errorText = "ERR_INVALID_PARTICLE_PARAMETER_VALUE";
      break;
    }
    case -1074396098: {
      errorText = "ERR_RESERVED_MUST_BE_nullptr";
      break;
    }
    case -1074396099: {
      errorText = "ERR_CALIBRATION_INFO_SIMPLE_TRANSFORM";
      break;
    }
    case -1074396100: {
      errorText = "ERR_CALIBRATION_INFO_PERSPECTIVE_PROJECTION";
      break;
    }
    case -1074396101: {
      errorText = "ERR_CALIBRATION_INFO_MICRO_PLANE";
      break;
    }
    case -1074396102: {
      errorText = "ERR_CALIBRATION_INFO_6";
      break;
    }
    case -1074396103: {
      errorText = "ERR_CALIBRATION_INFO_5";
      break;
    }
    case -1074396104: {
      errorText = "ERR_CALIBRATION_INFO_4";
      break;
    }
    case -1074396105: {
      errorText = "ERR_CALIBRATION_INFO_3";
      break;
    }
    case -1074396106: {
      errorText = "ERR_CALIBRATION_INFO_2";
      break;
    }
    case -1074396107: {
      errorText = "ERR_CALIBRATION_INFO_1";
      break;
    }
    case -1074396108: {
      errorText = "ERR_CALIBRATION_ERRORMAP";
      break;
    }
    case -1074396109: {
      errorText = "ERR_CALIBRATION_INVALID_SCALING_FACTOR";
      break;
    }
    case -1074396110: {
      errorText = "ERR_CALIBRATION_INFO_VERSION";
      break;
    }
    case -1074396111: {
      errorText = "ERR_CALIBRATION_FAILED_TO_FIND_GRID";
      break;
    }
    case -1074396112: {
      errorText = "ERR_INCOMP_MATRIX_SIZE";
      break;
    }
    case -1074396113: {
      errorText = "ERR_CALIBRATION_IMAGE_UNCALIBRATED";
      break;
    }
    case -1074396114: {
      errorText = "ERR_CALIBRATION_INVALID_ROI";
      break;
    }
    case -1074396115: {
      errorText = "ERR_CALIBRATION_IMAGE_CORRECTED";
      break;
    }
    case -1074396116: {
      errorText = "ERR_CALIBRATION_INSF_POINTS";
      break;
    }
    case -1074396117: {
      errorText = "ERR_MATRIX_SIZE";
      break;
    }
    case -1074396118: {
      errorText = "ERR_INVALID_STEP_SIZE";
      break;
    }
    case -1074396119: {
      errorText = "ERR_CUSTOMDATA_INVALID_KEY";
      break;
    }
    case -1074396120: {
      errorText = "ERR_NOT_IMAGE";
      break;
    }
    case -1074396121: {
      errorText = "ERR_SATURATION_THRESHOLD_OUT_OF_RANGE";
      break;
    }
    case -1074396122: {
      errorText = "ERR_DRAWTEXT_COLOR_MUST_BE_GRAYSCALE";
      break;
    }
    case -1074396123: {
      errorText = "ERR_INVALID_CALIBRATION_MODE";
      break;
    }
    case -1074396124: {
      errorText = "ERR_INVALID_CALIBRATION_ROI_MODE";
      break;
    }
    case -1074396125: {
      errorText = "ERR_INVALID_CONTRAST_THRESHOLD";
      break;
    }
    case -1074396126: {
      errorText = "ERR_ROLLBACK_RESOURCE_CONFLICT_1";
      break;
    }
    case -1074396127: {
      errorText = "ERR_ROLLBACK_RESOURCE_CONFLICT_2";
      break;
    }
    case -1074396128: {
      errorText = "ERR_ROLLBACK_RESOURCE_CONFLICT_3";
      break;
    }
    case -1074396129: {
      errorText = "ERR_ROLLBACK_UNBOUNDED_INTERFACE";
      break;
    }
    case -1074396130: {
      errorText = "ERR_NOT_RECT_OR_ROTATED_RECT";
      break;
    }
    case -1074396132: {
      errorText = "ERR_MASK_NOT_TEMPLATE_SIZE";
      break;
    }
    case -1074396133: {
      errorText = "ERR_THREAD_COULD_NOT_INITIALIZE";
      break;
    }
    case -1074396134: {
      errorText = "ERR_THREAD_INITIALIZING";
      break;
    }
    case -1074396135: {
      errorText = "ERR_INVALID_BUTTON_LABEL";
      break;
    }
    case -1074396136: {
      errorText = "ERR_DIRECTX_INVALID_FILTER_QUALITY";
      break;
    }
    case -1074396137: {
      errorText = "ERR_DIRECTX_DLL_NOT_FOUND";
      break;
    }
    case -1074396138: {
      errorText = "ERR_ROLLBACK_NOT_SUPPORTED";
      break;
    }
    case -1074396139: {
      errorText = "ERR_ROLLBACK_RESOURCE_OUT_OF_MEMORY";
      break;
    }
    case -1074396140: {
      errorText = "ERR_BARCODE_CODE128_SET";
      break;
    }
    case -1074396141: {
      errorText = "ERR_BARCODE_CODE128_FNC";
      break;
    }
    case -1074396142: {
      errorText = "ERR_BARCODE_INVALID";
      break;
    }
    case -1074396143: {
      errorText = "ERR_BARCODE_TYPE";
      break;
    }
    case -1074396144: {
      errorText = "ERR_BARCODE_CODE93_SHIFT";
      break;
    }
    case -1074396145: {
      errorText = "ERR_BARCODE_UPCA";
      break;
    }
    case -1074396146: {
      errorText = "ERR_BARCODE_MSI";
      break;
    }
    case -1074396147: {
      errorText = "ERR_BARCODE_I25";
      break;
    }
    case -1074396148: {
      errorText = "ERR_BARCODE_EAN13";
      break;
    }
    case -1074396149: {
      errorText = "ERR_BARCODE_EAN8";
      break;
    }
    case -1074396150: {
      errorText = "ERR_BARCODE_CODE128";
      break;
    }
    case -1074396151: {
      errorText = "ERR_BARCODE_CODE93";
      break;
    }
    case -1074396152: {
      errorText = "ERR_BARCODE_CODE39";
      break;
    }
    case -1074396153: {
      errorText = "ERR_BARCODE_CODABAR";
      break;
    }
    case -1074396154: {
      errorText = "ERR_IMAGE_TOO_SMALL";
      break;
    }
    case -1074396155: {
      errorText = "ERR_UNINIT";
      break;
    }
    case -1074396156: {
      errorText = "ERR_NEED_FULL_VERSION";
      break;
    }
    case -1074396157: {
      errorText = "ERR_UNREGISTERED";
      break;
    }
    case -1074396158: {
      errorText = "ERR_MEMORY_ERROR";
      break;
    }
    case -1074396159: {
      errorText = "ERR_OUT_OF_MEMORY";
      break;
    }
    case -1074396160: {
      errorText = "ERR_SYSTEM_ERROR";
      break;
    }
    case 0: {
      errorText = "ERR_SUCCESS";
      break;
    }
    // end National Instruments defined errors

    // begin BAE defined errors
    case ERR_VISION_GENERAL_ERROR: {
      errorText = "ERR_VISION_GENERAL_ERROR";
      break;
    }
    case ERR_COLOR_NOT_FOUND: {
      errorText = "ERR_COLOR_NOT_FOUND";
      break;
    }
    case ERR_PARTICLE_TOO_SMALL: {
      errorText = "ERR_PARTICLE_TOO_SMALL";
      break;
    }
    case ERR_CAMERA_FAILURE: {
      errorText = "ERR_CAMERA_FAILURE";
      break;
    }
    case ERR_CAMERA_SOCKET_CREATE_FAILED: {
      errorText = "ERR_CAMERA_SOCKET_CREATE_FAILED";
      break;
    }
    case ERR_CAMERA_CONNECT_FAILED: {
      errorText = "ERR_CAMERA_CONNECT_FAILED";
      break;
    }
    case ERR_CAMERA_STALE_IMAGE: {
      errorText = "ERR_CAMERA_STALE_IMAGE";
      break;
    }
    case ERR_CAMERA_NOT_INITIALIZED: {
      errorText = "ERR_CAMERA_NOT_INITIALIZED";
      break;
    }
    case ERR_CAMERA_NO_BUFFER_AVAILABLE: {
      errorText = "ERR_CAMERA_NO_BUFFER_AVAILABLE";
      break;
    }
    case ERR_CAMERA_HEADER_ERROR: {
      errorText = "ERR_CAMERA_HEADER_ERROR";
      break;
    }
    case ERR_CAMERA_BLOCKING_TIMEOUT: {
      errorText = "ERR_CAMERA_BLOCKING_TIMEOUT";
      break;
    }
    case ERR_CAMERA_AUTHORIZATION_FAILED: {
      errorText = "ERR_CAMERA_AUTHORIZATION_FAILED";
      break;
    }
    case ERR_CAMERA_TASK_SPAWN_FAILED: {
      errorText = "ERR_CAMERA_TASK_SPAWN_FAILED";
      break;
    }
    case ERR_CAMERA_TASK_INPUT_OUT_OF_RANGE: {
      errorText = "ERR_CAMERA_TASK_INPUT_OUT_OF_RANGE";
      break;
    }
    case ERR_CAMERA_COMMAND_FAILURE: {
      errorText = "ERR_CAMERA_COMMAND_FAILURE";
      break;
    }
  }

  return errorText;
}
