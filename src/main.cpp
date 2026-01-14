#include <FastLED.h>
#include <EEPROM.h>

#define DATA_PIN 4 // LED data pin
#define SELECT_BUTTON_PIN 0
#define POT_PIN 1 // THIS IS P2, P2 is analog input 1, so when you are using analog read, you refer to it as 1.

// How many leds are in the strip?
#define NUM_LEDS 9

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

bool lastButtonState = HIGH;        // the previous reading from the input pin
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 200;  // the debounce time; increase if the output flickers
byte selection = 0;

byte numberSelections = 5;
void solid(byte hue);
void myRainbow(byte potValue);
void randomLights(byte potValue);
void chase(int hue);
void yellowToWhite(byte potValue);

void setup()
{
  delay(500); // sanity check delay - allows reprogramming if accidently blowing power w/leds
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(2));

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // GRB ordering is assumed

  selection = EEPROM.read(0);
  if (!(selection < numberSelections))
  {
    selection = 0;
  }
}

void loop()
{
  static byte potValue = 0;
  potValue = (byte)map(analogRead(POT_PIN), 0, 1024, 0, 0xff);

  bool currentButtonState = digitalRead(SELECT_BUTTON_PIN);
  if (currentButtonState == LOW)
  {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime) > debounceDelay)
    {
      selection++;
      if (!(selection < numberSelections))
      {
        selection = 0;
      }
      else
      {
        EEPROM.write(0, selection);
      }
      lastDebounceTime = currentTime;
    }
  }

  lastButtonState = currentButtonState;

  switch (selection)
  {
  case 0:
    solid(potValue);
    break;
  case 1:
    myRainbow(potValue);
    break;
  case 2:
    randomLights(potValue);
    break;
  case 3:
    chase(potValue);
    break;
  case 4:
    yellowToWhite(potValue);
    break;
  default:
    break;
  }
}

void solid(byte hue)
{
  for (byte dot = 0; dot < NUM_LEDS; dot++)
  {
    leds[dot].setHSV(hue, 0xff, 128);
  }
  FastLED.show();
}

void myRainbow(byte potValue)
{
  static byte offset = 0;
  static unsigned long timer = millis();
  int delay = map(potValue, 0, 255, 20, 2000);
  if (millis() - timer > delay)
  {
    for (byte i = 0; i < NUM_LEDS; i++)
    {
      leds[i].setHSV((i * 15 + offset) % 255, 0xff, 128);
    }
    FastLED.show();
    offset++;
    timer = millis();
  }
}

void randomLights(byte potValue)
{
  static bool firstTime = true;
  static unsigned long timer = millis();
  int delay = map(potValue, 0, 255, 100, 5000);
  if ((millis() - timer > delay) || firstTime)
  {
    firstTime = false;
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
      byte hue = (byte)random(256);

      leds[dot] = CHSV(hue, 0xff, 128);
    }
    FastLED.show();
    timer = millis();
  }
}

void chase(int hue)
{
  static int offset = 0;
  static unsigned long timer = millis();
  const int dither[] = {160, 100, 50, 25};

  if (millis() - timer > 200)
  {
    for (byte i = 0; i < NUM_LEDS; i++)
    {
      leds[i].setHSV(hue, 0xff, dither[(i + offset) % 4]);
    }
    FastLED.show();
    offset++;
    timer = millis();
  }
}

void yellowToWhite(byte potValue)
{
  byte saturation = (byte)map(potValue, 0, 255, 255, 32);
  byte brightness = (byte)map(potValue, 0, 255, 40, 160);
  
  for (byte dot = 0; dot < NUM_LEDS; dot++)
  {
    leds[dot].setHSV(38, saturation, brightness);
  }
  FastLED.show();
}