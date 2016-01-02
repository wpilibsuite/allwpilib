/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.image;

/**
 * Exception class which looks up nivision error codes
 *$
 * @author dtjones
 */
public class NIVisionException extends Exception {

  /**
   * Create a new NIVisionException.
   *$
   * @param msg The message to pass with the exception describing what caused
   *        it.
   */
  public NIVisionException(String msg) {
    super(msg);
  }

  /**
   * Create a new vision exception
   *$
   * @param errorCode the bad status code
   */
  public NIVisionException(int errorCode) {
    super(lookUpCode(errorCode));
  }

  private static String lookUpCode(int errorCode) {

    switch (errorCode) {
      case 0:
        return "No error.";
      case -1074396160:
        return "System error.";
      case -1074396159:
        return "Not enough memory for requested operation.";
      case -1074396158:
        return "Memory error.";
      case -1074396157:
        return "Unlicensed copy of NI Vision.";
      case -1074396156:
        return "The function requires an NI Vision 5.0 Advanced license.";
      case -1074396155:
        return "NI Vision did not initialize properly.";
      case -1074396154:
        return "The image is not large enough for the operation.";
      case -1074396153:
        return "The barcode is not a valid Codabar barcode.";
      case -1074396152:
        return "The barcode is not a valid Code 3 of 9 barcode.";
      case -1074396151:
        return "The barcode is not a valid Code93 barcode.";
      case -1074396150:
        return "The barcode is not a valid Code128 barcode.";
      case -1074396149:
        return "The barcode is not a valid EAN8 barcode.";
      case -1074396148:
        return "The barcode is not a valid EAN13 barcode.";
      case -1074396147:
        return "The barcode is not a valid Interleaved 2 of 5 barcode.";
      case -1074396146:
        return "The barcode is not a valid MSI barcode.";
      case -1074396145:
        return "The barcode is not a valid UPCA barcode.";
      case -1074396144:
        return "The Code93 barcode contains invalid shift encoding.";
      case -1074396143:
        return "The barcode type is invalid.";
      case -1074396142:
        return "The image does not represent a valid linear barcode.";
      case -1074396141:
        return "The FNC value in the Code128 barcode is not located before the first data value.";
      case -1074396140:
        return "The starting code set in the Code128 barcode is not valid.";
      case -1074396139:
        return "Not enough reserved memory in the timed environment for the requested operation.";
      case -1074396138:
        return "The function is not supported when a time limit is active.";
      case -1074396137:
        return "Quartz.dll not found.  Install DirectX 8.1 or later.";
      case -1074396136:
        return "The filter quality you provided is invalid. Valid quality values range from -1 to 1000.";
      case -1074396135:
        return "Invalid button label.";
      case -1074396134:
        return "Could not execute the function in the separate thread because the thread has not completed initialization.";
      case -1074396133:
        return "Could not execute the function in the separate thread because the thread could not initialize.";
      case -1074396132:
        return "The mask must be the same size as the template.";
      case -1074396130:
        return "The ROI must only have either a single Rectangle contour or a single Rotated Rectangle contour.";
      case -1074396129:
        return "During timed execution, you must use the preallocated version of this operation.";
      case -1074396128:
        return "An image being modified by one process cannot be requested by another process while a time limit is active.";
      case -1074396127:
        return "An image with pattern matching, calibration, or overlay information cannot be manipulated while a time limit is active.";
      case -1074396126:
        return "An image created before a time limit is started cannot be resized while a time limit is active.";
      case -1074396125:
        return "Invalid contrast threshold. The threshold value must be greater than 0. ";
      case -1074396124:
        return "NI Vision does not support the calibration ROI mode you supplied.";
      case -1074396123:
        return "NI Vision does not support the calibration mode you supplied.";
      case -1074396122:
        return "Set the foreground and background text colors to grayscale to draw on a U8 image.";
      case -1074396121:
        return "The value of the saturation threshold must be from 0 to 255.";
      case -1074396120:
        return "Not an image.";
      case -1074396119:
        return "They custom data key you supplied is invalid. The only valid character values are decimal 32-126 and 161-255. There must also be no repeated, leading, or trailing spaces.";
      case -1074396118:
        return "Step size must be greater than zero.";
      case -1074396117:
        return "Invalid matrix size in the structuring element.";
      case -1074396116:
        return "Insufficient number of calibration feature points.";
      case -1074396115:
        return "The operation is invalid in a corrected image.";
      case -1074396114:
        return "The ROI contains an invalid contour type or is not contained in the ROI learned for calibration.";
      case -1074396113:
        return "The source/input image has not been calibrated.";
      case -1074396112:
        return "The number of pixel and real-world coordinates must be equal.";
      case -1074396111:
        return "Unable to automatically detect grid because the image is too distorted.";
      case -1074396110:
        return "Invalid calibration information version.";
      case -1074396109:
        return "Invalid calibration scaling factor.";
      case -1074396108:
        return "The calibration error map cannot be computed.";
      case -1074396107:
        return "Invalid calibration template image.";
      case -1074396106:
        return "Invalid calibration template image.";
      case -1074396105:
        return "Invalid calibration template image.";
      case -1074396104:
        return "Invalid calibration template image.";
      case -1074396103:
        return "Invalid calibration template image.";
      case -1074396102:
        return "Invalid calibration template image.";
      case -1074396101:
        return "Invalid calibration template image.";
      case -1074396100:
        return "Invalid calibration template image.";
      case -1074396099:
        return "Invalid calibration template image.";
      case -1074396098:
        return "You must pass NULL for the reserved parameter.";
      case -1074396097:
        return "You entered an invalid selection in the particle parameter.";
      case -1074396096:
        return "Not an object.";
      case -1074396095:
        return "The reference points passed are inconsistent.  At least two similar pixel coordinates correspond to different real-world coordinates.";
      case -1074396094:
        return "A resource conflict occurred in the timed environment. Two processes cannot manage the same resource and be time bounded.";
      case -1074396093:
        return "A resource conflict occurred in the timed environment. Two processes cannot access the same resource and be time bounded.";
      case -1074396092:
        return "Multiple timed environments are not supported.";
      case -1074396091:
        return "A time limit cannot be started until the timed environment is initialized.";
      case -1074396090:
        return "Multiple timed environments are not supported.";
      case -1074396089:
        return "The timed environment is already initialized.";
      case -1074396088:
        return "The results of the operation exceeded the size limits on the output data arrays.";
      case -1074396087:
        return "No time limit is available to stop.";
      case -1074396086:
        return "A time limit could not be set.";
      case -1074396085:
        return "The timed environment could not be initialized.";
      case -1074396084:
        return "No initialized timed environment is available to close.";
      case -1074396083:
        return "The time limit has expired.";
      case -1074396082:
        return "Only 8-bit images support the use of palettes.  Either do not use a palette, or convert your image to an 8-bit image before using a palette.";
      case -1074396081:
        return "Incorrect password.";
      case -1074396080:
        return "Invalid image type.";
      case -1074396079:
        return "Invalid metafile handle.";
      case -1074396077:
        return "Incompatible image type.";
      case -1074396076:
        return "Unable to fit a line for the primary axis.";
      case -1074396075:
        return "Unable to fit a line for the secondary axis.";
      case -1074396074:
        return "Incompatible image size.";
      case -1074396073:
        return "When the mask's offset was applied, the mask was entirely outside of the image.";
      case -1074396072:
        return "Invalid image border.";
      case -1074396071:
        return "Invalid scan direction.";
      case -1074396070:
        return "Unsupported function.";
      case -1074396069:
        return "NI Vision does not support the color mode you specified.";
      case -1074396068:
        return "The function does not support the requested action.";
      case -1074396067:
        return "The source image and destination image must be different.";
      case -1074396066:
        return "Invalid point symbol.";
      case -1074396065:
        return "Cannot resize an image in an acquisition buffer.";
      case -1074396064:
        return "This operation is not supported for images in an acquisition buffer.";
      case -1074396063:
        return "The external buffer must be aligned on a 4-byte boundary. The line width and border pixels must be 4-byte aligned, as well.";
      case -1074396062:
        return "The tolerance parameter must be greater than or equal to 0.";
      case -1074396061:
        return "The size of each dimension of the window must be greater than 2 and less than or equal to the size of the image in the corresponding dimension.";
      case -1074396060:
        return "Lossless compression cannot be used with the floating point wavelet transform mode. Either set the wavelet transform mode to integer, or use lossy compression.";
      case -1074396059:
        return "Invalid maximum number of iterations. Maximum number of iterations must be greater than zero.";
      case -1074396058:
        return "Invalid rotation mode.";
      case -1074396057:
        return "Invalid search vector width. The width must be an odd number greater than zero.";
      case -1074396056:
        return "Invalid matrix mirror mode.";
      case -1074396055:
        return "Invalid aspect ratio. Valid aspect ratios must be greater than or equal to zero.";
      case -1074396054:
        return "Invalid cell fill type.";
      case -1074396053:
        return "Invalid border integrity. Valid values range from 0 to 100.";
      case -1074396052:
        return "Invalid demodulation mode.";
      case -1074396051:
        return "Invalid cell filter mode.";
      case -1074396050:
        return "Invalid ECC type.";
      case -1074396049:
        return "Invalid matrix polarity.";
      case -1074396048:
        return "Invalid cell sample size.";
      case -1074396047:
        return "Invalid linear average mode.";
      case -1074396046:
        return "When using a region of interest that is not a rectangle, you must specify the contrast mode of the barcode as either black on white or white on black.";
      case -1074396045:
        return "Invalid 2-D barcode Data Matrix subtype.";
      case -1074396044:
        return "Invalid 2-D barcode shape.";
      case -1074396043:
        return "Invalid 2-D barcode cell shape.";
      case -1074396042:
        return "Invalid 2-D barcode contrast.";
      case -1074396041:
        return "Invalid 2-D barcode type.";
      case -1074396040:
        return "Cannot access NI-IMAQ driver.";
      case -1074396039:
        return "I/O error.";
      case -1074396038:
        return "When searching for a coordinate system, the number of lines to fit must be 1.";
      case -1074396037:
        return "Trigger timeout.";
      case -1074396036:
        return "The Skeleton mode you specified is invalid.";
      case -1074396035:
        return "The template image does not contain enough information for learning the aggressive search strategy.";
      case -1074396034:
        return "The template image does not contain enough edge information for the sample size(s) requested.";
      case -1074396033:
        return "Invalid template descriptor.";
      case -1074396032:
        return "The template descriptor does not contain data required for the requested search strategy in rotation-invariant matching.";
      case -1074396026:
        return "Invalid process type for edge detection.";
      case -1074396025:
        return "Angle range value should be equal to or greater than zero.";
      case -1074396024:
        return "Minimum coverage value should be greater than zero.";
      case -1074396023:
        return "The angle tolerance should be equal to or greater than 0.001.";
      case -1074396022:
        return "Invalid search mode for detecting straight edges";
      case -1074396021:
        return "Invalid kernel size for edge detection. The minimum kernel size is 3, the maximum kernel size is 1073741823 and the kernel size must be odd.";
      case -1074396020:
        return "Invalid grading mode.";
      case -1074396019:
        return "Invalid threshold percentage. Valid values range from 0 to 100.";
      case -1074396018:
        return "Invalid edge polarity search mode.";
      case -1074396017:
        return "The AIM grading data attached to the image you tried to open was created with a newer version of NI Vision. Upgrade to the latest version of NI Vision to read this file.";
      case -1074396016:
        return "No video driver is installed.";
      case -1074396015:
        return "Unable to establish network connection with remote system.";
      case -1074396014:
        return "RT Video Out does not support displaying the supplied image type at the selected color depth.";
      case -1074396013:
        return "Invalid video mode.";
      case -1074396012:
        return "Unable to display remote image on network connection.";
      case -1074396011:
        return "Unable to establish network connection.";
      case -1074396010:
        return "Invalid frame number.";
      case -1074396009:
        return "An internal DirectX error has occurred.  Try upgrading to the latest version of DirectX.";
      case -1074396008:
        return "An appropriate DirectX filter to process this file could not be found.  Install the filter that was used to create this AVI. Upgrading to the latest version of DirectX may correct this error.  NI Vision requires DirectX 8.1 or higher.";
      case -1074396007:
        return "Incompatible compression filter.";
      case -1074396006:
        return "Unknown compression filter.";
      case -1074396005:
        return "Invalid AVI session.";
      case -1074396004:
        return "A software key is restricting the use of this compression filter.";
      case -1074396003:
        return "The data for this frame exceeds the data buffer size specified when creating the AVI file.";
      case -1074396002:
        return "Invalid line gauge method.";
      case -1074396001:
        return "There are too many AVI sessions open.  You must close a session before you can open another one.";
      case -1074396000:
        return "Invalid file header.";
      case -1074395999:
        return "Invalid file type.";
      case -1074395998:
        return "Invalid color table.";
      case -1074395997:
        return "Invalid parameter.";
      case -1074395996:
        return "File is already open for writing.";
      case -1074395995:
        return "File not found.";
      case -1074395994:
        return "Too many files open.";
      case -1074395993:
        return "File I/O error.";
      case -1074395992:
        return "File access denied.";
      case -1074395991:
        return "NI Vision does not support the file type you specified.";
      case -1074395990:
        return "Could not read Vision info from file.";
      case -1074395989:
        return "Unable to read data.";
      case -1074395988:
        return "Unable to write data.";
      case -1074395987:
        return "Premature end of file.";
      case -1074395986:
        return "Invalid file format.";
      case -1074395985:
        return "Invalid file operation.";
      case -1074395984:
        return "NI Vision does not support the file data type you specified.";
      case -1074395983:
        return "Disk full.";
      case -1074395982:
        return "The frames per second in an AVI must be greater than zero.";
      case -1074395981:
        return "The buffer that was passed in is not big enough to hold all of the data.";
      case -1074395980:
        return "Error initializing COM.";
      case -1074395979:
        return "The image has invalid particle information.  Call imaqCountParticles on the image to create particle information.";
      case -1074395978:
        return "Invalid particle number.";
      case -1074395977:
        return "The AVI file was created in a newer version of NI Vision. Upgrade to the latest version of NI Vision to read this AVI file.";
      case -1074395976:
        return "The color palette must have exactly 0 or 256 entries.";
      case -1074395975:
        return "DirectX has timed out reading or writing the AVI file.  When closing an AVI file, try adding an additional delay.  When reading an AVI file, try reducing CPU and disk load.";
      case -1074395974:
        return "NI Vision does not support reading JPEG2000 files with this colorspace method.";
      case -1074395973:
        return "NI Vision does not support reading JPEG2000 files with more than one layer.";
      case -1074395972:
        return "DirectX is unable to enumerate the compression filters. This is caused by a third-party compression filter that is either improperly installed or is preventing itself from being enumerated. Remove any recently installed compression filters and try again.";
      case -1074395971:
        return "The offset you specified must be size 2.";
      case -1074395960:
        return "Initialization error.";
      case -1074395959:
        return "Unable to create window.";
      case -1074395958:
        return "Invalid window ID.";
      case -1074395957:
        return "The array sizes are not compatible.";
      case -1074395956:
        return "The quality you provided is invalid. Valid quality values range from -1 to 1000.";
      case -1074395955:
        return "Invalid maximum wavelet transform level.  Valid values range from 0 to 255.";
      case -1074395954:
        return "The quantization step size must be greater than or equal to 0.";
      case -1074395953:
        return "Invalid wavelet transform mode.";
      case -1074395920:
        return "Invalid number of classes.";
      case -1074395880:
        return "Invalid particle.";
      case -1074395879:
        return "Invalid measure number.";
      case -1074395878:
        return "The ImageBase Display control does not support writing this property node.";
      case -1074395877:
        return "The specified color mode requires the use of imaqChangeColorSpace2.";
      case -1074395876:
        return "This function does not currently support the color mode you specified.";
      case -1074395875:
        return "The barcode is not a valid Pharmacode symbol";
      case -1074395840:
        return "Invalid handle table index.";
      case -1074395837:
        return "The compression ratio must be greater than or equal to 1.";
      case -1074395801:
        return "The ROI contains too many contours.";
      case -1074395800:
        return "Protection error.";
      case -1074395799:
        return "Internal error.";
      case -1074395798:
        return "The size of the feature vector in the custom sample must match the size of those you have already added.";
      case -1074395797:
        return "Not a valid classifier session.";
      case -1074395796:
        return "You requested an invalid Nearest Neighbor classifier method.";
      case -1074395795:
        return "The k parameter must be greater than two.";
      case -1074395794:
        return "The k parameter must be <= the number of samples in each class.";
      case -1074395793:
        return "This classifier session is compact. Only the Classify and Dispose functions may be called on a compact classifier session.";
      case -1074395792:
        return "This classifier session is not trained. You may only call this function on a trained classifier session.";
      case -1074395791:
        return "This classifier function cannot be called on this type of classifier session.";
      case -1074395790:
        return "You requested an invalid distance metric.";
      case -1074395789:
        return "The classifier session you tried to open was created with a newer version of NI Vision. Upgrade to the latest version of NI Vision to read this file.";
      case -1074395788:
        return "This operation cannot be performed because you have not added any samples.";
      case -1074395787:
        return "You requested an invalid classifier type.";
      case -1074395786:
        return "The sum of Scale Dependence and Symmetry Dependence must be less than 1000.";
      case -1074395785:
        return "The image yielded no particles.";
      case -1074395784:
        return "The limits you supplied are not valid.";
      case -1074395783:
        return "The Sample Index fell outside the range of Samples.";
      case -1074395782:
        return "The description must be <= 255 characters.";
      case -1074395781:
        return "The engine for this classifier session does not support this operation.";
      case -1074395780:
        return "You requested an invalid particle type.";
      case -1074395779:
        return "You may only save a session in compact form if it is trained.";
      case -1074395778:
        return "The Kernel size must be smaller than the image size.";
      case -1074395777:
        return "The session you read from file must be the same type as the session you passed in.";
      case -1074395776:
        return "You can not use a compact classification file with read options other than Read All.";
      case -1074395775:
        return "The ROI you passed in may only contain closed contours.";
      case -1074395774:
        return "You must pass in a label.";
      case -1074395773:
        return "You must provide a destination image.";
      case -1074395772:
        return "You provided an invalid registration method.";
      case -1074395771:
        return "The golden template you tried to open was created with a newer version of NI Vision. Upgrade to the latest version of NI Vision to read this file.";
      case -1074395770:
        return "Invalid golden template.";
      case -1074395769:
        return "Edge Thickness to Ignore must be greater than zero.";
      case -1074395768:
        return "Scale must be greater than zero.";
      case -1074395767:
        return "The supplied scale is invalid for your template.";
      case -1074395766:
        return "This backwards-compatibility function can not be used with this session. Use newer, supported functions instead.";
      case -1074395763:
        return "You must provide a valid normalization method.";
      case -1074395762:
        return "The deviation factor for Niblack local threshold must be between 0 and 1.";
      case -1074395760:
        return "Board not found.";
      case -1074395758:
        return "Board not opened.";
      case -1074395757:
        return "DLL not found.";
      case -1074395756:
        return "DLL function not found.";
      case -1074395754:
        return "Trigger timeout.";
      case -1074395728:
        return "NI Vision does not support the search mode you provided.";
      case -1074395727:
        return "NI Vision does not support the search mode you provided for the type of 2D barcode for which you are searching.";
      case -1074395726:
        return "matchFactor has been obsoleted. Instead, set the initialMatchListLength and matchListReductionFactor in the MatchPatternAdvancedOptions structure.";
      case -1074395725:
        return "The data was stored with a newer version of NI Vision. Upgrade to the latest version of NI Vision to read this data.";
      case -1074395724:
        return "The size you specified is out of the valid range.";
      case -1074395723:
        return "The key you specified cannot be found in the image.";
      case -1074395722:
        return "Custom classifier sessions only classify feature vectors. They do not support classifying images.";
      case -1074395721:
        return "NI Vision does not support the bit depth you supplied for the image you supplied.";
      case -1074395720:
        return "Invalid ROI.";
      case -1074395719:
        return "Invalid ROI global rectangle.";
      case -1074395718:
        return "The version of LabVIEW or BridgeVIEW you are running does not support this operation.";
      case -1074395717:
        return "The range you supplied is invalid.";
      case -1074395716:
        return "NI Vision does not support the scaling method you provided.";
      case -1074395715:
        return "NI Vision does not support the calibration unit you supplied.";
      case -1074395714:
        return "NI Vision does not support the axis orientation you supplied.";
      case -1074395713:
        return "Value not in enumeration.";
      case -1074395712:
        return "You selected a region that is not of the right type.";
      case -1074395711:
        return "You specified a viewer that does not contain enough regions.";
      case -1074395710:
        return "The image has too many particles for this process.";
      case -1074395709:
        return "The AVI session has not been opened.";
      case -1074395708:
        return "The AVI session is a write session, but this operation requires a read session.";
      case -1074395707:
        return "The AVI session is a read session, but this operation requires a write session.";
      case -1074395706:
        return "This AVI session is already open. You must close it before calling the Create or Open functions.";
      case -1074395705:
        return "The data is corrupted and cannot be read.";
      case -1074395704:
        return "Invalid compression type.";
      case -1074395703:
        return "Invalid type of flatten.";
      case -1074395702:
        return "The length of the edge detection line must be greater than zero.";
      case -1074395701:
        return "The maximum Data Matrix barcode size must be equal to or greater than the minimum Data Matrix barcode size.";
      case -1074395700:
        return "The function requires the operating system to be Microsoft Windows 2000 or newer.";
      case -1074395656:
        return "You must specify the same value for the smooth contours advanced match option for all templates you want to match.";
      case -1074395655:
        return "You must specify the same value for the enable calibration support advanced match option for all templates you want to match.";
      case -1074395654:
        return "The source image does not contain grading information. You must prepare the source image for grading when reading the Data Matrix, and you cannot change the contents of the source image between reading and grading the Data Matrix.";
      case -1074395653:
        return "The multiple geometric matching template you tried to open was created with a newer version of NI Vision. Upgrade to the latest version of NI Vision to read this file.";
      case -1074395652:
        return "The geometric matching template you tried to open was created with a newer version of NI Vision. Upgrade to the latest version of NI Vision to read this file.";
      case -1074395651:
        return "You must specify the same edge filter size for all the templates you want to match.";
      case -1074395650:
        return "You must specify the same curve extraction mode for all the templates you want to match.";
      case -1074395649:
        return "The geometric feature type specified is invalid.";
      case -1074395648:
        return "You supplied a template that was not learned.";
      case -1074395647:
        return "Invalid multiple geometric template.";
      case -1074395646:
        return "Need at least one template to learn.";
      case -1074395645:
        return "You supplied an invalid number of labels.";
      case -1074395644:
        return "Labels must be <= 255 characters.";
      case -1074395643:
        return "You supplied an invalid number of match options.";
      case -1074395642:
        return "Cannot find a label that matches the one you specified.";
      case -1074395641:
        return "Duplicate labels are not allowed.";
      case -1074395640:
        return "The number of zones found exceeded the capacity of the algorithm.";
      case -1074395639:
        return "The hatch style for the window background is invalid.";
      case -1074395638:
        return "The fill style for the window background is invalid.";
      case -1074395637:
        return "Your hardware is not supported by DirectX and cannot be put into NonTearing mode.";
      case -1074395636:
        return "DirectX is required for this feature.  Please install the latest version..";
      case -1074395635:
        return "The passed shape descriptor is invalid.";
      case -1074395634:
        return "Invalid max match overlap.  Values must be between -1 and 100.";
      case -1074395633:
        return "Invalid minimum match separation scale.  Values must be greater than or equal to -1.";
      case -1074395632:
        return "Invalid minimum match separation angle.  Values must be between -1 and 360.";
      case -1074395631:
        return "Invalid minimum match separation distance.  Values must be greater than or equal to -1.";
      case -1074395630:
        return "Invalid maximum number of features learn. Values must be integers greater than zero.";
      case -1074395629:
        return "Invalid maximum pixel distance from line. Values must be positive real numbers.";
      case -1074395628:
        return "Invalid geometric matching template image.";
      case -1074395627:
        return "The template does not contain enough features for geometric matching.";
      case -1074395626:
        return "The template does not contain enough features for geometric matching.";
      case -1074395625:
        return "You specified an invalid value for the match constraint value of the  range settings.";
      case -1074395624:
        return "Invalid occlusion range. Valid values for the bounds range from 0 to 100 and the upper bound must be greater than or equal to the lower bound.";
      case -1074395623:
        return "Invalid scale range. Values for the lower bound must be a positive real numbers and the upper bound must be greater than or equal to the lower bound.";
      case -1074395622:
        return "Invalid match geometric pattern setup data.";
      case -1074395621:
        return "Invalid learn geometric pattern setup data.";
      case -1074395620:
        return "Invalid curve extraction mode.";
      case -1074395619:
        return "You can specify only one occlusion range.";
      case -1074395618:
        return "You can specify only one scale range.";
      case -1074395617:
        return "The minimum number of features must be less than or equal to the maximum number of features.";
      case -1074395616:
        return "Invalid edge filter size.";
      case -1074395615:
        return "Invalid minimum strength for features. Values must be positive real numbers.";
      case -1074395614:
        return "Invalid aspect ratio for rectangular features. Values must be positive real numbers in the range 0.01 to 1.0.";
      case -1074395613:
        return "Invalid minimum length for linear features. Values must be integers greater than 0.";
      case -1074395612:
        return "Invalid minimum radius for circular features. Values must be integers greater than 0.";
      case -1074395611:
        return "Invalid minimum rectangle dimension. Values must be integers greater than 0.";
      case -1074395610:
        return "Invalid initial match list length. Values must be integers greater than 5.";
      case -1074395609:
        return "Invalid subpixel tolerance. Values must be positive real numbers.";
      case -1074395608:
        return "Invalid number of subpixel iterations. Values must be integers greater 10.";
      case -1074395607:
        return "Invalid maximum number of features used per match. Values must be integers greater than or equal to zero.";
      case -1074395606:
        return "Invalid minimum number of features used for matching. Values must be integers greater than zero.";
      case -1074395605:
        return "Invalid maximum end point gap. Valid values range from 0 to 32767.";
      case -1074395604:
        return "Invalid column step. Valid range is 1 to 255.";
      case -1074395603:
        return "Invalid row step. Valid range is 1 to 255.";
      case -1074395602:
        return "Invalid minimum length. Valid values must be greater than or equal to zero.";
      case -1074395601:
        return "Invalid edge threshold. Valid values range from 1 to 360.";
      case -1074395600:
        return "You must provide information about the subimage within the browser.";
      case -1074395598:
        return "The acceptance level is outside the valid range of  0 to 1000.";
      case -1074395597:
        return "Not a valid OCR session.";
      case -1074395596:
        return "Invalid character size. Character size must be >= 1.";
      case -1074395595:
        return "Invalid threshold mode value.";
      case -1074395594:
        return "Invalid substitution character. Valid substitution characters are ASCII values that range from 1 to 254.";
      case -1074395593:
        return "Invalid number of blocks. Number of blocks must be >= 4 and <= 50.";
      case -1074395592:
        return "Invalid read strategy.";
      case -1074395591:
        return "Invalid character index.";
      case -1074395590:
        return "Invalid number of character positions. Valid values range from 0 to 255.";
      case -1074395589:
        return "Invalid low threshold value. Valid threshold values range from 0 to 255.";
      case -1074395588:
        return "Invalid high threshold value. Valid threshold values range from 0 to 255.";
      case -1074395587:
        return "The low threshold must be less than the high threshold.";
      case -1074395586:
        return "Invalid lower threshold limit. Valid lower threshold limits range from 0 to 255.";
      case -1074395585:
        return "Invalid upper threshold limit. Valid upper threshold limits range from 0 to 255.";
      case -1074395584:
        return "The lower threshold limit must be less than the upper threshold limit.";
      case -1074395583:
        return "Invalid minimum character spacing value. Character spacing must be >= 0 pixels.";
      case -1074395582:
        return "Invalid maximum horizontal element spacing value. Maximum horizontal element spacing must be >= 0.";
      case -1074395581:
        return "Invalid maximum vertical element spacing value. Maximum vertical element spacing must be >= 0.";
      case -1074395580:
        return "Invalid minimum bounding rectangle width. Minimum bounding rectangle width must be >= 1.";
      case -1074395579:
        return "Invalid aspect ratio value. The aspect ratio must be zero or >= 100.";
      case -1074395578:
        return "Invalid or corrupt character set file.";
      case -1074395577:
        return "The character value must not be an empty string.";
      case -1074395576:
        return "Character values must be <=255 characters.";
      case -1074395575:
        return "Invalid number of erosions. The number of erosions must be >= 0.";
      case -1074395574:
        return "The character set description must be <=255 characters.";
      case -1074395573:
        return "The character set file was created by a newer version of NI Vision. Upgrade to the latest version of NI Vision to read the character set file.";
      case -1074395572:
        return "You must specify characters for a string. A string cannot contain integers.";
      case -1074395571:
        return "This attribute is read-only.";
      case -1074395570:
        return "This attribute requires a Boolean value.";
      case -1074395569:
        return "Invalid attribute.";
      case -1074395568:
        return "This attribute requires integer values.";
      case -1074395567:
        return "String values are invalid for this attribute. Enter a boolean value.";
      case -1074395566:
        return "Boolean values are not valid for this attribute. Enter an integer value.";
      case -1074395565:
        return "Requires a single-character string.";
      case -1074395564:
        return "Invalid predefined character value.";
      case -1074395563:
        return "This copy of NI OCR is unlicensed.";
      case -1074395562:
        return "String values are not valid for this attribute. Enter a Boolean value.";
      case -1074395561:
        return "The number of characters in the character value must match the number of objects in the image.";
      case -1074395560:
        return "Invalid object index.";
      case -1074395559:
        return "Invalid read option.";
      case -1074395558:
        return "The minimum character size must be less than the maximum character size.";
      case -1074395557:
        return "The minimum character bounding rectangle width must be less than the maximum character bounding rectangle width.";
      case -1074395556:
        return "The minimum character bounding rectangle height must be less than the maximum character bounding rectangle height.";
      case -1074395555:
        return "The maximum horizontal element spacing value must not exceed the minimum character spacing value.";
      case -1074395554:
        return "Invalid read resolution.";
      case -1074395553:
        return "Invalid minimum bounding rectangle height. The minimum bounding rectangle height must be >= 1.";
      case -1074395552:
        return "Not a valid character set.";
      case -1074395551:
        return "A trained OCR character cannot be renamed while it is a reference character.";
      case -1074395550:
        return "A character cannot have an ASCII value of 255.";
      case -1074395549:
        return "The number of objects found does not match the number of expected characters or patterns to verify.";
      case -1074395410:
        return "NI Vision does not support less than one icon per line.";
      case -1074395409:
        return "Invalid subpixel divisions.";
      case -1074395408:
        return "Invalid detection mode.";
      case -1074395407:
        return "Invalid contrast value. Valid contrast values range from 0 to 255.";
      case -1074395406:
        return "The coordinate system could not be found on this image.";
      case -1074395405:
        return "NI Vision does not support the text orientation value you supplied.";
      case -1074395404:
        return "UnwrapImage does not support the interpolation method value you supplied.  Valid interpolation methods are zero order and bilinear. ";
      case -1074395403:
        return "The image was created in a newer version of NI Vision. Upgrade to the latest version of NI Vision to use this image.";
      case -1074395402:
        return "The function does not support the maximum number of points that you specified.";
      case -1074395401:
        return "The function does not support the matchFactor that you specified.";
      case -1074395400:
        return "The operation you have given Multicore Options is invalid. Please see the available enumeration values for Multicore Operation.";
      case -1074395399:
        return "You have given Multicore Options an invalid argument.";
      case -1074395397:
        return "A complex image is required.";
      case -1074395395:
        return "The input image must be a color image.";
      case -1074395394:
        return "The color mask removes too much color information.";
      case -1074395393:
        return "The color template image is too small.";
      case -1074395392:
        return "The color template image is too large.";
      case -1074395391:
        return "The contrast in the hue plane of the image is too low for learning shape features.";
      case -1074395390:
        return "The contrast in the luminance plane of the image is too low to learn shape features.";
      case -1074395389:
        return "Invalid color learn setup data.";
      case -1074395388:
        return "Invalid color learn setup data.";
      case -1074395387:
        return "Invalid color match setup data.";
      case -1074395386:
        return "Invalid color match setup data.";
      case -1074395385:
        return "Rotation-invariant color pattern matching requires a feature mode including shape.";
      case -1074395384:
        return "Invalid color template image.";
      case -1074395383:
        return "Invalid color template image.";
      case -1074395382:
        return "Invalid color template image.";
      case -1074395381:
        return "Invalid color template image.";
      case -1074395380:
        return "Invalid color template image.";
      case -1074395379:
        return "Invalid color template image.";
      case -1074395378:
        return "Invalid color template image.";
      case -1074395377:
        return "Invalid color template image.";
      case -1074395376:
        return "The color template image does not contain data required for shift-invariant color matching.";
      case -1074395375:
        return "Invalid color template image.";
      case -1074395374:
        return "Invalid color template image.";
      case -1074395373:
        return "Invalid color template image.";
      case -1074395372:
        return "The color template image does not contain data required for rotation-invariant color matching.";
      case -1074395371:
        return "Invalid color template image.";
      case -1074395370:
        return "Invalid color template image.";
      case -1074395369:
        return "Invalid color template image.";
      case -1074395368:
        return "Invalid color template image.";
      case -1074395367:
        return "Invalid color template image.";
      case -1074395366:
        return "The color template image does not contain data required for color matching in shape feature mode.";
      case -1074395365:
        return "The color template image does not contain data required for color matching in color feature mode.";
      case -1074395364:
        return "The ignore color spectra array is invalid.";
      case -1074395363:
        return "Invalid subsampling ratio.";
      case -1074395362:
        return "Invalid pixel width.";
      case -1074395361:
        return "Invalid steepness.";
      case -1074395360:
        return "Invalid complex plane.";
      case -1074395357:
        return "Invalid color ignore mode.";
      case -1074395356:
        return "Invalid minimum match score. Acceptable values range from 0 to 1000.";
      case -1074395355:
        return "Invalid number of matches requested. You must request a minimum of one match.";
      case -1074395354:
        return "Invalid color weight. Acceptable values range from 0 to 1000.";
      case -1074395353:
        return "Invalid search strategy.";
      case -1074395352:
        return "Invalid feature mode.";
      case -1074395351:
        return "NI Vision does not support rectangles with negative widths or negative heights.";
      case -1074395350:
        return "NI Vision does not support the vision information type you supplied.";
      case -1074395349:
        return "NI Vision does not support the SkeletonMethod value you supplied.";
      case -1074395348:
        return "NI Vision does not support the 3DPlane value you supplied.";
      case -1074395347:
        return "NI Vision does not support the 3DDirection value you supplied.";
      case -1074395346:
        return "imaqRotate does not support the InterpolationMethod value you supplied.";
      case -1074395345:
        return "NI Vision does not support the axis of symmetry you supplied.";
      case -1074395343:
        return "You must pass a valid file name. Do not pass in NULL.";
      case -1074395340:
        return "NI Vision does not support the SizeType value you supplied.";
      case -1074395336:
        return "You specified the dispatch status of an unknown algorithm.";
      case -1074395335:
        return "You are attempting to set the same algorithm to dispatch and to not dispatch. Remove one of the conflicting settings.";
      case -1074395334:
        return "NI Vision does not support the Conversion Method value you supplied.";
      case -1074395333:
        return "NI Vision does not support the VerticalTextAlignment value you supplied.";
      case -1074395332:
        return "NI Vision does not support the CompareFunction value you supplied.";
      case -1074395331:
        return "NI Vision does not support the BorderMethod value you supplied.";
      case -1074395330:
        return "Invalid border size. Acceptable values range from 0 to 50.";
      case -1074395329:
        return "NI Vision does not support the OutlineMethod value you supplied.";
      case -1074395328:
        return "NI Vision does not support the InterpolationMethod value you supplied.";
      case -1074395327:
        return "NI Vision does not support the ScalingMode value you supplied.";
      case -1074395326:
        return "imaqDrawLineOnImage does not support the DrawMode value you supplied.";
      case -1074395325:
        return "NI Vision does not support the DrawMode value you supplied.";
      case -1074395324:
        return "NI Vision does not support the ShapeMode value you supplied.";
      case -1074395323:
        return "NI Vision does not support the FontColor value you supplied.";
      case -1074395322:
        return "NI Vision does not support the TextAlignment value you supplied.";
      case -1074395321:
        return "NI Vision does not support the MorphologyMethod value you supplied.";
      case -1074395320:
        return "The template image is empty.";
      case -1074395319:
        return "NI Vision does not support the interpolation type you supplied.";
      case -1074395318:
        return "You supplied an insufficient number of points to perform this operation.";
      case -1074395317:
        return "You specified a point that lies outside the image.";
      case -1074395316:
        return "Invalid kernel code.";
      case -1074395313:
        return "Writing files is not supported on this device.";
      case -1074395312:
        return "The input image does not seem to be a valid LCD or LED calibration image.";
      case -1074395311:
        return "The color spectrum array you provided does not contain enough elements or contains an element set to not-a-number (NaN).";
      case -1074395310:
        return "NI Vision does not support the PaletteType value you supplied.";
      case -1074395309:
        return "NI Vision does not support the WindowThreadPolicy value you supplied.";
      case -1074395308:
        return "NI Vision does not support the ColorSensitivity value you supplied.";
      case -1074395307:
        return "The precision parameter must be greater than 0.";
      case -1074395306:
        return "NI Vision does not support the Tool value you supplied.";
      case -1074395305:
        return "NI Vision does not support the ReferenceMode value you supplied.";
      case -1074395304:
        return "NI Vision does not support the MathTransformMethod value you supplied.";
      case -1074395303:
        return "Invalid number of classes for auto threshold. Acceptable values range from 2 to 256.";
      case -1074395302:
        return "NI Vision does not support the threshold method value you supplied.";
      case -1074395301:
        return "The ROI you passed into imaqGetMeterArc must consist of two lines.";
      case -1074395300:
        return "NI Vision does not support the MeterArcMode value you supplied.";
      case -1074395299:
        return "NI Vision does not support the ComplexPlane value you supplied.";
      case -1074395298:
        return "You can perform this operation on a real or an imaginary ComplexPlane only.";
      case -1074395297:
        return "NI Vision does not support the ParticleInfoMode value you supplied.";
      case -1074395296:
        return "NI Vision does not support the BarcodeType value you supplied.";
      case -1074395295:
        return "imaqInterpolatePoints does not support the InterpolationMethod value you supplied.";
      case -1074395294:
        return "The contour index you supplied is larger than the number of contours in the ROI.";
      case -1074395293:
        return "The supplied ContourID did not correlate to a contour inside the ROI.";
      case -1074395292:
        return "Do not supply collinear points for this operation.";
      case -1074395291:
        return "Shape Match requires the image to contain only pixel values of 0 or 1.";
      case -1074395290:
        return "The template you supplied for ShapeMatch contains no shape information.";
      case -1074395287:
        return "The line you provided contains two identical points, or one of the coordinate locations for the line is not a number (NaN).";
      case -1074395286:
        return "Invalid concentric rake direction.";
      case -1074395285:
        return "Invalid spoke direction.";
      case -1074395284:
        return "Invalid edge process.";
      case -1074395283:
        return "Invalid rake direction.";
      case -1074395282:
        return "Unable to draw to viewer. You must have the latest version of the control.";
      case -1074395281:
        return "Your image must be larger than its border size for this operation.";
      case -1074395280:
        return "The ROI must only have a single Rectangle contour.";
      case -1074395279:
        return "ROI is not a polygon.";
      case -1074395278:
        return "LCD image is not a number.";
      case -1074395277:
        return "The decoded barcode information did not pass the checksum test.";
      case -1074395276:
        return "You specified parallel lines for the meter ROI.";
      case -1074395275:
        return "Invalid browser image.";
      case -1074395270:
        return "Cannot divide by zero.";
      case -1074395269:
        return "Null pointer.";
      case -1074395268:
        return "The linear equations are not independent.";
      case -1074395267:
        return "The roots of the equation are complex.";
      case -1074395265:
        return "The barcode does not match the type you specified.";
      case -1074395263:
        return "No lit segment.";
      case -1074395262:
        return "The LCD does not form a known digit.";
      case -1074395261:
        return "An internal error occurred while attempting to access an invalid coordinate on an image.";
      case -1074395260:
        return "An internal memory error occurred.";
      case -1074395258:
        return "The filter width must be odd for the Canny operator.";
      case -1074395257:
        return "You supplied an invalid edge direction in the Canny operator.";
      case -1074395256:
        return "The window size must be odd for the Canny operator. ";
      case -1074395253:
        return "Invalid learn mode.";
      case -1074395252:
        return "Invalid learn setup data.";
      case -1074395251:
        return "Invalid match mode.";
      case -1074395250:
        return "Invalid match setup data.";
      case -1074395249:
        return "At least one range in the array of rotation angle ranges exceeds 360 degrees.";
      case -1074395248:
        return "The array of rotation angle ranges contains too many ranges.";
      case -1074395247:
        return "Invalid template descriptor.";
      case -1074395246:
        return "Invalid template descriptor.";
      case -1074395245:
        return "Invalid template descriptor.";
      case -1074395244:
        return "Invalid template descriptor.";
      case -1074395243:
        return "The template descriptor was created with a newer version of NI Vision. Upgrade to the latest version of NI Vision to use this template.";
      case -1074395242:
        return "Invalid template descriptor.";
      case -1074395241:
        return "The template descriptor does not contain data required for rotation-invariant matching.";
      case -1074395240:
        return "Invalid template descriptor.";
      case -1074395239:
        return "Invalid template descriptor.";
      case -1074395238:
        return "The template descriptor does not contain data required for shift-invariant matching.";
      case -1074395237:
        return "Invalid template descriptor.";
      case -1074395235:
        return "The template image does not contain enough contrast.";
      case -1074395234:
        return "The template image is too small.";
      case -1074395233:
        return "The template image is too large.";
      case -1074395212:
        return "The size of the template string must match the size of the string you are trying to correct.";
      case -1074395211:
        return "The supplied text template contains nonstandard characters that cannot be generated by OCR.";
      case -1074395210:
        return "At least one character in the text template was of a lexical class that did not match the supplied character reports.";
      case -1074395203:
        return "The OCR library cannot be initialized correctly.";
      case -1074395201:
        return "There was a failure when loading one of the internal OCR engine or LabView libraries.";
      case -1074395200:
        return "One of the parameters supplied to the OCR function that generated this error is invalid.";
      case -1074395179:
        return "The OCR engine failed during the preprocessing stage.";
      case -1074395178:
        return "The OCR engine failed during the recognition stage.";
      case -1074395175:
        return "The provided filename is not valid user dictionary filename.";
      case -1074395174:
        return "NI Vision does not support the AutoOrientMode value you supplied.";
      case -1074395173:
        return "NI Vision does not support the Language value you supplied.";
      case -1074395172:
        return "NI Vision does not support the CharacterSet value you supplied.";
      case -1074395171:
        return "The system could not locate the initialization file required for OCR initialization.";
      case -1074395170:
        return "NI Vision does not support the CharacterType value you supplied.";
      case -1074395169:
        return "NI Vision does not support the RecognitionMode value you supplied.";
      case -1074395168:
        return "NI Vision does not support the AutoCorrectionMode value you supplied.";
      case -1074395167:
        return "NI Vision does not support the OutputDelimiter value you supplied.";
      case -1074395166:
        return "The system could not locate the OCR binary directory required for OCR initialization.";
      case -1074395165:
        return "The system could not locate the OCR weights directory required for OCR initialization.";
      case -1074395164:
        return "The supplied word could not be added to the user dictionary.";
      case -1074395163:
        return "NI Vision does not support the CharacterPreference value you supplied.";
      case -1074395162:
        return "NI Vision does not support the CorrectionMethod value you supplied.";
      case -1074395161:
        return "NI Vision does not support the CorrectionLevel value you supplied.";
      case -1074395160:
        return "NI Vision does not support the maximum point size you supplied.  Valid values range from 4 to 72.";
      case -1074395159:
        return "NI Vision does not support the tolerance value you supplied.  Valid values are non-negative.";
      case -1074395158:
        return "NI Vision does not support the ContrastMode value you supplied.";
      case -1074395156:
        return "The OCR attempted to detected the text skew and failed.";
      case -1074395155:
        return "The OCR attempted to detected the text orientation and failed.";
      case -1074395153:
        return "Invalid font file format.";
      case -1074395152:
        return "Font file not found.";
      case -1074395151:
        return "The OCR engine failed during the correction stage.";
      case -1074395150:
        return "NI Vision does not support the RoundingMode value you supplied.";
      case -1074395149:
        return "Found a duplicate transform type in the properties array. Each properties array may only contain one behavior for each transform type.";
      case -1074395148:
        return "Overlay Group Not Found.";
      case -1074395147:
        return "The barcode is not a valid RSS Limited symbol";
      case -1074395146:
        return "Couldn't determine the correct version of the QR code.";
      case -1074395145:
        return "Invalid read of the QR code.";
      case -1074395144:
        return "The barcode that was read contains invalid parameters.";
      case -1074395143:
        return "The data stream that was demodulated could not be read because the mode was not detected.";
      case -1074395142:
        return "Couldn't determine the correct model of the QR code.";
      case -1074395141:
        return "The OCR engine could not find any text in the supplied region.";
      case -1074395140:
        return "One of the character reports is no longer usable by the system.";
      case -1074395139:
        return "Invalid Dimensions.";
      case -1074395138:
        return "The OCR region provided was too small to have contained any characters.";
      default:
        return "Error code not found";
    }
  }
}
