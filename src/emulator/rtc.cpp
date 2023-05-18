// **************************************************
// **************************************************
// **************************************************
// Real Time Clock (RTC) (Used by MBC3)
// **************************************************
// **************************************************
// **************************************************

#include "rtc.h"

#include <QFile>
#include <chrono>
#include <fstream>
#include <thread>

#include "cgb.h"

RTC::RTC()
    : cgb(nullptr),
      seconds(0),
      minutes(0),
      hours(0),
      daysLo(0),
      daysHi(0),
      clock(system_clock::now().time_since_epoch().count()),
      latchVal(),
      rtcRegs(),
      rtcReg(rtcRegs[0]) {}

// update rtc registers to reflect
// the current time
void RTC::latch() {
  // get time difference between
  // clock and current timestamp and
  // update clock to equal timestamp
  long long now = system_clock::now().time_since_epoch().count();
  auto timeDiff = now - clock;
  clock = now;

  // get time elapsed since rtc was last saved
  auto daysElapsed = timeDiff / US_PER_DAY;
  timeDiff %= US_PER_DAY;
  auto hoursElapsed = timeDiff / US_PER_HOUR;
  timeDiff %= US_PER_HOUR;
  auto minutesElapsed = timeDiff / US_PER_MINUTE;
  timeDiff %= US_PER_MINUTE;
  auto secondsElapsed = timeDiff / US_PER_SECOND_INT;

  // update seconds rtc register
  uint8 secondsResult = seconds + secondsElapsed;
  seconds = secondsResult % SECONDS_PER_MINUTE;

  // update minutes rtc register
  uint8 minutesResult = minutes + minutesElapsed;
  if (secondsResult >= SECONDS_PER_MINUTE) minutesResult += 1;
  minutes = minutesResult % MINUTES_PER_HOUR;

  // update hours rtc register
  uint8 hoursResult = hours + hoursElapsed;
  if (minutesElapsed >= MINUTES_PER_HOUR) hoursResult += 1;
  hours = hoursResult % HOURS_PER_DAY;

  // update days rtc register
  uint16 daysResult = ((daysHi & BIT0_MASK) << 8 | daysLo) + daysElapsed;
  if (hoursElapsed >= HOURS_PER_DAY) daysResult += 1;
  daysLo = daysResult;
  daysHi &= BIT0_MASK;
  daysHi |= (daysResult >> 8) & BIT0_MASK;
  if (daysElapsed >= MAX_DAYS) daysHi |= BIT7_MASK;
}

void RTC::resetClock() {
  clock = system_clock::now().time_since_epoch().count();
}

bool RTC::halted() { return daysHi & BIT6_MASK; }

void RTC::load() {
  auto path = cgb->romPath.replace(".gbc", ".rtc");
  path = path.replace(".gb", ".rtc");
  QFile rtcFile(path);

  // set rtc registers and clock
  seconds = stoi(rtcFile.readLine().toStdString());
  minutes = stoi(rtcFile.readLine().toStdString());
  hours = stoi(rtcFile.readLine().toStdString());
  daysLo = stoi(rtcFile.readLine().toStdString());
  daysHi = stoi(rtcFile.readLine().toStdString());
  clock = stoll(rtcFile.readLine().toStdString());

  // clock is saved as 0 if rtc was halted
  if (clock == 0) resetClock();
}

void RTC::save() {
  auto path = cgb->romPath.replace(".gbc", ".rtc");
  path = path.replace(".gb", ".rtc");
  QFile rtcFile(path);

  // save rtc registers and current
  // system timestamp
  rtcFile.write((to_string(seconds) + "\n").c_str());
  rtcFile.write((to_string(minutes) + "\n").c_str());
  rtcFile.write((to_string(hours) + "\n").c_str());
  rtcFile.write((to_string(daysLo) + "\n").c_str());
  rtcFile.write((to_string(daysHi) + "\n").c_str());
  rtcFile.write(
      to_string(halted() ? 0 : system_clock::now().time_since_epoch().count())
          .c_str());
}
