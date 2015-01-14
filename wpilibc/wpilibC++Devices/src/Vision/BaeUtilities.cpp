/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> 
#include <stdarg.h> 
 
#include "Vision/BaeUtilities.h"
#include "Servo.h"
#include "Timer.h"

/** @file
 *   Utility functions 
 */

/**
 * debug output flag options:
 * DEBUG_OFF, DEBUG_MOSTLY_OFF, DEBUG_SCREEN_ONLY, DEBUG_FILE_ONLY, DEBUG_SCREEN_AND_FILE
 */
static DebugOutputType	dprintfFlag = DEBUG_OFF;  

/**
 * Set the debug flag to print to screen, file on cRIO, both or neither
 * @param tempString The format string.
 */
void SetDebugFlag ( DebugOutputType flag  )  
{ dprintfFlag = flag; }

/**
 * Debug print to a file and/or a terminal window.
 * Call like you would call printf.
 * Set functionName in the function if you want the correct function name to print out.
 * The file line number will also be printed.
 * @param tempString The format string.
 */
void dprintf ( const char * tempString, ...  )  /* Variable argument list */
{
  va_list		args;			  /* Input argument list */
  int			line_number;      /* Line number passed in argument */
  int			type;
  const char	*functionName;    /* Format passed in argument */
  const char	*fmt;             /* Format passed in argument */
  char			text[512];   	  /* Text string */
  char			outtext[512];     /* Text string */
  FILE			*outfile_fd;      /* Output file pointer */
  char			filepath[128];    /* Text string */
  int			fatalFlag=0;
  const char	*filename;
  int			index;
  int			tempStringLen;

  if (dprintfFlag == DEBUG_OFF) { return; }
  
  va_start (args, tempString);
  
  tempStringLen = strlen(tempString);
  filename = tempString;
  for (index=0;index<tempStringLen;index++){
	  if (tempString[index] == ' ') {
		  printf( "ERROR in dprintf: malformed calling sequence (%s)\n",tempString);return;
	  	}
	  if (tempString[index] == '\\' || tempString[index] == '/')
		  filename = tempString + index + 1;
  }
  
  /* Extract function name */
  functionName = va_arg (args, const char *);
 
 /* Extract line number from argument list */
  line_number = va_arg (args, int);

 /* Extract information type from argument list */
  type = va_arg (args, int);

 /* Extract format from argument list */
  fmt = va_arg (args, const char *);

  vsprintf (text, fmt, args);

  va_end (args);

  /* Format output statement */
  switch (type)
   {
   case DEBUG_TYPE:
     sprintf (outtext, "[%s:%s@%04d] DEBUG %s\n",
              filename, functionName, line_number, text);
     break;
   case INFO_TYPE:
     sprintf (outtext, "[%s:%s@%04d] INFO %s\n",
              filename, functionName, line_number, text);
     break;
   case ERROR_TYPE:
     sprintf (outtext, "[%s:%s@%04d] ERROR %s\n",
              filename, functionName, line_number, text);
     break;
   case CRITICAL_TYPE:
     sprintf (outtext, "[%s:%s@%04d] CRITICAL %s\n",
              filename, functionName, line_number, text);
     break;
   case FATAL_TYPE:
     fatalFlag = 1;
     sprintf (outtext, "[%s:%s@%04d] FATAL %s\n",
              filename, functionName, line_number, text);
     break;
   default:
     printf( "ERROR in dprintf: malformed calling sequence\n");
     return;
     break;
   }

  sprintf (filepath, "%s.debug", filename);

  /* Write output statement */
  switch (dprintfFlag)
  {
  default:
  case DEBUG_OFF: 
  	break;
  case DEBUG_MOSTLY_OFF: 
  	if (fatalFlag)	{
	  if ((outfile_fd = fopen (filepath, "a+")) != NULL)	  {
	    fwrite (outtext, sizeof (char), strlen (outtext), outfile_fd);
	    fclose (outfile_fd);
	  }
	}
  	break;
  case DEBUG_SCREEN_ONLY: 
  	printf ("%s", outtext);
  	break;
  case DEBUG_FILE_ONLY: 
	if ((outfile_fd = fopen (filepath, "a+")) != NULL)	{
	  fwrite (outtext, sizeof (char), strlen (outtext), outfile_fd);
	  fclose (outfile_fd);
	}
  	break;
  case DEBUG_SCREEN_AND_FILE: // BOTH
  	printf ("%s", outtext);
	if ((outfile_fd = fopen (filepath, "a+")) != NULL)	{
	  fwrite (outtext, sizeof (char), strlen (outtext), outfile_fd);
	  fclose (outfile_fd);
	}
  	break;
  }
}

/**
 * @brief Normalizes a value in a range, used for drive input
 * @param position The position in the range, starting at 0
 * @param range The size of the range that position is in
 * @return The normalized position from -1 to +1
 */
double RangeToNormalized(double position, int range){
	return(((position*2.0)/(double)range)-1.0);
}

/**
 * @brief Convert a normalized value to the corresponding value in a range.
 * This is used to convert normalized values to the servo command range.
 * @param normalizedValue The normalized value (in the -1 to +1 range)
 * @param minRange The minimum of the range (0 is default)
 * @param maxRange The maximum of the range (1 is default)
 * @return The value in the range corresponding to the input normalized value
 */
float NormalizeToRange(float normalizedValue, float minRange, float maxRange) {
	float range = maxRange-minRange;
	float temp = (float)((normalizedValue / 2.0)+ 0.5)*range;
	return (temp + minRange);
}	
float NormalizeToRange(float normalizedValue) {
	return (float)((normalizedValue / 2.0) + 0.5);
}	

