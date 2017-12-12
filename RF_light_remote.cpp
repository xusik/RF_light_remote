// 	                               +-\/-+
// 	                        VCC   1|    |20  GND
// 	                        P1.0  2|    |19  XIN
// 	                        P1.1  3|    |18  XOUT
// 	                        P1.2  4|    |17  TEST
// 	                 BUTTON P1.3  5|    |16  RST#
// 	                        P1.4  6|    |15  P1.7 MOSI  6
// 	                 5  SCK P1.5  7|    |14  P1.6 MISO  7
// 	                 3   CE P2.0  8|    |13  P2.5
// 	   	 	 4  CSN P2.1  9|    |12  P2.4
//		 	 8  IRQ P2.2 10|    |11  P2.3
// 			               +----+
//
//  tb/tgb1{"s":"1"}
//  tb/tgb1{"s":"0"}
//
//

#include "Enrf24.h"
#include "nRF24L01.h"
#include <string.h>
#include <SPI.h>
#include "Energia.h"

#define _pinPushButton  PUSH2

Enrf24 radio(P2_0, P2_1, P2_2);  // P2.0=CE, P2.1=CSN, P2.2=IRQ
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };

volatile int push_flag = LOW;

const char *str_change = "CHANGE";
int button_now_pressed, msp_ps = 0;
unsigned long work_time = 0;

void power_savings ();
void ps_out();

void setup() {

  pinMode(PUSH2, INPUT_PULLUP); //Enable internal pullup
  attachInterrupt(PUSH2, ps_out, FALLING);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel (120);
  radio.setTXaddress((void*)txaddr);
}

void loop() {

	if (push_flag) {

		work_time = millis();

		button_now_pressed = !digitalRead(PUSH2);

		delay(10);

		if (button_now_pressed + !digitalRead(PUSH2) == 2){
			delay(100);
			if (button_now_pressed + !digitalRead(PUSH2) == 2){
				  radio.print(str_change);
				  radio.flush();  //
			}
		}

		push_flag = LOW;
		button_now_pressed = 0;
	}

	msp_ps =  millis() - work_time;
	if (msp_ps > 500){ power_savings (); }
}

void ps_out(){
	push_flag = HIGH;
	if (stay_asleep == true){
		wakeup();
		SPI.begin();
		SPI.setDataMode(SPI_MODE0);
		SPI.setBitOrder(MSBFIRST);
		radio.begin();  // Defaults 1Mbps, channel 0, max TX power
		radio.setChannel (120);
		radio.setTXaddress((void*)txaddr);
	}
}

void power_savings (){
	attachInterrupt(PUSH2, ps_out, FALLING);
	radio.deepsleep();
	suspend();
}



