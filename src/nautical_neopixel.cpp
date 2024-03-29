
// nautical light display using neopixels
// author: Aaron Paquette <aapocketz@gmail.com>

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// set this to enable avr-stub debugging
#ifdef DEBUG
#include "avr8-stub.h"
#endif

// decl
void parse(unsigned int count, int led_idx, char *str);
void flash(unsigned int count, int led_idx, uint32_t on_color,
           uint32_t off_color, int group1, int group2, int on_time,
           int off_time, int period);
void morse(unsigned int count, int led_idx, uint32_t color, char ch,
           int period);
void fixed(unsigned int count, int led_idx, uint32_t color);
int aToGroups(char *str, int &group1, int &group2);

// how many LEDs are we using for navigation lights
#define LED_COUNT 20

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 6

// clang-format off
// array of char strings
// these must correspond to NOAA nautical chart 1 specifications for
// lighted aids to navigation, indicating the pattern and color of lights
// as listed on nautical charts themselves
// for example "Fl R 4s" would be Flashing Red 4 second period"
// or "Fl (4+5) G 30s" would be 4 green short flashes followed by
// 5 green short flashes every 30 seconds
// NOTE: there are a lot of options for lighted aids, and handlers may
// need to be added for types not included here!
// a good reference is this:
// https://www.navcen.uscg.gov/pdf/lightlists/LightList_V2_2019.pdf

// these are the nautical lights needed for chart 12270
// Chesapeake Bay Eastern Bay and South River; Selby Bay
// https://www.charts.noaa.gov/OnLineViewer/12270.shtml
// char* nav_leds[LED_COUNT] = {
//   "Fl (4+5) G 30s",
//   "Fl G 4s",
//   "Q G",
//   "Fl G 2.5s",
//   "Fl (2+1) G 6s",
//   "Fl G 6s",
//   "Q R",
//   "Fl R 2.5",
//   "Fl R 4s",
//   "Fl R 6s",
//   "Fl (2+1) R 6s",
//   "Fl W 6s",
//   "Fl W 10s",
//   "Fl W 4s",
//   "Fl W 5s",
//   "Fl (4+3) Y 30s",
//   "Fl Y 2.5s",
//   "Fl Y 4s",
//   "F Y",
//   "Oc (2+1) R 6s"
// };

// nautical chart 12283 Annapolis Harbor
// https://www.charts.noaa.gov/OnLineViewer/12283.shtml
char* nav_leds[LED_COUNT] = {
  "Q G",
  "Q W",
  "Fl R 2.5s",
  "Fl R 2.5s",
  "Fl Y 2.5s",
  "Fl G 2.5s",
  "Fl R 4s",
  "Fl R 4s",
  "Fl (4+5) G 30s",
  "Fl G 4s",
  "Fl G 4s",
  "Fl (4+3) Y 30s",
  "Fl Y 4s",
  "Fl W 6s",
  "Fl R 6s",
  "Fl W 10s"
};


// // test examples
// char* nav_leds[LED_COUNT] = {
//   "Q (6+LFl) R 15s",
//   "L.Fl G 10s",
//   "Q R",
//   "VQ R",
//   "UQ R",
//   "VQ (3) G 5s",
//   "Mo (U) R 6s",
//   "Iso R 5s",
//   "Q G",
//   "Q (2) R 10s",
//   "Q (2+1) G 10s",
//   "Fl R 10s",
//   "Oc G 10s"

// };
// clang-format on

// ms between simulated ticks.
#define TICK_PERIOD 100

// flash duration (period light is on during flashing (or off for occulting)
// duty cycle for normal flash will be FLASH_DURATION/FLASH_PERIOD
// flashing period (flash 30 times a minute)
#define FLASH_DURATION 500
#define FLASH_PERIOD 2000

// quick flashing duration/period (flash 60 times a minute)
#define QUICK_DURATION 500
#define QUICK_PERIOD 1000

// very quick flashing period (flash 120 times a minute)
#define VERY_QUICK_DURATION 250
#define VERY_QUICK_PERIOD 500

// ultra quick flashing period (flash 240 times a minute)
#define ULTRA_QUICK_DURATION 125
#define ULTRA_QUICK_PERIOD 250