/**
 * @brief Displays an activity indicator to console. 
 * Call this function like you would call printf.
 * @param fmt The format string
*/
void ShowActivity (char *fmt, ...)
{
  static char   activity_indication_string[] = "|/-\\";
  static int    ai = 3;
  va_list       args;
  char          text[1024];

  va_start (args, fmt);

  vsprintf (text, fmt, args);

  ai = ai == 3 ? 0 : ai + 1;

  printf ("%c %s \r", activity_indication_string[ai], text);
  fflush (stdout);

  va_end (args);
}

#define PI 3.14159265358979
/**
 * @brief Calculate sine wave increments (-1.0 to 1.0). 
 * The first time this is called, it sets up the time increment. Subsequent calls
 * will give values along the sine wave depending on current time. If the wave is
 * stopped and restarted, it must be reinitialized with a new "first call".
 * 
 * @param period length of time to complete a complete wave
 * @param sinStart Where to start the sine wave (0.0 = 2 pi, pi/2 = 1.0, etc.)
 */
double SinPosition (double *period, double sinStart)
{
  double rtnVal;
  static double sinePeriod=0.0;
  static double timestamp;
  double sinArg;

  //1st call
  if (period != NULL) {
    sinePeriod = *period;
	timestamp = GetTime();
	return 0.0;
  }

  //Multiplying by 2*pi to the time difference makes sinePeriod work if it's measured in seconds.
  //Adding sinStart to the part multiplied by PI, but not by 2, allows it to work as described in the comments.
  sinArg = PI *((2.0 * (GetTime() - timestamp)) + sinStart) / sinePeriod;
  rtnVal = sin (sinArg);  
  return (rtnVal);
}


/**
 * @brief Find the elapsed time since a specified time.
 * @param startTime The starting time
 * @return How long it has been since the starting time
 */
double ElapsedTime ( double startTime )  
{
	double realTime = GetTime();	
	return (realTime-startTime);
}

/**
 * @brief Initialize pan parameters
 * @param period The number of seconds to complete one pan
 */
void panInit()	{
	double period = 3.0;  	// number of seconds for one complete pan
	SinPosition(&period, 0.0);	// initial call to set up time
}

void panInit(double period)	{
	if (period < 0.0) period=3.0;
	SinPosition(&period, 0.0);	// initial call to set up time
}

/**
 * @brief Move the horizontal servo back and forth.
 * @param panServo The servo object to move
 * @param sinStart The position on the sine wave to begin the pan
 */
void panForTarget(Servo *panServo)	{ panForTarget(panServo, 0.0); }

void panForTarget(Servo *panServo, double sinStart)	{
	float normalizedSinPosition = (float)SinPosition(NULL, sinStart);
	float newServoPosition = NormalizeToRange(normalizedSinPosition);
	panServo->Set( newServoPosition );
	//ShowActivity ("pan x: normalized %f newServoPosition = %f" , 
	//		normalizedSinPosition, newServoPosition );
}


/** @brief Read a file and return non-comment output string 

Call the first time with 0 lineNumber to get the number of lines to read
Then call with each lineNumber to get one camera parameter. There should
be one property=value entry on each line, i.e. "exposure=auto"

 * @param inputFile filename to read
 * @param outputString one string
 * @param lineNumber if 0, return number of lines; else return that line number
 * @return int number of lines or -1 if error
 **/
int processFile(char *inputFile, char *outputString, int lineNumber)
{
	FILE *infile;
	int stringSize = 80;		// max size of one line in file 
	char inputStr[stringSize];
	int lineCount=0;
	  
	if (lineNumber < 0)
		  return (-1);

	if ((infile = fopen (inputFile, "r")) == NULL) {
	    printf ("Fatal error opening file %s\n",inputFile);
	    return (0);
	}

  while (!feof(infile)) {
    if (fgets (inputStr, stringSize, infile) != NULL) {
      // Skip empty lines
      if (emptyString(inputStr))
        continue;
      // Skip comment lines
      if (inputStr[0] == '#' || inputStr[0] == '!')
        continue;

      lineCount++;
      if (lineNumber == 0)
        continue;
      else
      {
    	  if (lineCount == lineNumber)
    		  break;
      }
    }
  }

  // close file 
  fclose (infile);
  // if number lines requested return the count
  if (lineNumber == 0)
	  return (lineCount);
  // check for input out of range
  if (lineNumber > lineCount)
	  return (-1);
  // return the line selected
  if (lineCount) {
    stripString(inputStr);
    strcpy(outputString, inputStr);
    return(lineCount);
  } 
  else {
    return(-1);
  }
}

/** Ignore empty string 
 * @param string to check if empty
 **/
int emptyString(char *string)
{
  int i,len;

  if(string == NULL)
    return(1);

  len = strlen(string);
  for(i=0; i<len; i++) {
    // Ignore the following characters
    if (string[i] == '\n' || string[i] == '\r' ||
        string[i] == '\t' || string[i] == ' ')
      continue;
    return(0);
  }
  return(1);
}

/** Remove special characters from string 
 * @param string to process
 **/
void stripString(char *string)
{
  int i,j,len;

  if(string == NULL)
    return;

  len = strlen(string);
  for(i=0,j=0; i<len; i++) {
    // Remove the following characters from the string
    if (string[i] == '\n' || string[i] == '\r' || string[i] == '\"')
      continue;
    // Copy anything else
    string[j++] = string[i];
  }
  string[j] = '\0'; 
}


