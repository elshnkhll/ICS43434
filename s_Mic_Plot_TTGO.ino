/*  Adafruit I2S MEMS Microphone Breakout - SPH0645LM4H PID: 3421 */
/*  https://www.adafruit.com/product/3421 */

#include <WiFi.h>
#include "driver/i2s.h"
#include "I2S_MEMS_Mic.h"


void setup()
{
  Serial.begin(115200);
  //Turn off WIFI to save power
  WiFi.mode(WIFI_OFF);

  Mic_Init();
  delay(1000);
}

void loop(){

  int32_t a = Mic_Get_Sample();
  Serial.println( a, DEC  );
  yield();
  
}
