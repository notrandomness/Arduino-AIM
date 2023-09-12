/// @file    arduinoLEDTest.ino
/// @brief   Uses @ref ColorPalettes to control LED strips and plays an animation on the built-in lED matrix. Also connects to wifi to change LED modes.
/// @example ColorPalette.ino

#include <FastLED.h>
#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include "FireWall.h"
#include "Waves.h"

#define SECRET_SSID "Arcade"
#define SECRET_PASS "Brandon is awesome"

#define LED_PIN     5
#define NUM_LEDS    900
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.



CRGBPalette16 currentPalette;
TBlendType    currentBlending;
ArduinoLEDMatrix matrix;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status
WiFiServer server(80);
bool modeSet = false;
bool topDirection = true; //true is right to left
bool midDirection = true;
bool botDirection = true;
uint8_t patternSize = 1; // low is large
double speed = 1;
double acceleration = 0.5;
int loopCount = 0;
int nonPaletteMode = 0;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
uint8_t frame[8][12] = {
  {0,1,1,1,1,0,0,0,0,0,0,0},
  {1,1,0,0,1,1,0,0,0,0,0,0},
  {1,0,0,0,0,1,0,0,0,0,0,0},
  {1,0,0,0,0,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,0,0,0,0,1},
  {0,0,0,0,0,0,1,0,0,0,0,1},
  {0,0,0,0,0,0,1,1,0,0,1,1},
  {0,0,0,0,0,0,0,1,1,1,1,0}

};

uint8_t frames[12][8][12];

