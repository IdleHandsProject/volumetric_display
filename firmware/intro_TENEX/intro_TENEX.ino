#include <Arduino.h>
#include <math.h>

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


//MAX RESOLUTION PIXELS (X/Y is the resolution of a single display, Z is depth)
#define X_RES 128
#define Y_RES 56
#define Z_RES 10
#define Z_DISTANCE 2
#define acc_SCALE 10

#define X_MAX 12700
#define Y_MAX 5600
#define Z_MAX 9000

int ball_xyz[3] = {64000, 25000, 5000};
int max_xyz[3] = {127000, 55000, 9000};
int vel_xyz[3] = {0, 0, 0};
int ball_r = 8000;
float resitution = 1.3;

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

#define OUTPUT_READABLE_ACCELGYRO

int displays [10] = {A0, A1, A2, A3, A4, A5, 10, 9, 6, 5};

bool volumeBuffer[128][56][10] = {0};

//U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled(U8G2_MIRROR, /* cs=*/ -1, /* dc=*/ 11, /* reset=*/ 13);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled(U8G2_R0, /* cs=*/ -1, /* dc=*/ 11, /* reset=*/ 13);

#define X_MAX 12700
#define Y_MAX 5600
#define Z_MAX 9000

#define PIEZO 1


#define SH_width 128
#define SH_height 64
unsigned const char SH[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xF8, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xD0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF6, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
  0xFF, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x7F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFE, 0x1F, 0x00, 0x00, 0x00, 0xE0, 0x03, 0x70, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00, 0x00,
  0xE0, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFE, 0x07, 0x00, 0x00, 0xE0, 0x07, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x1F, 0x00, 0x00, 0xC0, 0x07, 0xF0, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
  0xC0, 0x0F, 0xF8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xF8, 0x03, 0x00, 0xC0, 0x0F, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x0F, 0x00, 0xC0, 0x0F, 0xF0, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00,
  0x80, 0x0F, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFE, 0x00, 0x80, 0x0F, 0xE0, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x03, 0x80, 0x0F, 0xE0, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x07,
  0x80, 0x1F, 0xE0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xC0, 0x0F, 0x00, 0x1F, 0xC0, 0x07, 0x00, 0xF8, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x0F, 0xC0, 0x07,
  0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E,
  0x00, 0x1F, 0xC0, 0x07, 0xFD, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFC, 0x00, 0x1F, 0xC0, 0xFF, 0xFF, 0x09, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x5F, 0xFF, 0xFF,
  0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
  0xF3, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xE0, 0xFF, 0xFF, 0x81, 0x0F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x1F, 0x80, 0x1F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF,
  0x1F, 0x1F, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0xCF, 0x0F, 0x3F, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x81, 0x0F, 0x3F, 0x00, 0x1F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x1F, 0x80,
  0x0F, 0x3F, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0xFF, 0x01, 0x00, 0x1F, 0x3E, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x7F, 0x00, 0x80, 0x0F, 0x3F, 0x00, 0x3E,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x80,
  0x1F, 0x7E, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFC, 0x03, 0x00, 0x80, 0x0F, 0xFE, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x80, 0x0F, 0xFE, 0x01, 0x78,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0xC0,
  0x07, 0xFE, 0x01, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0x00, 0x00, 0xC0, 0x07, 0xFC, 0x01, 0x78, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x1F, 0x00, 0x00, 0xE0, 0x03, 0x20, 0x00, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x00, 0x00, 0xF0,
  0x03, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x0F, 0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x7F,
  0x00, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFC, 0x01, 0xC0, 0x3F, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F, 0xFC, 0x0F, 0x00, 0x00, 0x00, 0xC0,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0x03,
  0x00, 0x00, 0x00, 0xC0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFC, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};


// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

