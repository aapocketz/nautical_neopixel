
// nautical light display using neopixels
// author: Aaron Paquette <aapocketz@gmail.com

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
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// how many LEDs are we using for navigation lights
#define LED_COUNT 20

// ms between simulated ticks.
#define TICK_PERIOD 100

// flash duration (period light is on during flashing (or off for occulting)
// duty cycle for normal flash will be FLASH_DURATION/FLASH_PERIOD
#define FLASH_DURATION 500

// flashing period (flash 30 times a minute)
#define FLASH_PERIOD 2000

// quick flashing period (flash once a second)
#define QUICK_PERIOD 1000

// array of char strings
// these must correspond to NOAA nautical chart 1 specifications for 
// lighted aids to navigation, indicating the pattern and color of lights
// as listed on nautical charts themselves
// for example "Fl R 4s" would be Flashing Red 4 second period"
// or "Fl (4+5) G 30s" would be 4 green short flashes followed by 
// 5 green short flashes every 30 seconds
// NOTE: there are a lot of options for lighted aids, and handlers may 
// need to be added for types not included here!
// a good reference is this: https://www.navcen.uscg.gov/pdf/lightlists/LightList_V2_2019.pdf


// these are the nautical lights needed for chart 12270
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

char* nav_leds[LED_COUNT] = {
  "Q G",
  "Q (2) R 10s",
  "Q (2+1) G 10s",
  "Fl R 10s",
  "Oc G 10s"

};

// shift the phase so the leds are less likely to blink at the same time
// lowering the max current draw
// each LED will start its period shifted by phase offset. Set to zero if 
// you want them blinking simultaneously
int phase_offset = FLASH_DURATION * 0;


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
const uint32_t WHITE = strip.Color(127, 127, 127);
// const uint32_t WHITE = strip.Color(0, 0, 255); // use blue for testing as its more visible than white
const uint32_t MAGENTA = strip.Color(255, 0, 255);
const uint32_t OFF = strip.Color(0, 0, 0); 


unsigned int global_counter = 0;


// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  // strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  strip.clear();
  unsigned int counter = global_counter;
  for (int led_idx = 0; led_idx < LED_COUNT; ++led_idx) {    
    parse(counter, led_idx, nav_leds[led_idx]);
    counter += phase_offset;
  }
  strip.show(); // Update strip with new contents
  delay(TICK_PERIOD);  // increment tick
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
int aToPeriod(char * cstr) {
  float seconds = atof(cstr);
  return seconds * 1000;
}

// parse the lighting characteristics string
void parse(unsigned int count, int led_idx, char* str) {
  
  // main color
  uint32_t color = MAGENTA;
  
  // check the length string
  int len = strlen(str);
  
  // ensure the length is something reasonable
  if (len < 2 or len > 30) return;
  
  // check for quick flashing
  if (str[0] == 'Q') {
	  if (str[1] == ' ' and str[2] != '(') {
		  // quick flashing
		  color = charToColor(str[2]);
      flash(count, led_idx, color, OFF, 1, 0, FLASH_DURATION, QUICK_PERIOD-FLASH_DURATION, QUICK_PERIOD);
	  } else {
		  // quick group flashing
		  int group1 = 1;
		  int group2 = 0;
		  int period = 100000;
		  if (str[2] == '(' and isdigit(str[3])) {
			  if (str[4] == ')') {
				  // single group flashing
				  group1 = atoi(&str[3]);
				  // offset pointer
				  str += 6;
			  } else if (str[4] == '+' and isdigit(str[5]) and
				     str[6] == ')') {
				  // composite group flashing
				  group1 = atoi(&str[3]);
				  group2 = atoi(&str[5]);
				  // offset pointer
				  str += 8;
			  }
		  }
		  color = charToColor(str[0]);
		  period = aToPeriod(&str[2]);
		  flash(count, led_idx, color, OFF, group1, group2, FLASH_DURATION, QUICK_PERIOD-FLASH_DURATION, period);
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
		  if (str[3] == '(' and isdigit(str[4])) {
			  if (str[5] == ')') {
				  // single group flashing
				  group1 = atoi(&str[4]);
				  // offset pointer
				  str += 7;
			  } else if (str[5] == '+' and isdigit(str[6]) and
				     str[7] == ')') {
				  // composite group flashing
				  group1 = atoi(&str[4]);
				  group2 = atoi(&str[6]);
				  // offset pointer
				  str += 9;
			  }
		  } else if (not isdigit(str[3]) and str[4] == ' ') {
			  // offset pointer
			  str += 3;
		  }
		  color = charToColor(str[0]);
		  period = aToPeriod(&str[2]);

		  flash(count, led_idx, color, OFF, group1, group2, FLASH_DURATION, FLASH_PERIOD-FLASH_DURATION, period);
	  }
  } else if (str[0] == 'O' and str[1] == 'c') {
	  // occulting
	  // flashing
	  int group1 = 1;
	  int group2 = 0;
	  int period = 100000;
	  if (str[3] == '(' and isdigit(str[4])) {
		  if (str[5] == ')') {
			  // single group flashing
			  group1 = atoi(&str[4]);
			  // offset pointer
			  str += 7;
		  } else if (str[5] == '+' and isdigit(str[6]) and
			     str[7] == ')') {
			  // composite group flashing
			  group1 = atoi(&str[4]);
			  group2 = atoi(&str[6]);
			  // offset pointer
			  str += 9;
		  }
	  } else if (not isdigit(str[3]) and str[4] == ' ') {
		  // offset pointer
		  str += 3;
	  }
	  color = charToColor(str[0]);
	  period = aToPeriod(&str[2]);

    flash(count, led_idx, OFF, color, group1, group2, FLASH_DURATION, FLASH_PERIOD-FLASH_DURATION, period);
  }
}

// fixed color
void fixed(unsigned int count, int led_idx, uint32_t color) {
    strip.setPixelColor(led_idx, color);
}

// flashing
void flash(unsigned int count, int led_idx, uint32_t on_color, uint32_t off_color, int group1, int group2, int on_time, int off_time, int period) {
  
  count = count % period;
  uint32_t active_color = off_color;

  int sub_period = on_time + off_time;
  
  if ((count % sub_period) > on_time) {
    active_color = off_color;
  } else {
    int subframe = count / sub_period;
    // determine groupset
    // assume half the period is for the first grouping
    int group = group1;
    if ((count * 2) >= period) {
      group = group2;
      count = count % (period / 2);
      subframe = count / sub_period;
    }
    if (subframe < group) {
      active_color = on_color;
    }
  }
  strip.setPixelColor(led_idx, active_color);
}