void setup() {
    delay( 3000 ); // power-up safety delay
    Serial.begin(9600);
    randomSeed(analogRead(0));
    matrix.begin();
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    memcpy (frames[0], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[1], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[2], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[3], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[4], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[5], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[6], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[7], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[8], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[9], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[10], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    memcpy (frames[11], frame, 8*12*sizeof(uint8_t));
    iterateScrollingFrame(frame);

    connectToWifi();
}

void connectToWifi()
{
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Access Point Web Server initializing...");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    return;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  //WiFi.config(IPAddress(192,168,100,1));

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

String getParameter(WiFiClient& client)
{
  String param = "";
  char c = client.read();
  while( c != '?' && c != '\n')
  {
    param += c;
    c = client.read();
  }
  param.trim();
  return param;
}

void serverLoop()
{
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status != WL_CONNECTED) 
    {
      // arduino disconnected from wifi
      Serial.println("Device disconnected from wifi. Attempting to Reconnect...");
      connectToWifi();
    }
  }
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      //delayMicroseconds(10);                // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out to the serial monitor

        // Check to see if the client request was "GET /mode=" followed by mode name, ending with '?'
        if (c == '=')
        {
          if (currentLine.endsWith("mode"))
          {
            ChangePaletteTo(getParameter(client));
          }
          else if (currentLine.endsWith("speed"))
          {
            speed = getParameter(client).toDouble();
          }
          else if (currentLine.endsWith("acceleration"))
          {
            acceleration = getParameter(client).toDouble();
            loopCount = 0;
          }
          else if (currentLine.endsWith("size"))
          {
            patternSize = getParameter(client).toDouble();
          }
          else if (currentLine.endsWith("td"))
          {
            topDirection = getParameter(client).equals("1");
          }
          else if (currentLine.endsWith("md"))
          {
            midDirection = getParameter(client).equals("1");
          }
          else if (currentLine.endsWith("bd"))
          {
            botDirection = getParameter(client).equals("1");
          }
        }
        
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            //client.print("<p style=\"font-size:7vw;\">Click <a href=\"/H\">here</a> turn the LED on<br></p>");
            //client.print("<p style=\"font-size:7vw;\">Click <a href=\"/L\">here</a> turn the LED off<br></p>");
            client.print("success");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


void loop()
{
    if (!modeSet)
    {
      ChangePalettePeriodically();
    }
    
    /* motion speed */
    static uint8_t startIndexTop = 0;
    static uint8_t startIndexMid = 0;
    static uint8_t startIndexBot = 0;

    loopCount++;
    if (acceleration > 0)
    {
      speed = acceleration * loopCount;
    }

    startIndexTop += speed;
    startIndexMid += speed;
    startIndexBot += speed;
    
    if (nonPaletteMode == 0)
    {
      FillLEDsFromPaletteColors( startIndexTop, startIndexMid, startIndexBot);
    }
    else
    {
      FillLEDsNonPalette();
    }
    
    FastLED.show();

    static uint8_t scrollIndex = 0;
    if (scrollIndex > 11)
    {
      scrollIndex = 0;
    }
    matrix.renderBitmap(frames[scrollIndex], 8, 12);
    scrollIndex += 1;

    if (scrollIndex == 1)
    {
      serverLoop();
    }
}


void iterateScrollingFrame(uint8_t frame[][12])
{
    uint8_t i, j, prev, next;
    for(i=0; i<8; i++)
    {
      next = frame[i][11]; //Set next to last value
        for(j=0; j<12; j++)
        {
          prev = frame[i][j]; //Set prev to current value
          frame[i][j] = next; //Set current value to next
          next  = prev;
        }
    }
}

void FillLEDsNonPalette()
{
  if(nonPaletteMode == 1)
  {
    FillFireWall();
  }
  else if (nonPaletteMode == 2)
  {
    FillOceanWaves();
  }
  else if (nonPaletteMode == 3)
  {
    FillTrippyWaves();
  }
}

void FillTrippyWaves()
{
  static Waves trippy = Waves();
  trippy.updateWaves();
  CHSV color;
  for (int i = 0; i < 300; i++)
  {
    color = trippy.getTrippyLEDAtIndex(i);
    leds[i] = color;
    leds[599-i] = color;
    leds[i+600] = color;
  }
}

void FillOceanWaves()
{
  static Waves ocean = Waves();
  ocean.updateWaves();
  for (int i = 0; i < 300; i++)
  {
    leds[i] = ocean.getTopLEDAtIndex(i);
    leds[599-i] = ocean.getMidLEDAtIndex(i);
    leds[i+600] = ocean.getBotLEDAtIndex(i);
  }
}

void FillFireWall()
{
  static FireWall fireWall = FireWall();
  fireWall.updateTemperatures();
  for (int i = 0; i < 300; i++)
  {
    leds[i] = fireWall.getTopLEDAtIndex(i);
    leds[599-i] = fireWall.getMidLEDAtIndex(i);
    leds[i+600] = fireWall.getBotLEDAtIndex(i);
  }
}


void FillLEDsFromPaletteColors( uint8_t colorIndexTop, uint8_t colorIndexMid, uint8_t colorIndexBot)
{
    uint8_t brightness = 255;
    
    // for( int i = 0; i < NUM_LEDS; ++i) {
    //     leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    //     colorIndex += 3;
    // }
    if (topDirection && midDirection && botDirection)
    {
      int j = 600;
      int k = 600;
      for( int i = 0; i < 300; i++) 
      {
        leds[i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[--j] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[k++] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
    else if (topDirection && midDirection) 
    {
      int j = 600;
      int k = 900;
      for( int i = 0; i < 300; i++) 
      {
        leds[i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[--j] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[--k] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
    else if (topDirection && botDirection)
    {
      int j = 300;
      int k = 600;
      for( int i = 0; i < 300; i++) 
      {
        leds[i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[j++] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[k++] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
    else if (midDirection && botDirection)
    {
      int j = 600;
      int k = 600;
      for( int i = 300; i > 0;) 
      {
        leds[--i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[--j] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[k++] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
    else if (topDirection)
    {
      int j = 300;
      int k = 900;
      for( int i = 0; i < 300; i++) 
      {
        leds[i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[j++] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[--k] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
    else if (midDirection)
    {
      int j = 600;
      int k = 900;
      for( int i = 300; i > 0;) 
      {
        leds[--i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[--j] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[--k] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
    else if (botDirection)
    {
      int j = 300;
      int k = 600;
      for( int i = 300; i > 0;) 
      {
        leds[--i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[j++] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[k++] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
    else
    {
      int j = 300;
      int k = 900;
      for( int i = 300; i > 0;) 
      {
        leds[--i] = ColorFromPalette( currentPalette, colorIndexTop, brightness, currentBlending);
        //colorIndex += 3
        leds[j++] = ColorFromPalette( currentPalette, colorIndexMid, brightness, currentBlending);
        //colorIndex += 3
        leds[--k] = ColorFromPalette( currentPalette, colorIndexBot, brightness, currentBlending);
        colorIndexTop += patternSize;
        colorIndexMid += patternSize;
        colorIndexBot += patternSize;
      }
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{


    uint8_t seconds = (millis() / 1000);
    static uint8_t changedAt = 200;
    uint8_t changeAfter = 60; //changes every 60 seconds
    static uint8_t option = 17;
    uint8_t timeElapsed = (seconds - changedAt);
    if (timeElapsed < 0 )
    {
      changedAt = seconds;
    }

    if (changeAfter <= timeElapsed) //true if changeAfter seconds have passed since last change (time to change)
    {
      
      changedAt = seconds;
      loopCount = 0;
      //change lights
      //set new speed, acceleration and patternSize
      speed = random(1,11);
      if (random(2) == 1)
      {
        acceleration = random(51)/100.0;
      }
      else
      {
        acceleration = 0;
      }
      patternSize = random(1, 9);
      bool topAndBotDirection = random(2);
      midDirection = random(2);
      topDirection = topAndBotDirection;
      botDirection = topAndBotDirection;
      nonPaletteMode = 0; //use palettes unless otherwise specified

      //change palette
      if( option ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
      else if( option == 1)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
      else if( option == 2)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
      else if( option == 3)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
      else if( option == 4)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
      else if( option == 5)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
      else if( option == 6)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
      else if( option == 7)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
      else if( option == 8)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
      else if( option == 9)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
      else if( option == 10)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
      else if( option == 11)  { currentPalette = LavaColors_p;           currentBlending = LINEARBLEND; }
      else if( option == 12)  { currentPalette = ForestColors_p;           currentBlending = LINEARBLEND; }
      else if( option == 13)  { currentPalette = OceanColors_p;           currentBlending = LINEARBLEND; }
      else if( option == 14)  { SetupBlackAndRandomStripedPalette();       currentBlending = NOBLEND; }
      else if( option == 15)  { nonPaletteMode = 1; }
      else if( option == 16)  { nonPaletteMode = 2; }
      else if( option == 17)  { nonPaletteMode = 3; }
      option++;
      option%=18;
    }
}

void ChangePaletteTo(String mode)
{
    Serial.println("Changing LED mode to: " + mode);
    modeSet = true;
    nonPaletteMode = 0;
    if( mode == "rainbow")  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
    else if( mode == "rainbow+stripe")  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
    else if( mode == "rainbow+stripe+blend")  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
    else if( mode == "purple+green")  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
    else if( mode == "random")  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
    else if( mode == "black+and+white")  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
    else if( mode == "black+white+blend")  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
    else if( mode == "clouds")  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
    else if( mode == "party")  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
    else if( mode == "america")  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
    else if( mode == "america+blend")  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    else if( mode == "black+and+random")  { SetupBlackAndRandomStripedPalette();       currentBlending = NOBLEND; }
    else if( mode == "lava")  { currentPalette = LavaColors_p;           currentBlending = LINEARBLEND; }
    else if( mode == "forest")  { currentPalette = ForestColors_p;           currentBlending = LINEARBLEND; }
    else if( mode == "ocean")  { currentPalette = OceanColors_p;           currentBlending = LINEARBLEND; }
    else if( mode =="firewall") { nonPaletteMode = 1;}
    else if( mode =="oceanwaves") { nonPaletteMode = 2;}
    else if( mode =="trippywaves") { nonPaletteMode = 3;}
    else { modeSet = false;}
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, 255);//random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

void SetupBlackAndRandomStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // uint8_t red = random(2)? 255 : 0;
    // uint8_t green = random(2)? 255 : 0;
    // uint8_t blue = random(2)? 255 : 0;
    CRGB randomColor  = CHSV( random8(), 255, 255);
    // and set every fourth one to white.
    currentPalette[0] = randomColor;
    currentPalette[4] = randomColor;
    currentPalette[8] = randomColor;
    currentPalette[12] = randomColor;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};



// Additional notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