void initializeAllDisplays() {
  for (int display_pin = 0; display_pin < 10; display_pin++) {
    pinMode (displays[display_pin], OUTPUT);
    digitalWrite(displays[display_pin], LOW);
  }
  oled.begin();
  oled.setFlipMode(0);
  for (int display_pin = 0; display_pin < 10; display_pin++) {
    pinMode (displays[display_pin], OUTPUT);
    digitalWrite(displays[display_pin], HIGH);
  }
}

int layer = 9;
long stime = 0;

void setup() {

  pinMode(PIEZO, OUTPUT);
  digitalWrite(PIEZO, LOW);

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif


  Serial.begin(115200);
  //while (!Serial);

  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  initializeAllDisplays();
  oled_prepare();
  stime = millis();

  drawWaveDisplay(27, 25, 0);


}

int dir = 0;
int animation = 0;
int max_menu = 9;
int rotateTrig = 0;
int rotateOffset = 0;
int squareXOffset = 0;
int squareYOffset = 0;
int squareXDir = 1;
int squareYDir = 1;
byte xory = 0;
int bounce = 0;
int order[10] = {8, 0, 2, 3, 4, 5, 6, 7, 9, 1};

void loop() {
  //  Uncomment to use accelerometer data
  //accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // drawRandomFill();

  if (millis() - stime > 7000) {
    animation++;
    if (animation > max_menu) {
      animation = 0;
    }
    if (animation == 2) {

      vel_xyz[0] = random(5000, 7000);
      vel_xyz[1] = random(5000, 7000);
      vel_xyz[2] = random(500, 1000);

    }
    clearVolumeBuffer();
    drawVolumeBuffer();
    stime = millis();
    bounce = 0;
  }
  switch (order[animation]) {
    case 0:
      {
        for (int i = 0; i < 10; i++) {
          if (layer == i) {
            drawWaveDisplay(25 + i, 25, i);
            //tick();
          }
          drawRecDisplay(64, 28, i + 54, i + 18,  9 - i);
        }
        layer--;
        if (layer < 0) {
          layer = 9;
        }
        drawVolumeBuffer();
        break;
      }
    case 1:
      {
        for (int display_num = 0; display_num < 10; display_num++) {
          startDrawDisplay(display_num);
          //oled.drawStr( 30, 20, "Hello World");
          drawSH();
          endDrawDisplay(display_num);
          delay(70);
          clearDisplayZ(display_num);

        }
        for (int display_num = 9; display_num >= 0; display_num--) {
          startDrawDisplay(display_num);
          drawSH();
          endDrawDisplay(display_num);
          delay(70);
          clearDisplayZ(display_num);
        }
        break;
      }
    case 2://3D BOUCING BALL
      {
        int acc[3] = {5, 2, 1};
        int scaled_xyz[3] = {0, 0, 0};
        for (int axis = 0; axis < 3; axis++) {
          vel_xyz[axis] += acc[axis];
          ball_xyz[axis] += vel_xyz[axis];
          if (axis < 2) {
            if (ball_xyz[axis] > (max_xyz[axis] - ball_r)) {
              ball_xyz[axis] = max_xyz[axis] - ball_r;
              vel_xyz[axis] /= float(-1.2);

            }
            if (ball_xyz[axis] < (1 + ball_r)) {
              ball_xyz[axis] = 1 + ball_r;
              vel_xyz[axis] /= float(-1.2);

            }
          }
          if (axis == 2) {
            if (ball_xyz[axis] > (max_xyz[axis])) {
              ball_xyz[axis] = max_xyz[axis];
              vel_xyz[axis] /= float(-1.2);

            }
            if (ball_xyz[axis] < 1) {
              ball_xyz[axis] = 1;
              vel_xyz[axis] /= float(-1.2);

            }
          }
          scaled_xyz[axis] = ball_xyz[axis] / 1000;
        }
        Serial.print(vel_xyz[0]);
        Serial.print(", ");
        Serial.print(vel_xyz[1]);
        Serial.print(", ");
        Serial.println(vel_xyz[2]);
        drawCircleDisplay(scaled_xyz[0], scaled_xyz[1], ball_r / 1000, scaled_xyz[2]);
        for (int i = 0; i < 10; i++) {
          drawRecDisplay(64, 27, 63, 27,  9 - i);
        }
        drawVolumeBuffer();
        startDrawDisplay(9);
        oled.drawBox(0, 0, 127, 55);
        endDrawDisplay(9);
        //Serial.println(millis()-stime);
        clearVolumeBuffer();

        break;
      }
    case 3://Random FILL Pixels
      {
        drawRandomFill();
        drawVolumeBuffer();
        delay(70);
        break;
      }

    case 4://LINES
      {
        int x_location[40];
        int z_location[40];
        int y_location[40];
        for (int numLines = 0; numLines < 40; numLines++) {
          z_location[numLines] = random(9);
          x_location[numLines] = random(127);
          y_location[numLines] = random(55);
        }
        for (int display_num = 0; display_num < 10; display_num++) {
          startDrawDisplay(display_num);
          for (int numLines = 0; numLines < 40; numLines++) {
            if (z_location[numLines] == display_num) {
              int randx = random(127);
              int randy = random(55);
              oled.drawLine(x_location[numLines], 0, x_location[numLines], 55);
            }
          }
          endDrawDisplay(display_num);
          delay(70);
        }
        break;
      }
    case 5:
      {
        int cx = 64;
        int cy = 30;

        int trisize = 20;
        int px[3] = {0, 0, 0};
        int py[3] = {0, 0, 0};
        int tempx = 0;
        int tempy = 0;
        int angle[3] = {0, 120, 240};
        for (int display_num = 0; display_num < 9; display_num++) {
          for (int j = 0; j < 3; j++) {
            float rad = (float(angle[j] + rotateTrig + (display_num * rotateOffset)) * 71) / 4068;
            //Serial.println(rad);
            rotate_point(cx, cy, rad, tempx, tempy, trisize);
            px[j] = tempx;
            py[j] = tempy;
          }
          startDrawDisplay(display_num);
          //if (display_num == layer) {
          //  oled.drawCircle(cx, cy, trisize, U8G2_DRAW_ALL);
          //}
          //else {
          oled.drawLine(px[1], py[1], px[2], py[2]);
          oled.drawLine(px[0], py[0], px[1], py[1]);
          oled.drawLine(px[0], py[0], px[2], py[2]);
          //}
          endDrawDisplay(display_num);
          if ((rotateTrig % 35) == 0) {
            layer--;
            if (layer < 0) {
              layer = 9;
            }
          }
        }
        delay(10);
        break;
      }
    case 6:
      {
        for (int display_num = 0; display_num < 9; display_num++) {
          startDrawDisplay(display_num);
          int shrinkx = (display_num * 6);
          int shrinky = (display_num * 2);
          int xO = squareXOffset * display_num / 4;
          int yO = squareYOffset * display_num / 8;
          oled.drawRFrame(0 + shrinkx + (xO * 2), 0 + shrinky + (yO * 2), 127 - shrinkx * 2 + xO, 55 - shrinky * 2 + yO, 10 - display_num);
          endDrawDisplay(display_num);
        }
        delay(50);
        if (xory == 1) {
          squareXOffset += squareXDir;
          if ((squareXOffset > 3) || (squareXOffset < -3)) {
            squareXDir *= -1;
            xory = 0;
          }
        }
        else {
          squareYOffset += squareYDir;
          if ((squareYOffset > 3) || (squareYOffset < -3)) {
            squareYDir *= -1;
            xory = 1;
          }
        }
        break;
      }
    case 7:
      {
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        int xc = 64;
        int yc = 30;
        int d = 25;

        int bd = d * 1000;
        int acc[3] = {0, 0, 0};

        acc[0] = ay / 50;
        acc[1] = az / 50;
        acc[2] = ax / 50;

        if (bounce == 0) {
          acc[1] = random(3000, 5000);
          acc[0] = random(-6000, 6000);
          bounce = 1;
        }

        int scaled_xyz[3] = {64, 30, 5};
        for (int axis = 0; axis < 2; axis++) {
          vel_xyz[axis] += acc[axis];
          ball_xyz[axis] += vel_xyz[axis];
          if (axis < 2) {
            if (ball_xyz[axis] > (max_xyz[axis] - bd)) {
              ball_xyz[axis] = max_xyz[axis] - bd;
              vel_xyz[axis] /= float(-1.2);
            }
            if (ball_xyz[axis] < (1 + bd)) {
              ball_xyz[axis] = 1 + bd;
              vel_xyz[axis] /= float(-1.2);

            }

            if (ball_xyz[axis] < 1) {
              ball_xyz[axis] = 1;
              vel_xyz[axis] /= float(-1.2);
            }
          }
          scaled_xyz[axis] = ball_xyz[axis] / 1000;
        }

        //        if(vel_xyz[1] < 10){
        //          bounce = 0;
        //        }



        xc = scaled_xyz[0];
        yc = scaled_xyz[1];

        drawCircleDisplay(xc, yc, d, 4);
        drawCircleDisplay(xc, yc, d - 2, 5);
        drawCircleDisplay(xc, yc, d - 4, 6);
        drawCircleDisplay(xc, yc, d - 8, 7);
        drawCircleDisplay(xc, yc, d - 16, 8);
        drawCircleDisplay(xc, yc, d - 22, 9);
        drawCircleDisplay(xc, yc, d - 2, 3);
        drawCircleDisplay(xc, yc, d - 4, 2);
        drawCircleDisplay(xc, yc, d - 8, 1);
        drawCircleDisplay(xc, yc, d - 16, 0);
        drawCircleDisplay(xc, yc, d - 22, 0);

        drawVolumeBuffer();
        break;
      }
    case 8:
    { for (int display_num = 0; display_num < 10; display_num++) {
          startDrawDisplay(display_num);
          oled.drawBox(0, 0, 127, 55);
          endDrawDisplay(display_num);
          delay(100);
          clearDisplayZ(display_num);

        }
        break;
      }
    case 9:
      { 
        for (int display_num = 9; display_num >= 0; display_num--) {
          startDrawDisplay(display_num);
          drawSH();
          endDrawDisplay(display_num);
          delay(700);
          if (display_num == 0){
            delay(10000);
          }
          clearDisplayZ(display_num);
        }
        break;
      }
    default:
      break;
  }
  rotateTrig += 7;
  rotateOffset += 1;
  clearVolumeBuffer();
}



