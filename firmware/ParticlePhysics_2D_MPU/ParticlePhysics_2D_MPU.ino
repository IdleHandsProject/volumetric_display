//--------------------------------------------------------------------------
// This code is modified from the following Adafruit tutorial: https://learn.adafruit.com/animated-led-sand/overview
/*

    This code is modified from the following Adafruit tutorial: https://learn.adafruit.com/animated-led-sand/overview
    This program shows an upvote arrow 3 times then on the 4th it breaks into particles that will move based on accelerometer readings.
*/

#include <Arduino.h>
#include <math.h>

#include "I2Cdev.h"
#include "MPU6050.h"


#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


int displays [10] = {A0, A1, A2, A3, A4, A5, 10, 9, 6, 5};
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled(U8G2_MIRROR, /* cs=*/ 4, /* dc=*/ 11, /* reset=*/ 13);

//int N_GRAINS = 0;
#define N_GRAINS     100 // Number of grains of sand
#define WIDTH        128 // Display width in pixels
#define HEIGHT       56 // Display height in pixels
#define MAX_FPS      60 
#define SCALE        128
#define WBOUNCE      -2
#define BBOUNCE      -2

#define MAX_X (WIDTH  * SCALE - 1) 
#define MAX_Y (HEIGHT * SCALE - 1) 
struct Grain {
  int16_t  x,  y; 
  int16_t vx, vy; 
} grain[N_GRAINS];

int16_t rax, ray, raz;
int16_t rgx, rgy, rgz;
uint32_t        prevTime   = 0;      
uint8_t         backbuffer = 0,      
                img[WIDTH * HEIGHT];


void setup(void) {
  uint8_t i, j, bytes;
  for (int display_pin = 0; display_pin < 10; display_pin++) {

    pinMode (displays[display_pin], OUTPUT);
    digitalWrite(displays[display_pin], LOW);
  }
  Serial.begin(115200);

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
  Serial.println("Initializing I2C devices...");
  mpu.initialize();
  Serial.println("Testing device connections...");
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  oled.begin();
  oled.setFlipMode(0);
  u8g2_prepare();
  for (int display_pin = 0; display_pin < 10; display_pin++) {
    pinMode (displays[display_pin], OUTPUT);
    digitalWrite(displays[display_pin], HIGH);
  }
  memset(img, 0, sizeof(img)); // Clear the img[] array
  int grain_num = 0;
  for (int x = 0; x < 256; x++) {
      int NX = x % 128;
      int NY = x / 56;
      grain[grain_num].x = (NX  * SCALE); // Assign random position within
      grain[grain_num].y = (NY * SCALE);
      grain_num++;
  }

  for (i = 0; i < N_GRAINS; i++) { // For each sand grain...
    img[(grain[i].y / SCALE) * WIDTH + (grain[i].x / SCALE)] = 255; // Mark it
    grain[i].vx = grain[i].vy = 0; // Initial velocity is zero
  }
}


