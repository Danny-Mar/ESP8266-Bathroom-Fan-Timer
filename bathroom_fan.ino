#include "ESP8266TimerInterrupt.h"
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include "OTA.h"

#define FAN_SW 3
#define FAN 0

#define TIMER_INTERVAL_MS	1 
#define SERIAL_DEBUGGING_ON true
#define fanRunTimeMinutes 20

const unsigned long eventInterval = 3000;
unsigned long previousTime = 0;
int fanRunTime = 0;
int count = 0;
bool fanSwState = false;
bool one_millisec_passed = false;
bool WifiEnabled;

ESP8266Timer ITimer;

void ICACHE_RAM_ATTR TimerHandler(void)
{
  one_millisec_passed = true;
}

 
void setup() 
{
  if(SERIAL_DEBUGGING_ON)
  {
  Serial.begin(115200);
  }
   pinMode(FAN,OUTPUT);
  digitalWrite(FAN, LOW);
  
  pinMode(FAN_SW,INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  wm.setConfigPortalTimeout(120); // auto close configportal after n seconds

  bool response;
  response = wm.autoConnect("AutoConnectAP"); // anonymous ap
  if(!response) {
      Serial.println("Failed to connect");
      WifiEnabled = false;
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("Lets Go");
      WifiEnabled = true;
  }
  
  SetupOTA();
  
  // Interval in microsecs
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
  serialPrint("Starting  ITimer OK");
  }
  else
  {
  serialPrint("Can't set ITimer correctly. Select another freq. or interval");
  }
  
  serialPrint("setup complete");
}
 
void loop() 
{

  if(WifiEnabled)
  {
     ArduinoOTA.handle();
  }
  
	unsigned long currentTime = millis();

	if (currentTime - previousTime >= eventInterval) 
	{

		if (fanRunTime > 0)
		{
      fanRunTime -= eventInterval;
      char tempStr[40];
      sprintf(tempStr, "Run time remaining = %d minutes", ((fanRunTime/1000)/60));
      serialPrint(tempStr);
		}
    else
    {
      serialPrint("finished running fan");
    }

		count = 0;
		previousTime = currentTime;
	}

  if (count >= 3)
  {
    fanRunTime = (fanRunTimeMinutes*60*1000) + (((count-3)*10)*60*1000);
    char tempStr[40];
    sprintf(tempStr, "Run fan for %d minutes", ((fanRunTime/1000)/60));
    serialPrint(tempStr);
  }
	
	
	if (fanSwState == HIGH)
	{
		digitalWrite(FAN,LOW);
	}
	else if (fanRunTime > 0)
	{
		digitalWrite(FAN,LOW);
	}
	else
	{
		digitalWrite(FAN,HIGH);
	}
	


	if(one_millisec_passed)
	{
	static int fanSwDebounce = 0;
  static bool lastfanSwState;

	if (digitalRead(FAN_SW))
	{
  fanSwDebounce--;
  if(fanSwDebounce <= 0)
  {
    fanSwState = LOW;
    fanSwDebounce = 0;
    lastfanSwState = LOW;
  }
	}
	else
	{
  fanSwDebounce++;
  if( fanSwDebounce >= 50)
  {
    if (lastfanSwState == LOW)
    {
      count++;
      char tempStr[20];
      sprintf(tempStr, "switch toggled %d times", count);
      serialPrint(tempStr);
      previousTime = currentTime;  
    }
    fanSwState = HIGH;
    fanSwDebounce = 50;
    lastfanSwState = HIGH;
    fanRunTime = 0;
  }
	}
	one_millisec_passed = false;
	}
	
}


void serialPrint(const char * message)
{
  static char LastMessage[40];  
  char tempMessage[40];
  if(SERIAL_DEBUGGING_ON)
  {
  strcpy(tempMessage,message);

  if(strcmp(LastMessage,tempMessage)!=0)  
  {
    Serial.println(message);
  }
  
  strcpy(LastMessage, tempMessage);
  }

}