void oled_prepare(void) {
  oled.setFont(u8g2_font_6x10_tf);
  oled.setFontRefHeightExtendedText();
  oled.setDrawColor(1);
  oled.setFontPosTop();
  oled.setFontDirection(0);
}

void drawVolumeBuffer() {
  int axis_res[3] = {X_RES, Y_RES, Z_RES};
  for (int display_num = 0; display_num < 10; display_num++) {
    digitalWrite(displays[display_num], LOW);
    oled.clearBuffer();
    for (int y = 0; y < Y_RES; y++) {
      for (int x = 0; x < X_RES; x++) {
        if (volumeBuffer[x][y][display_num] == 1) {
          oled.drawPixel(x, y);
        }
      }
    }
    oled.sendBuffer();
    digitalWrite(displays[display_num], HIGH);
  }
}

void clearVolumeBuffer() {
  for (int z = 0; z < Z_RES; z++) {
    for (int y = 0; y < Y_RES; y++) {
      for (int x = 0; x < X_RES; x++) {
        volumeBuffer[x][y][z] = 0;
      }
    }
  }
}

void clearDisplayZ(int d_num) {
  digitalWrite(displays[d_num], LOW);
  oled.clear();
  digitalWrite(displays[d_num], HIGH);
}

void startDrawDisplay(int d_num) {
  digitalWrite(displays[d_num], LOW);
  oled.clearBuffer();
}

