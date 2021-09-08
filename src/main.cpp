// Adafruit IO Analog Out Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-analog-output
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.


/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "configlocal.h"
#include "AdafruitIO.h"
#include <Adafruit_NeoPixel.h>
#define PIN 2 // neopixel pin
#define NUMPIXELS 1
#define NEOPIXEL_TYPE NEO_GRB
#define DELAYVAL 100
#define MAXREAD 300
/************************ /Configuration *******************************/

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEOPIXEL_TYPE + NEO_KHZ800);

// set up the 'pm2.5' feed
AdafruitIO_Feed *pm25 = io.feed("particulate-matter.pm-2-5");

// set up the 'pm10' feed
AdafruitIO_Feed *pm10 = io.feed("particulate-matter.pm-10");

// set up the 'pm1' feed
AdafruitIO_Feed *pm1 = io.feed("particulate-matter.pm1");

void handleMessage(AdafruitIO_Data *data);
int colourfrompercent(int percent);

void setup()
{


  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (!Serial)
    ;

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'analog' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  pm25->onMessage(handleMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  pm25->get();

  pixels.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop()
{

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
}

// this function is just converting a provided percentage to a hue value, for HSL
int colourFromReading(int reading)
{
  float h;
  if (reading > MAXREAD)
  {
    reading = MAXREAD;
  }
  h = ((reading / MAXREAD) * 100) * 3.6;
  return h;
}


// this function is called whenever an 'analog' message
// is received from Adafruit IO. it was attached to
// the analog feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data)
{

  // convert the data to integer
  int pm25Read = data->toInt();

  Serial.print("received <- ");
  Serial.println(pm25Read);
  Serial.print("colour 0-360 <- ");
  Serial.println(colourFromReading((int)pm25Read));

  // write the current 'reading' to the led
  // #if defined(ARDUINO_ARCH_ESP32)
  //   ledcWrite(1, reading); // ESP32 analogWrite()
  // #else
  //   analogWrite(LED_PIN, reading);
  // #endif

  pixels.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for (int i = 0; i < NUMPIXELS; i++)
  { // For each pixel...


    uint32_t rgbcolor = pixels.gamma32(pixels.ColorHSV(colourFromReading(pm25Read), 255, 255));

    pixels.setPixelColor(i, rgbcolor);

    /*
    for addressing the leds directly..
    uint32_t rgbcolor = pixels.gamma32(pixels.ColorHSV(colourFromReading(pm10Read, 30), 0.5, 1));

    pixels.setPixelColor(2, rgbcolor);
    */

    pixels.show(); // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }

}