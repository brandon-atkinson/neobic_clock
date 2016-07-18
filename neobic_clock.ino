#include <avr/power.h>
#include <Debouncer.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

#define PIXEL_COUNT 60
#define PIXEL_DATA_PIN 1
#define TIME_INCR_PIN 3
#define TIME_DECR_PIN 4

#define MINUTE_ADJUST_THRESHOLD_MILLIS 3000
#define STEP_SECOND 1
#define STEP_MINUTE 60

RTC_DS1307 rtc;

Adafruit_NeoPixel neopixels = Adafruit_NeoPixel(
  PIXEL_COUNT,
  PIXEL_DATA_PIN,
  NEO_GRBW + NEO_KHZ800
);

uint8_t hand_intensity = 16;

Debouncer incr_button = Debouncer(TIME_INCR_PIN);
Debouncer decr_button = Debouncer(TIME_DECR_PIN);

void setup() {
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1); //set trinket to 16MHz
  
  pinMode(PIXEL_DATA_PIN, OUTPUT);
  pinMode(TIME_INCR_PIN, INPUT);
  pinMode(TIME_DECR_PIN, INPUT);
  
  Wire.begin(); //start i2c
  rtc.begin();
  
  neopixels.begin();
  neopixels.show();
}

void loop() {
  if (incr_button.read() == LOW) {
    adjust_clock(scale_adjustment(incr_button.held()));
  }

  if (decr_button.read() == LOW) {
    adjust_clock(-scale_adjustment(decr_button.held()));
  }

  DateTime date_time = rtc.now();
  draw_clock(date_time);
}

int32_t scale_adjustment(unsigned long button_held) {
  int32_t adjustment = STEP_SECOND;
  if (button_held > MINUTE_ADJUST_THRESHOLD_MILLIS) {
    adjustment = STEP_MINUTE;
  }
  return adjustment;
}

void adjust_clock(int32_t adjustment_seconds) {
  DateTime newTime = rtc.now() + TimeSpan(adjustment_seconds);
  rtc.adjust(newTime);
}

void draw_clock(DateTime now) {
  neopixels.clear();
  for (uint16_t pixel = 0; pixel < neopixels.numPixels(); pixel++) {
    uint32_t color = 0;
    if (pixel % 5 == 0) {
      color += neopixels.Color(0,0,0,1);
    }
    if (((now.hour() % 12) * 5) + (now.minute() / 12) == pixel) {
      color += neopixels.Color(hand_intensity,0,0,0);
    }
    if (now.minute() == pixel) {
      color += neopixels.Color(0,hand_intensity,0,0);
    }
    if (now.second() == pixel) {
      color += neopixels.Color(0,0,hand_intensity,0);
    }

    neopixels.setPixelColor(pixel, color);
  }
  neopixels.show();
}