void endDrawDisplay(int d_num) {
  oled.sendBuffer();
  digitalWrite(displays[d_num], HIGH);
}


void drawCircleDisplay(int x_center, int y_center, float r, int d_num) {
  for (int angle = 0; angle < 360; angle++) {
    float angle_circle = (angle * 0.01745331);      // angle expressed in radians - 1 degree = 0,01745331 radians
    float x_edge = (x_center + (r * cos(angle_circle)));
    float y_edge = (y_center + (r * sin(angle_circle)));
    int x = int(x_edge);
    int y = int(y_edge);
    volumeBuffer[x][y][d_num] = 1;
  }
}

void drawRecDisplay(int x_center, int y_center, float lx, float ly, int d_num) {
  int rangex = int(lx);
  int rangey = int(ly);
  for (int x = (-rangex); x < (rangex + 1); x++) {
    volumeBuffer[x_center - x][y_center + rangey][d_num] = 1;
    volumeBuffer[x_center - x][y_center - rangey][d_num] = 1;
  }
  for (int y = (-rangey); y < (rangey + 1); y++) {
    volumeBuffer[x_center + rangex][y_center - y][d_num] = 1;
    volumeBuffer[x_center - rangex][y_center - y][d_num] = 1;
  }
}

void draw3DLine(int xs, int ys, int zs, int xe, int ye, int ze) {

}

