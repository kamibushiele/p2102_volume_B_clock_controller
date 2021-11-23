#ifndef _RTC_H_
#define _RTC_H_

#include "common.h"
#include <ezTime.h>
#include "settings.h"


extern Timezone myTZ;
extern bool manual_time;
extern bool standalone_mode;

void rtcInit(bool offline);
void rtcInLoop(char dot);

#endif