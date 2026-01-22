#include <Arduino.h>
#include <ShiftRegister74HC595.h>
#include <vector>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SPI.h"
#include <Adafruit_ILI9341.h>
#include "ted.h"
#include "nomouth.h"
// #include "textdebug.h"
#include <algorithm>

// <num registers> (data pin, lock pin, latch pin)
const uint8_t LED_ARRAYS = 4;
ShiftRegister74HC595<LED_ARRAYS> sr(18, 20, 19);
uint COLS = 16;
uint ROWS = 2;
uint MAX_CHARS = COLS * ROWS;
LiquidCrystal_I2C lcd(0x27, COLS, ROWS); // set the LCD address to 0x27 for a 16 chars and 2 line display
uint CHAR_LED_PIN = 1;
uint FIG_LED_PIN = 2;
bool FIG = false;
uint START_INDEX = MAX_CHARS;
uint NOMOUTH_OFFSET = START_INDEX;

#define TFT_DC 9
#define TFT_CS 7
#define TFT_MOSI 10
#define TFT_CLK 11
#define TFT_RST 8
#define TFT_MISO 16
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup()
{
  // put your setup code here, to run once:
  // pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output
  lcd.init(); // initialize the lcd
  lcd.backlight();
  // self test
  sr.setAllHigh();
  digitalWrite(FIG_LED_PIN, HIGH);
  digitalWrite(CHAR_LED_PIN, HIGH);
  delay(1000);
  tft.begin();
  tft.setRotation(3);
  tft.drawRGBBitmap(0, 0, (const uint16_t *)ted.pixel_data, ted.width, ted.height);
  sr.setAllLow();
  digitalWrite(FIG_LED_PIN, LOW);
}

uint8_t ita2(char i)
{
  if (i >= 97 && i <= 122)
  {
    i = i - 32;
  }
  if ((i >= 65 && i <= 90) || (i >= 8 && i <= 13) || i == 32)
  {
    FIG = false;
  }
  else
  {
    FIG = true;
  }
  switch (i)
  {
  // we are not using bit 0 or 7,8 so leave these unset
  // bits are also inverted because the display position
  // is opposed of the ordering
  case 'A':
  case '-':
    return 0b00000110;
  case 'B':
  case '?':
    return 0b00110010;
  case 'C':
  case ':':
    return 0b00011100;
  case 'D':
    // Short code for "Who Are You" goes here, skipping as not needed
    return 0b00010010;
  case 'E':
  case '3':
    return 0b00000010;
  case 'F':
  case '%':
    return 0b00011010;
  case 'G':
  case '@':
    return 0b00110100;
  case 'H':
  case '$': // 'merica?
    return 0b00101000;
  case 'I':
  case '8':
    return 0b00001100;
  case 'J':
  case 7: // BELL
    return 0b00010110;
  case 'K':
  case '(':
    return 0b00011110;
  case 'L':
  case ')':
    return 0b00100100;
  case 'M':
  case '.':
    return 0b00111000;
  case 'N':
  case ',':
    return 0b00001100;
  case 'O':
  case '9':
    return 0b00110000;
  case 'P':
  case '0':
    return 0b00101100;
  case 'Q':
  case '1':
    return 0b00101110;
  case 'R':
  case '4':
    return 0b00010100;
  case 'S':
  case '\'':
    return 0b00101000;
  case 'T':
  case '5':
    return 0b00000010;
  case 'U':
  case '7':
    return 0b00111000;
  case 'V':
  case '=':
    return 0b00011110;
  case 'W':
  case '2':
    return 0b00110010;
  case 'X':
  case '/':
    return 0b00101110;
  case 'Y':
  case '6':
    return 0b00101010;
  case 'Z':
  case '+':
    return 0b00100010;
  case '\r':
    return 0b00010000;
  case '\n':
    return 0b00000100;
  case '\t':
  case ' ':
    return 0b00001000;
  default:
    return 0b00000000;
  }
}

void loop()
{
  // clear the screen
  lcd.clear();
  // reset cursor
  lcd.setCursor(0, 0);
  // This should always be true, but check it anyways
  if (NOMOUTH_OFFSET >= LED_ARRAYS)
  {
    uint8_t ledValues[LED_ARRAYS];
    Serial.print("Array Chars: ");
    for (int i = 0; i < LED_ARRAYS; i++)
    {
      char curr = NOMOUTH[NOMOUTH_OFFSET - i];
      ledValues[i] = ita2(NOMOUTH[NOMOUTH_OFFSET - i]);
      Serial.print(curr);
    }
    Serial.println("");
    sr.setAll(ledValues);
    /// debug
    Serial.print("Array: [");
    for (int i = 0; i < 4; i++)
    {
      Serial.print(ledValues[i], BIN);
      if (i < 3)
      {
        Serial.print(", "); // Add comma between elements (not after last)
      }
    }
    Serial.println("]");
    /// debug
  }
  if (FIG)
  {
    digitalWrite(FIG_LED_PIN, HIGH);
    digitalWrite(CHAR_LED_PIN, LOW);
    Serial.println("FIG");
  }
  else
  {
    digitalWrite(FIG_LED_PIN, LOW);
    digitalWrite(CHAR_LED_PIN, HIGH);
  }
  if (NOMOUTH_OFFSET > sizeof(NOMOUTH) - 1)
  {
    NOMOUTH_OFFSET = START_INDEX;
  }
  if (NOMOUTH_OFFSET >= MAX_CHARS)
  {
    char buff[32];
    std::copy(&NOMOUTH[NOMOUTH_OFFSET - MAX_CHARS], &NOMOUTH[NOMOUTH_OFFSET], buff);
    for (int i = 0; i < 32; i++)
    {
      if (i == 16)
      {
        lcd.setCursor(0, 1);
      }
      lcd.write(buff[i]);
    }
    Serial.println(buff);
  }
  NOMOUTH_OFFSET++;
  delay(200);
  // // put function definitions here
}