// long flashing period (flash 30 times a minute)
#define LONG_FLASH_DURATION 2000
#define LONG_FLASH_PERIOD 4000

// morse code constants
char *alpha[] = {
    ".-",   // A
    "-...", // B
    "-.-.", // C
    "-..",  // D
    ".",    // E
    "..-.", // F
    "--.",  // G
    "....", // H
    "..",   // I
    ".---", // J
    "-.-",  // K
    ".-..", // L
    "--",   // M
    "-.",   // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.",  // R
    "...",  // S
    "-",    // T
    "..-",  // U
    "...-", // V
    ".--",  // W
    "-..-", // X
    "-.--", // Y
    "--.."  // Z
};

#define MORSE_UNIT 500
#define DOT_TIME 1 * MORSE_UNIT
#define DASH_TIME 3 * MORSE_UNIT
#define SPACE_TIME 1 * MORSE_UNIT


// programmable phase offset per LED
// useful to manage power envelope and keep lights from shining at the same exact time
// for example here we put red,green, white and yellow so their normal flashes are at different phases
#define R_PHASE FLASH_DURATION * 0
#define G_PHASE FLASH_DURATION * 3
#define W_PHASE FLASH_DURATION * 2
#define Y_PHASE FLASH_DURATION * 1
int phase_offsets[LED_COUNT] = {
  G_PHASE, // "Q G", // these are the Annapolis LED lights
  W_PHASE, // "Q W",
  R_PHASE, // "Fl R 2.5s",
  R_PHASE, // "Fl R 2.5s",
  Y_PHASE, // "Fl Y 2.5s",
  G_PHASE, // "Fl G 2.5s",
  R_PHASE, // "Fl R 4s",
  R_PHASE, // "Fl R 4s",
  G_PHASE, // "Fl (4+5) G 30s",
  G_PHASE, // "Fl G 4s",
  G_PHASE, // "Fl G 4s",
  Y_PHASE, // "Fl (4+3) Y 30s",
  Y_PHASE, // "Fl Y 4s",
  W_PHASE, // "Fl W 6s",
  R_PHASE, // "Fl R 6s",
  W_PHASE // "Fl W 10s"
};




// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// commonly used nautical light colors
const uint32_t RED = strip.Color(255, 0, 0);
const uint32_t GREEN = strip.Color(0, 255, 0);
const uint32_t YELLOW = strip.Color(255, 255, 0);
// const uint32_t WHITE = strip.Color(127, 127, 127);
const uint32_t WHITE = strip.Color(200, 200, 200);
// const uint32_t WHITE = strip.Color(0, 0, 255); // use blue for testing as its
// more visible than white
const uint32_t MAGENTA = strip.Color(255, 0, 255);
const uint32_t OFF = strip.Color(0, 0, 0);

unsigned int global_counter = 0;

// setup() function -- runs once at startup --------------------------------

void setup() {

#ifdef DEBUG
  debug_init();
#endif

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP
  strip.setBrightness(100); // Set BRIGHTNESS to about 1/5 (max = 255)
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  strip.clear();
  for (int led_idx = 0; led_idx < LED_COUNT; ++led_idx) {
    unsigned int counter = global_counter + phase_offsets[led_idx];
    parse(counter, led_idx, nav_leds[led_idx]);
  }
  strip.show();       // Update strip with new contents
  delay(TICK_PERIOD); // increment tick
  global_counter += TICK_PERIOD;
}

// utility function color mapping
uint32_t charToColor(char ch) {
  switch (ch) {
  case 'R':
    return RED;
  case 'G':
    return GREEN;
  case 'Y':
    return YELLOW;
  case 'W':
    return WHITE;
  default:
    // magenta may indicate error!
    return MAGENTA;
  }
}

// utility function period parsing
int aToPeriod(char *cstr) {
  int len = strlen(cstr);
  if (len > 0 and cstr[len - 1] == 's') {
    // lop off s in string
    cstr[len - 1] = NULL;
  }
  float seconds = atof(cstr);
  return seconds * 1000;
}

