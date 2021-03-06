#include <Arduino.h>
#include <SD.h>
#include "Dispositif.h"

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif 

Dispositif::Dispositif()
{
  pin1 = 5;
  pin2 = 6;
  pin3 = 9;
  
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  
  ledOn = false;
  orderZone = true;
  zoneMatch = false;

  //if(!SD.begin(4));
    // TO DO : report to website that SD card is malfunctioning
  
  // Read config file, to configure Driver
  File file;
  file = SD.open("config.txt", FILE_READ);
  
  String line = "";
  line.reserve(255);
  
  // Zone of the LED Driver
  char c = file.read();
  while(c != '\n')
  {
    line += c;
    c = file.read();
  }
  zone = line;//.toInt();
  
  // ID of the LED Driver
  line = "";
  c = file.read();
  while(c != '\n')
  {
    line += c;
    c = file.read();
  }
  id = line;//.toInt();
  
  // ID of the Arduino in the Xbee tree
  line = "";
  c = file.read();
  while(c != '\n')
  {
    line += c;
    c = file.read();
  }
  idNode = line;//.toInt();            	
  
  file.close();
}

Dispositif::Dispositif(String zone, String id, String idNode)
{
  pin1 = 5;
  pin2 = 6;
  pin3 = 9;
  
  zone = zone;
  id = id;
  idNode = idNode;
  
  ledOn = false;
  orderZone = true;
  zoneMatch = false;
  
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);

  //if(!SD.begin());
    // TO DO : report to website that SD card is malfunctioning
  
  if(SD.exists("config.txt"))
    SD.remove("config.txt");
  
  File file;
  file = SD.open("config.txt", FILE_WRITE);
  
  file.println(zone);
  file.println(id);
  file.println(idNode);
  
  file.close();
}

void Dispositif::Action(String action)
{
  if(action == "jour")             // Turn the light on
  {
    digitalWrite(pin1, HIGH);
    ledOn = true;
  }
  else if(action == "nuit")        // Turn the light off
  {
    digitalWrite(pin1, LOW);
    ledOn = false;
  }
  else if(action == "harlem")      // Harlem Shake
  {
    uint8_t i = 3;
    uint8_t j = 10;
    while(i > 0)
    {
        while(j > 0)
        {
            digitalWrite(pin1, HIGH);
            delay(j * 8);
            digitalWrite(pin1, LOW);
            delay(j * 8);
            j -= 1;
        }
        while(j < 10)
        {
            digitalWrite(pin1, HIGH);
            delay(j * 8);
            digitalWrite(pin1, LOW);
            delay(j * 8);
            j += 1;
        }

        i -= 1;
    }
  }
  else if(action == "blink")	// LED blinks at the speed of page refresh
  {
     if(ledOn)
       digitalWrite(pin1, LOW);
     else
       digitalWrite(pin1, HIGH);
	   
	 ledOn = !ledOn;
  }
}

void Dispositif::setBrightness(uint8_t number, uint8_t value)
{
    //uint8_t pin = 5;
    if (1 == number) // pin 6
    {
      sbi(TCCR0A, COM0A1);
      OCR0A = value; // set pwm duty
    }
    else if (2 == number) // pin 9
    {
      sbi(TCCR1A, COM1A1);
      OCR1A = value; // set pwm duty
    }
    else // pin 5
    {
      sbi(TCCR0A, COM0B1);
      OCR0B = value; // set pwm duty
    }
}

void Dispositif::parseMessage(String code)
{
  if(orderZone)
  {
    //Serial.println(code);
    //int temp = code.toInt();
    // we check we are in the good zone, if it's true, the next part can be read
    if(code == zone)
    {
      zoneMatch = true;
    }
    else
    {
      zoneMatch = false;
    }
    orderZone = false; 
  }
  else
  { 
    orderZone = true;
    // if we are in the right zone, we can update the brightness
    if(zoneMatch)
    {
       //int p = code.toInt();  // Code is a string
       setBrightness(0, code.toInt());
    }
  }
}

/*void Dispositif::digitalWriteC(uint8_t pin, uint8_t val)
{
  uint8_t timer = digitalPinToTimer(pin);
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out;

	out = portOutputRegister(port);

	uint8_t oldSREG = SREG;
	cli();

	if (val == LOW) {
		*out &= ~bit;
	} else {
		*out |= bit;
	}

	SREG = oldSREG;
}*/