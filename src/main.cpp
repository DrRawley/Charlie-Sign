#include <FastLED.h>

#define DATA_PIN 4 // LED data pin
#define SELECT_BUTTON_PIN 0
#define POT_PIN 1 // THIS IS P2, P2 is analog input 1, so when you are using analog read, you refer to it as 1.

// How many leds are in the strip?
#define NUM_LEDS 9

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

bool lastButtonState = HIGH;        // the previous reading from the input pin
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 200;   // the debounce time; increase if the output flickers
byte selection = 0;

byte numberSelections = 2;
void solid(byte hue);
void myRainbow(void);

void setup()
{
  delay(500); // sanity check delay - allows reprogramming if accidently blowing power w/leds
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // GRB ordering is assumed
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
    myRainbow();
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

void myRainbow(void)
{
  static byte offset = 0;
  static unsigned long timer = millis();

  if (millis() - timer > 100)
  {
    for (byte i = 0; i < NUM_LEDS; i++)
    {
      leds[i].setHSV((i + offset + 15) % 255, 0xff, 128);
    }
    FastLED.show();
    offset++;
    timer = millis();
  }
}