// parse the lighting characteristics string
void parse(unsigned int count, int led_idx, char *str) {
  // main color
  uint32_t color = MAGENTA;

  // check the length string
  int len = strlen(str);

  // ensure the length is something reasonable
  if (len < 2 or len > 30)
    return;

  if (str[0] == 'I' and str[1] == 's' and str[2] == 'o') {
    // isophase
    color = charToColor(str[4]);
    int period = aToPeriod(&str[6]);
    flash(count, led_idx, color, OFF, 1, 0, period / 2, 0, period);
  } else if (str[0] == 'Q') {
    if (str[1] == ' ' and str[2] != '(') {
      // quick flashing
      color = charToColor(str[2]);
      flash(count, led_idx, color, OFF, 1, 0, QUICK_DURATION,
            QUICK_PERIOD - QUICK_DURATION, QUICK_PERIOD);
    } else {
      // quick group flashing
      // very quick group flashing
      int group1 = 1;
      int group2 = 0;
      int period = 100000;
      str += 2;
      int char_skip = aToGroups(str, group1, group2);
      str += char_skip;
      color = charToColor(str[0]);
      period = aToPeriod(&str[2]);
      flash(count, led_idx, color, OFF, group1, group2, QUICK_DURATION,
            QUICK_PERIOD - QUICK_DURATION, period);
    }
  } else if (str[0] == 'V' and str[1] == 'Q') {
    // very quick
    if (str[2] == ' ' and str[3] != '(') {
      // very quick flashing
      color = charToColor(str[3]);
      flash(count, led_idx, color, OFF, 1, 0, VERY_QUICK_DURATION,
            VERY_QUICK_PERIOD - VERY_QUICK_DURATION, VERY_QUICK_PERIOD);
    } else {
      // very quick group flashing
      int group1 = 1;
      int group2 = 0;
      int period = 100000;
      str += 3;
      int char_skip = aToGroups(str, group1, group2);
      str += char_skip;
      color = charToColor(str[0]);
      period = aToPeriod(&str[2]);
      flash(count, led_idx, color, OFF, group1, group2, VERY_QUICK_DURATION,
            VERY_QUICK_PERIOD - VERY_QUICK_DURATION, period);
    }
  } else if (str[0] == 'U' and str[1] == 'Q') {
    // ultra quick
    if (str[2] == ' ' and str[3] != '(') {
      // ultra quick flashing
      color = charToColor(str[3]);
      flash(count, led_idx, color, OFF, 1, 0, ULTRA_QUICK_DURATION,
            ULTRA_QUICK_PERIOD - ULTRA_QUICK_DURATION, ULTRA_QUICK_PERIOD);
    } else {
      // ultra quick group flashing
      int group1 = 1;
      int group2 = 0;
      int period = 100000;
      str += 3;
      int char_skip = aToGroups(str, group1, group2);
      str += char_skip;
      color = charToColor(str[0]);
      period = aToPeriod(&str[2]);
      flash(count, led_idx, color, OFF, group1, group2, ULTRA_QUICK_DURATION,
            ULTRA_QUICK_PERIOD - ULTRA_QUICK_DURATION, period);
    }
  } else if (str[0] == 'F') {
    if (str[1] == ' ') {
      // fixed color
      color = charToColor(str[2]);
      fixed(count, led_idx, color);
    } else if (str[1] == 'l' and str[2] == ' ') {
      // flashing
      int group1 = 1;
      int group2 = 0;
      int period = 100000;
      str += 3;
      int char_skip = aToGroups(str, group1, group2);
      str += char_skip;
      color = charToColor(str[0]);
      period = aToPeriod(&str[2]);
      flash(count, led_idx, color, OFF, group1, group2, FLASH_DURATION,
            FLASH_PERIOD - FLASH_DURATION, period);
    }
  } else if (str[0] == 'L' and str[1] == '.' and str[2] == 'F' and
             str[3] == 'l') {
    // flashing
    int group1 = 1;
    int group2 = 0;
    int period = 100000;
    str += 5;
    int char_skip = aToGroups(str, group1, group2);
    str += char_skip;
    color = charToColor(str[0]);
    period = aToPeriod(&str[2]);
    flash(count, led_idx, color, OFF, group1, group2, LONG_FLASH_DURATION,
          LONG_FLASH_PERIOD - LONG_FLASH_DURATION, period);
  } else if (str[0] == 'O' and str[1] == 'c') {
    // occulting
    // flashing
    int group1 = 1;
    int group2 = 0;
    int period = 100000;
    str += 3;
    int char_skip = aToGroups(str, group1, group2);
    str += char_skip;
    color = charToColor(str[0]);
    period = aToPeriod(&str[2]);
    flash(count, led_idx, OFF, color, group1, group2, FLASH_DURATION,
          FLASH_PERIOD - FLASH_DURATION, period);
  } else if (str[0] == 'M' and str[1] == 'o') {
    // Morse code
    if (str[3] == '(' and str[5] == ')') {
      char ch = str[4];
      // offset pointer
      str += 7;
      color = charToColor(str[0]);
      int period = aToPeriod(&str[2]);
      morse(count, led_idx, color, ch, period);
    }
  }
}
// determine groups from strings
// return: number of char consumed
// group1 and group2 are also return values
int aToGroups(char *str, int &group1, int &group2) {
  if (str[0] == '(' and isdigit(str[1])) {
    if (str[2] == ')') {
      // single group flashing
      group1 = atoi(&str[1]);
      group2 = 0;
      return 4;
    } else if (str[2] == '+' and isdigit(str[3]) and str[4] == ')') {
      // composite group flashing
      group1 = atoi(&str[1]);
      group2 = atoi(&str[3]);
      return 6;
    } else if (str[2] == '+' and str[3] == 'L' and str[4] == 'F' and
               str[5] == 'l' and str[6] == ')') {
      // composite group flashing with second group long flash
      group1 = atoi(&str[1]);
      group2 = -1; // use negative values to support long flashes
      return 8;
    }
  } else {
    group1 = 1;
    group2 = 0;
    return 0;
  }
}

