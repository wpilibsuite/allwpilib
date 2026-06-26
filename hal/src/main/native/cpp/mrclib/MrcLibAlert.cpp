#include "wpi/util/Alert.h"

#include "mrclib/Alert.h"
#include "mrclib/MrcString.hpp"

static_assert(WPI_ALERT_HIGH == MRC_ALERT_HIGH);
static_assert(WPI_ALERT_MEDIUM == MRC_ALERT_MEDIUM);
static_assert(WPI_ALERT_LOW == MRC_ALERT_LOW);

static MRC_String WPIStringToMRCString(const struct WPI_String* wpiStr) {
  MRC_String mrcStr;
  if (wpiStr) {
    mrcStr.str = wpiStr->str;
    mrcStr.len = wpiStr->len;
  } else {
    mrcStr.str = nullptr;
    mrcStr.len = 0;
  }
  return mrcStr;
}


static int32_t MrcLibCreateAlert(const struct WPI_String* group, const struct WPI_String* id,
                              const struct WPI_String* text, int32_t level,
                              WPI_AlertHandle* handle) {
  MRC_String mrcGroup = WPIStringToMRCString(group);
  MRC_String mrcId = WPIStringToMRCString(id);
  MRC_String mrcText = WPIStringToMRCString(text);
  return MRC_Alert_CreateAlert(&mrcGroup, &mrcId, &mrcText, level, handle);
}
