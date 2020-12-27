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
	uint32_t getStartTime() {return StartTime;}
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
	LedRandom(uint32_t maxTime, uint32_t timeBetweenChange) : LedDance(), MaxTimeInMode(maxTime), TimeBetweenChange(timeBetweenChange), LastTime(0) {}
	virtual ~LedRandom() {}
protected:
	ErrorType onInit() override {
		setTimeInMode(MaxTimeInMode);
		LastTime = 0;
		return ErrorType();
	}
	ErrorType onRun(cmdc0de::RGB leds[NUM_LEDS]) override {
		if((LastTime/TimeBetweenChange)!=(HAL_GetTick()/TimeBetweenChange)) {
			LastTime = HAL_GetTick();
			for(int i=0;i<NUM_LEDS;++i) {
				leds[i].setBrightness(rand()%100);
				leds[i].setRed(rand()%255);
				leds[i].setBlue(rand()%255);
				leds[i].setBlue(rand()%255);
			}
		}
		return ErrorType();
	}
private:
	uint32_t MaxTimeInMode;
	uint32_t TimeBetweenChange;
	uint32_t LastTime;
};

class LedChase : public LedDance {
public:
	LedChase(uint32_t maxTime, uint32_t timeBetweenChange, bool bFill) : LedDance(), MaxTimeInMode(maxTime), TimeBetweenChange(timeBetweenChange), LastTime(0), Count(0), ChaseColor(), Fill(bFill) {}
	virtual ~LedChase() {}
protected:
	ErrorType onInit() override {
		setTimeInMode(MaxTimeInMode);
		LastTime = 0;
		ChaseColor = RGB(rand()%255,rand()%255,rand()%255,rand()%100);
		return ErrorType();
	}
	ErrorType onRun(cmdc0de::RGB leds[NUM_LEDS]) override {
		if((LastTime/TimeBetweenChange)!=(HAL_GetTick()/TimeBetweenChange)) {
			LastTime = HAL_GetTick();
			uint8_t startPos = Count;
			for(int i=0;i<NUM_LEDS;i++) {
				if((Fill && i< Count) || (!Fill &&i==Count)) {
					leds[i] = ChaseColor;
				} else {
					leds[i].setBrightness(0);
					leds[i].setBlue(0);
					leds[i].setGreen(0);
					leds[i].setRed(0);
				}
			}
			Count = ++Count%NUM_LEDS;
		}
		return ErrorType();
	}
private:
	uint32_t MaxTimeInMode;
	uint32_t TimeBetweenChange;
	uint32_t LastTime;
	uint32_t Count;
	RGB ChaseColor;
	bool Fill;
};

class LedBreath : public LedDance {
public:
	LedBreath(uint32_t maxTime, uint32_t timeBetweenChange, bool bUp, uint8_t incBy)
		: LedDance(), MaxTimeInMode(maxTime), TimeBetweenChange(timeBetweenChange), LastTime(0), ChaseColor(), Up(bUp), IncBy(incBy) {}
	virtual ~LedBreath() {}
protected:
	ErrorType onInit() override {
		setTimeInMode(MaxTimeInMode);
		LastTime = 0;
		ChaseColor = RGB(rand()%255,rand()%255,rand()%255,rand()%100);
		return ErrorType();
	}
	ErrorType onRun(cmdc0de::RGB leds[NUM_LEDS]) override {
		if((LastTime/TimeBetweenChange)!=(HAL_GetTick()/TimeBetweenChange)) {
			LastTime = HAL_GetTick();
			for(int i=0;i<NUM_LEDS;i++) {
				int16_t bright = leds[i].getBrightness();
				leds[i] = ChaseColor;
				bright = Up ? bright+IncBy : bright-IncBy;
				if(bright>=100) {
					Up = false;
					bright=100;
				} else if (bright<=0) {
					Up = true;
					bright=0;
				}
				leds[i].setBrightness(bright);
			}
		}
		return ErrorType();
	}
private:
	uint32_t MaxTimeInMode;
	uint32_t TimeBetweenChange;
	uint32_t LastTime;
	RGB ChaseColor;
	bool Up;
	uint8_t IncBy;
};

static LedRandom TheLedRandom(10000,500);
static LedChase TheLedChase(10000,50, false);
static LedChase TheLedChaseF(10000,50, true);
static LedBreath TheLedBreath(10000,50,true,4);

static const uint8_t TOTAL_MODES = 4;
static LedDance *DanceArray[TOTAL_MODES];
static uint8_t Mode = 0;


void initApp() {
  for(int i=0;i<NUM_LEDS;i++) {
	ledBuf[i] = cmdc0de::RGB::GREEN;
  }
  apa102c.init(NUM_LEDS,&ledBuf[0]);
  DanceArray[0] = &TheLedRandom;
  DanceArray[1] = &TheLedChase;
  DanceArray[2] = &TheLedChaseF;
  DanceArray[3] = &TheLedBreath;
  Mode = rand()%TOTAL_MODES;
}

int8_t percent =100;
//uint16_t count = 0;
bool up = false;
uint32_t lastTime = HAL_GetTick();

void run() {
	uint32_t timeNow = HAL_GetTick();
#if 0
	HAL_Delay(10);
	if((lastTime/30)!=(timeNow/30)) {
		lastTime = timeNow;
		apa102c.setBrightness(up ? percent++ : percent--);
		apa102c.send();
		if(percent<1) {
			up = true;
		} else if (percent>99) {
			up = false;
		}
	}
	//++count;
	//if(10==count) count = 0;
#else
	if(DanceArray[Mode]->isDanceOver(timeNow)==true) {
		Mode = rand()%TOTAL_MODES;
		DanceArray[Mode]->init();
	}
	DanceArray[Mode]->run(ledBuf);
	apa102c.send(NUM_LEDS,&ledBuf[0]);
#endif
}