// fixed color
void fixed(unsigned int count, int led_idx, uint32_t color) {
  strip.setPixelColor(led_idx, color);
}

// clang-format off
// flashing
// controls sequence of led flashes
//
// count: the clock counter in microsec ticks
//
// led_idx: the led index
//
// on_color: the color of the led when the flash is "on". Note: occulting lights
// this color could be "OFF" off_color: the color of the led when the flash is
// "off". Normal lights will have this color be "OFF"
//
// group1: the number of flashes during the first half period
//
// group2: number of flashes during the second half period
//
// on_time: the number of ticks the light is "on" when flashing
//
// off_time: the number of ticks the light is "off" when flashing
//
// period: the repeat period of the flashes
//
// example:
// this is what a group1=2, group2=0, on_time=5, off_time=5, and period of 40
// would look like the | represents the period, the {} are groups and the + is
// when we are on, and the - are when we are off.
//
//             |{+++++-----+++++-----}{--------------------}|
//
// important note! this function currently doesn't do any error checking to make
// sure values are sane.
// clang-format on

void flash(unsigned int count, int led_idx, uint32_t on_color,
           uint32_t off_color, int group1, int group2, int on_time,
           int off_time, int period) {
  count = count % period;
  uint32_t active_color = off_color;

  int sub_period = on_time + off_time;

  int subframe = count / sub_period;
  // determine groupset
  // assume half the period is for the first grouping
  int group = group1;
  if ((count * 2) >= period) {
    group = group2;
    if (group < 0) {
      // special case long flash, kinda hacky here
      // modify the flash duration values
      group = -group;
      on_time = LONG_FLASH_DURATION;
      off_time = LONG_FLASH_PERIOD - LONG_FLASH_DURATION;
      sub_period = on_time + off_time;
    }
    count = count % (period / 2);
    subframe = count / sub_period;
  }
  if (subframe < group) {
    active_color = on_color;
  }
  if ((count % sub_period) > on_time) {
    active_color = off_color;
  }

  strip.setPixelColor(led_idx, active_color);
}

void morse(unsigned int count, int led_idx, uint32_t color, char ch,
           int period) {
  count = count % period;
  char *code_str = alpha[ch - 'A'];

  uint32_t active_color = OFF;
  int on_time = 0;
  for (int i = 0; i < strlen(code_str); ++i) {
    char t = code_str[i];
    int off_time = on_time;
    if (t == '.') {
      off_time += DOT_TIME;
    } else {
      off_time += DASH_TIME;
    }
    if (on_time <= count and count < off_time) {
      active_color = color;
      break;
    }
    on_time = off_time + SPACE_TIME;
  }
  strip.setPixelColor(led_idx, active_color);
}
