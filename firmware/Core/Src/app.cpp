/*
 * app.cpp
 *
 *  Created on: Apr 5, 2020
 *      Author: dcomes
 */

#include "lib/libstm32/config.h"
#include "app.h"
#include "lib/libstm32/spibus.h"
#include "lib/libstm32/spidevice.h"
#include "lib/libstm32/leds/apa102c.h"
#include "lib/libstm32/rgbcolor.h"
#include "main.h"
#include "spi.h"

using namespace cmdc0de;

uint32_t MIN_TIME_IN_MODE = 10000;

SPIBus myBus(&hspi1);
APA102c apa102c(myBus.createMasterDevice());
const int NUM_LEDS = 20;
cmdc0de::RGB ledBuf[NUM_LEDS] = {RGB::BLUE};


class LedDance {
public:
	LedDance() : StartTime(0), EndTime(0) {}
	ErrorType init() {
		StartTime = HAL_GetTick();
		return onInit();
	}
	ErrorType run(cmdc0de::RGB leds[NUM_LEDS]) {
		return onRun(leds);
	}
	bool isDanceOver(uint32_t time) {return time>EndTime;}
	virtual ~LedDance() {}
protected:
	virtual ErrorType onInit()=0;
	virtual ErrorType onRun(cmdc0de::RGB leds[NUM_LEDS]) = 0;
	void setEndTime(uint32_t et) {EndTime = et;}
	void setTimeInMode(uint32_t max) {EndTime = StartTime+max;}
private:
	uint32_t StartTime;
	uint32_t EndTime;
};

class LedRandom : public LedDance {
public:
	LedRandom(uint32_t maxTime) : LedDance(), MaxTimeInMode(maxTime) {}
	virtual ~LedRandom() {}
protected:
	ErrorType onInit() override {
		setTimeInMode(MaxTimeInMode);
		return ErrorType();
	}
	ErrorType onRun(cmdc0de::RGB leds[NUM_LEDS]) override {
		for(int i=0;i<NUM_LEDS;++i) {
			leds[i].setBrightness(rand()%31);
			leds[i].setRed(rand()%255);
			leds[i].setBlue(rand()%255);
			leds[i].setBlue(rand()%255);
		}
		return ErrorType();
	}
private:
	uint32_t MaxTimeInMode;
};
LedRandom TheLedRandom(10000);
static const uint8_t TOTAL_MODES = 1;
LedDance *DanceArray[TOTAL_MODES];
bool up = false;
uint8_t Mode = 0;


void initApp() {
  for(int i=0;i<NUM_LEDS;i++) {
	ledBuf[i] = cmdc0de::RGB::GREEN;
  }
  apa102c.init(NUM_LEDS,&ledBuf[0]);
  DanceArray[0] = &TheLedRandom;
}

int8_t percent =100;
uint16_t count = 0;

void run() {
//	uint32_t timeNow = HAL_GetTick();
#if 1
	HAL_Delay(10);
	if(count==0) {
		apa102c.setBrightness(up ? percent++ : percent--);
		apa102c.send();
		if(percent<1) {
			up = true;
		} else if (percent>99) {
			up = false;
		}
	}
	++count;
	if(10000==count) count = 0;
#else
	if(DanceArray[Mode]->isDanceOver(timeNow)==true) {
		Mode = ++Mode%TOTAL_MODES;
		DanceArray[Mode]->init();
	}
	DanceArray[Mode]->run(ledBuf);
	apa102c.send();


#endif
}
