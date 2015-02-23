#include <TimerOne.h>
#include "LPD6803.h"

#define MIN_CV_LEVEL 75
#define MAX_CV_LEVEL 1000
#define LED_RANGE 31
#define WHEEL_RANGE 127

//Initial cut to talk pull CV values from the CV output from the theremini and blink lights
//
//Hacked on the example which came with the LPD6803 library - during Hack The Festival 2015


/*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
int dataPin = 2;       // 'yellow' wire
int clockPin = 3;      // 'green' wire
int cvPin = A0;
int cvValue = 0;
byte ledLevel = 0;


// Don't forget to connect 'blue' to ground and 'red' to +5V

// Timer 1 is also used by the strip to send pixel clocks

// Set the first variable to the NUMBER of pixels. 20 = 20 pixels in a row
LPD6803 strip = LPD6803(50, dataPin, clockPin);


void setup() {

 Serial.begin(9600);
     
  // The Arduino needs to clock out the data to the pixels
  // this happens in interrupt timer 1, we can change how often
  // to call the interrupt. setting CPUmax to 100 will take nearly all all the
  // time to do the pixel updates and a nicer/faster display, 
  // especially with strands of over 100 dots.
  // (Note that the max is 'pessimistic', its probably 10% or 20% less in reality)
  
  strip.setCPUmax(100);  // start with 50% CPU usage. up this if the strand flickers or is slow
  
  // Start up the LED counter
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}


void loop() {
 
  cvValue = analogRead(cvPin);
  
 ledLevel = map(cvValue, 0, 1023, 0, WHEEL_RANGE); 

 
 int bar_length = 7;
 int bar_level = map(cvValue,0,MAX_CV_LEVEL, 0,bar_length);
 uint8_t pixelCVRange = (MAX_CV_LEVEL / bar_length);
 uint8_t residualcv = cvValue-pixelCVRange*bar_level;
 uint8_t rcv = map(residualcv,0,pixelCVRange,0,31);
 Serial.println(rcv);
 
  if (cvValue <= MIN_CV_LEVEL)
  {
    cvValue =0;
  }
  
  if (cvValue==0)
  {
    bar_level =0;
  }
  
  
  //Serial.println(bar_level);
  strip.clearPixels();
  // SetBar(Color(31, 31, 0), 0, bar_length,bar_level,Color(rcv,rcv,0));
  SetBar(Color(31, 31, 31), 0, bar_length,bar_level,Color(rcv,rcv,rcv));
  SetBar(Color(31, 0, 0), 7, bar_length,bar_level,Color(rcv,0,0));
  SetBar(Color(0, 31, 0), 14, bar_length,bar_level,Color(0,rcv,0));
  SetBar(Color(0, 0, 31), 21, bar_length,bar_level,Color(0,0,rcv));
  SetBar(Color(31, 31, 0), 28, bar_length,bar_level,Color(rcv,rcv,0));
  SetBar(Color(0, 31, 31), 35, bar_length,bar_level,Color(0,rcv,rcv));
  SetBar(Color(31, 0, 31), 42, bar_length,bar_level,Color(rcv,0,rcv));

  
  strip.show();
  delay(10); 
  

  //ledLevel=map(cvValue,0,1024,0,LED_RANGE);
  
}


/* Helper functions */

// Create a 15 bit color value from R,G,B
unsigned int Color(byte r, byte g, byte b)
{  
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
}



// Bar takes a start and end pixel, and updates it ala' a bar graph/old school LED VU meter.
// 
void SetBar(uint16_t barColour, uint16_t startPixel, uint16_t length, uint16_t level, uint16_t lastColour)
{  
   int i, j;
   
   //Bail out if we're going to overflow the strip.
   if (startPixel+length-1 > strip.numPixels() )
   {
     return;
   }

   //clip the length if it's too long
   if (level > length)
   {
     level = length;
   }
   
  //set the coloured pixels on      
  for (i=startPixel; i <= startPixel+level; i++) 
  {
    strip.setPixelColor(i,barColour);
  }  
  
  //set the dark pixels off
   for (i=startPixel+level; i < startPixel+length; i++)
  {
    strip.setPixelColor(i,Color(0,0,0));
  }
  
  //set the last pixel to lastColour
  strip.setPixelColor(startPixel+level,lastColour);
 
    
}

    
    