void loop() {
  digitalWrite(5, LOW);
  uint32_t t;
  while (((t = micros()) - prevTime) < (1000000L / MAX_FPS));
  prevTime = t;
  

  backbuffer = 1 - backbuffer; // Swap front/back buffer index

  // Read accelerometer...
  mpu.getMotion6(&rax, &ray, &raz, &rgx, &rgy, &rgz);
  //randomSeed(A6);

  int16_t ax = -ray / SCALE,      // Transform accelerometer axes
          ay = raz / SCALE,      // to grain coordinate space
          az = abs(rax) / SCALE; // Random motion factor

//  Serial.print(ax);
//  Serial.print(",");
//  Serial.print(ay);
//  Serial.print(",");
//  Serial.println(az);

  az = (az >= 3) ? 1 : 4 - az;      // Clip & invert
  ax -= az;                         // Subtract motion factor from X, Y
  ay -= az;
  long az2 = az * 2 + 1;         // Range of random motion to add back in


  int32_t v2; // Velocity squared
  float   v;  // Absolute velocity
  for (int i = 0; i < N_GRAINS; i++) {
    grain[i].vx += ax + random(az2); // A little randomness makes
    grain[i].vy += ay + random(az2); // tall stacks topple better!

    v2 = (int32_t)grain[i].vx * grain[i].vx + (int32_t)grain[i].vy * grain[i].vy;
    if (v2 > 65536) { // If v^2 > 65536, then v > SCALE
      v = sqrt((float)v2); // Velocity vector magnitude
      grain[i].vx = (int)(SCALE * (float)grain[i].vx / v); // Maintain heading
      grain[i].vy = (int)(SCALE * (float)grain[i].vy / v); // Limit magnitude
    }
  }

  uint8_t        i, bytes, oldidx, newidx, delta;
  int16_t        newx, newy;
  // const uint8_t *ptr = remap;

  //randomSeed(A6);
  int list[N_GRAINS];
  for (int num = 0; num < N_GRAINS; num++) {
    list[num] = num;
  }
  for (int a = 0; a < N_GRAINS; a++)
  {
    int r = random(a, (N_GRAINS));
    int temp = list[a];
    list[a] = list[r];
    list[r] = temp;
  }


  for (i = 0; i < N_GRAINS; i++) {
    newx = grain[list[i]].x + grain[list[i]].vx;
    newy = grain[list[i]].y + grain[list[i]].vy;
    if (newx > MAX_X) {
      newx         = MAX_X;
      grain[list[i]].vx /= WBOUNCE;
    } else if (newx < 0) {
      newx         = 0;
      grain[list[i]].vx /= WBOUNCE;
    }
    if (newy > MAX_Y) {
      newy         = MAX_Y;
      grain[list[i]].vy /= WBOUNCE;
    } else if (newy < 0) {
      newy         = 0;
      grain[list[i]].vy /= WBOUNCE;
    }

    oldidx = (grain[list[i]].y / SCALE) * WIDTH + (grain[list[i]].x / SCALE);
    newidx = (newy      / SCALE) * WIDTH + (newx      / SCALE);
    if ((oldidx != newidx) &&
        img[newidx]) {
      delta = abs(newidx - oldidx);
      Serial.println(delta);
      if (delta == 1) {
        newx         = grain[list[i]].x;
        grain[list[i]].vx /= BBOUNCE;
        newidx       = oldidx;
      } else if (delta == WIDTH) {
        newy         = grain[list[i]].y;
        grain[list[i]].vy /= BBOUNCE;
        newidx       = oldidx;
      } else {

        if ((abs(grain[list[i]].vx) - abs(grain[list[i]].vy)) >= 0) {
          newidx = (grain[list[i]].y / SCALE) * WIDTH + (newx / SCALE);
          if (!img[newidx]) {
            newy         = grain[list[i]].y;
            grain[list[i]].vy /= BBOUNCE;
          } else {
            newidx = (newy / SCALE) * WIDTH + (grain[list[i]].x / SCALE);
            if (!img[newidx]) {
              newx         = grain[list[i]].x;
              grain[list[i]].vx /= BBOUNCE;
            } else {
              newx         = grain[list[i]].x;
              newy         = grain[list[i]].y;
              grain[list[i]].vx /= BBOUNCE;
              grain[list[i]].vy /= BBOUNCE;
              newidx       = oldidx;
            }
          }
        } else {
          newidx = (newy / SCALE) * WIDTH + (grain[list[i]].x / SCALE);
          if (!img[newidx]) {
            newx         = grain[list[i]].x;
            grain[list[i]].vy /= BBOUNCE;
          } else {
            newidx = (grain[list[i]].y / SCALE) * WIDTH + (newx / SCALE);
            if (!img[newidx]) {
              newy         = grain[list[i]].y;
              grain[list[i]].vy /= BBOUNCE;
            } else {
              newx         = grain[list[i]].x;
              newy         = grain[list[i]].y;
              grain[list[i]].vx /= BBOUNCE;
              grain[list[i]].vy /= BBOUNCE;
              newidx       = oldidx;
            }
          }
        }
      }
    }
    grain[list[i]].x  = newx;
    grain[list[i]].y  = newy;
    img[oldidx] = 0;
    img[newidx] = 255;

  }
  oled.clearBuffer();
  for (int i = 0; i < N_GRAINS; i++) {
    int x = grain[i].x / SCALE;
    int y = grain[i].y / SCALE;
    oled.drawPixel(x, y);
  }
  oled.sendBuffer();

}

void u8g2_prepare(void) {
  oled.setFont(u8g2_font_6x10_tf);
  oled.setFontRefHeightExtendedText();
  oled.setDrawColor(1);
  oled.setFontPosTop();
  oled.setFontDirection(0);
}