void drawSphere(int xloc, int yloc, int zloc, int r) {
  int displayNum = r / 2;
  for (int z = 0; z < displayNum; z++) {
    for (int y = 0; y < Y_RES; y++) {
      for (int x = 0; x < X_RES; x++) {
      }
    }
  }
}

void drawWaveDisplay(int center, int range, int d_num) {
  int points[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (int point_num = 0; point_num < 9; point_num++) {
    points[point_num] = center + random(-range, range);
    points[point_num] = constrain(points[point_num], 0, 55);
    //Serial.println(points[point_num]);
  }
  int near_point = 0;
  int wave = points[0];
  int lastwave = points[0];
  volumeBuffer[0][wave][d_num] = 1;
  // Serial.println("Test");
  for (int x_location = 0; x_location < 128; x_location++) {
    if (x_location > 15) {
      int modulo = (x_location) % 16;
      //      Serial.print(modulo);
      //      Serial.print(", ");
      if (modulo == 0) {
        near_point++;
      }
    }
    else {

    }
    wave = float(0.1) * points[near_point] + float(0.9) * lastwave;
    //    Serial.print(near_point);
    //    Serial.print(", ");
    //    Serial.print(x_location);
    //    Serial.print(", ");
    //Serial.println(wave);
    lastwave = wave;
    volumeBuffer[x_location][wave][d_num] = 1;
  }
}

void drawRandomFill() {
  randomSeed(analogRead(A6));
  Serial.println("Randomly selecting pixels");
  for (int pixels = 0; pixels < 1000; pixels++) {
    int xpix = random(X_RES);
    int ypix = random(Y_RES);
    int zpix = random(Z_RES);
    volumeBuffer[xpix][ypix][zpix] = 1;
  }
}

void drawSH(void)
{
  uint8_t mdy = 0;
  if ( oled.getDisplayHeight() < 59 )
    mdy = 5;
  oled.drawXBM(0, 0, SH_width, SH_height, SH);
}

void tick() {
  digitalWrite(PIEZO, HIGH);
  delayMicroseconds(200);
  digitalWrite(PIEZO, LOW);
}

void rotate_point(int cx, int cy, float angle, int &px, int &py, int trisize)
{
  float fpx = float(cx + trisize);
  float fpy = float(cy + trisize);
  float s = sin(angle);
  float c = cos(angle);

  // translate point back to origin:
  fpx -= float(cx);
  fpy -= float(cy);

  // rotate point
  float xnew = fpx * c - fpy * s;
  float ynew = fpx * s + fpy * c;

  // translate point back:
  fpx = xnew + float(cx);
  fpy = ynew + float(cy);
  px = int(fpx);
  py = int(fpy);

}
