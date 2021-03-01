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
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

#define OUTPUT_READABLE_ACCELGYRO

int displays [10] = {A0, A1, A2, A3, A4, A5, 10, 9, 6, 5};

bool volumeBuffer[128][56][10] = {0};

U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled(U8G2_MIRROR, /* cs=*/ -1, /* dc=*/ 11, /* reset=*/ 13);

#define X_MAX 12700
#define Y_MAX 5600
#define Z_MAX 9000

int ball_xyz[3] = {64000, 25000, 5000};
int max_xyz[3] = {127000, 55000, 9000};
int vel_xyz[3] = {0, 0, 0};
int ball_r = 8000;
float resitution = 1.3;

#define PIEZO 1



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

void setup() {


#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif


  Serial.begin(115200);
  //while(!Serial);

  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  initializeAllDisplays();
  oled_prepare();
}

void loop() {
  //  Uncomment to use accelerometer data
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);



  int acc[3] = {0, 0, 0};
  acc[0] = -ay/acc_SCALE;
  acc[1] = az/acc_SCALE;
  acc[2] = -ax/acc_SCALE;
   //  Serial.print(acc[0]);
  //  Serial.print(", ");
  //  Serial.print(acc[1]);
  //  Serial.print(", ");
  //  Serial.println(acc[2]);
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
//  Serial.print(", ");
//  Serial.print(scaled_xyz[0]);
//  Serial.print(", ");
//  Serial.print(scaled_xyz[1]);
//  Serial.print(", ");
//  Serial.println(scaled_xyz[2]);

  drawCircleDisplay(scaled_xyz[0], scaled_xyz[1], ball_r / 1000, scaled_xyz[2]);
  for (int i = 0; i < 10; i++) {
    drawRecDisplay(64, 27, 63, 27,  9 - i);
  }


  drawVolumeBuffer();
  //Serial.println(millis()-stime);
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
