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

SPIBus myBus(&hspi1);
APA102c apa102c(myBus.createMasterDevice());
const int NUM_LEDS = 9;
cmdc0de::RGB ledBuf[NUM_LEDS] = {RGB::BLUE};

void initApp() {
  for(int i=0;i<NUM_LEDS;i++) {
	ledBuf[i] = cmdc0de::RGB::GREEN;
  }
  apa102c.init(NUM_LEDS,&ledBuf[0]);
}

int8_t percent =100;
uint16_t count = 0;
bool up = false;

void run() {
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
}
