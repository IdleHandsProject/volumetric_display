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

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

#define OUTPUT_READABLE_ACCELGYRO

int displays [10] = {A0, A1, A2, A3, A4, A5, 10, 9, 6, 5};

//U8G2_SSD1306_64X48_ER_F_4W_SW_SPI oled(U8G2_R0, 13, 12, 9, 10, 11);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled(U8G2_MIRROR, /* cs=*/ 4, /* dc=*/ 11, /* reset=*/ 13);

#define num_particles 50
#define X_MAX 12800
#define Y_MAX 6400
#define Z_MAX 10000
#define VEL_MAX 25500

#define PIEZO 4

int axis_max[3] = {12700, 5600, 9000};
int boundary[3] = {300,300,0};

int particle_pos[num_particles][3];

//int particle_pos[10][3] =
//{ {100, 0, 10},
//  {200, 0, 10},
//  {300, 0, 10},
//  {400, 0, 10},
//  {500, 0, 10},
//  {600, 0, 10},
//  {700, 0, 10},
//  {800, 0, 10},
//  {900, 0, 10},
//  {1000, 0, 10}
//};
//
//int prev_partical_pos[10][3] =
//{ {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0}
//};
int particle_vel[num_particles][3];

//int particle_vel[10][3] =
//{ {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0}
//};

#define INITIAL_RANDOM_VELOCITY
#define ADD_ACCEL

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================



void setup() {

/
  
  for (int display_pin = 0; display_pin < 10; display_pin++) {
    pinMode (displays[display_pin], OUTPUT);
    digitalWrite(displays[display_pin], LOW);
  }
  for (int i = 0; i < num_particles; i ++) {
    #ifdef INITIAL_RANDOM_VELOCITY
        int x_vel = random(-100, 100);
        int y_vel = random(-100, 100);
        int z_vel = random(-100, 100);
        particle_vel[i][0] = x_vel;
        particle_vel[i][1] = y_vel;
        particle_vel[i][2] = z_vel;
#endif
    int x_pos = random(0, 12700);
    int y_pos = random(0, 6300);
    int z_pos = random(0, 10000);

    particle_pos[i][0] = x_pos;
    particle_pos[i][1] = y_pos;
    particle_pos[i][2] = z_pos;

  }


#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(115200);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  oled.begin();
  oled.setFlipMode(0);
  u8g2_prepare();
}

void loop() {

  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  int acc_x = ay / 10000;
  int acc_y = az / 10000;
  int acc_z = ax / 10000;

#ifdef OUTPUT_READABLE_ACCELGYRO
  // display tab-separated accel/gyro x/y/z values
  //  Serial.print("a/g:\t");
  //  Serial.print(acc_x); Serial.print("\t");
  //  Serial.print(acc_y); Serial.print("\t");
  //  Serial.println(acc_z);

#endif

  //oled.firstPage();
  //do {
  for (int display_num = 0; display_num < 10; display_num++) {
    digitalWrite(displays[display_num], LOW);
    oled.clearBuffer();
    oled.drawStr( 50, 30, "Hello World");
    //for (int i = 0; i < num_particles; i++) {
    //  update_pos_vel(i, acc_x, acc_y, acc_z);
    //  draw_particles(i, display_num);
    //  tick_sound();
    //}
    oled.sendBuffer();
    delay(40);
    oled.clear();
    digitalWrite(displays[display_num], HIGH);

  }
  for (int display_num = 9; display_num > 0; display_num--) {
    digitalWrite(displays[display_num], LOW);
    oled.clearBuffer();
    oled.drawStr( 50, 30, "Hello World");
    //for (int i = 0; i < num_particles; i++) {
    //  update_pos_vel(i, acc_x, acc_y, acc_z);
    //  draw_particles(i, display_num);
    //  tick_sound();
    //}
    oled.sendBuffer();
    delay(40);
    oled.clear();
    digitalWrite(displays[display_num], HIGH);

  }
  //  }
  //} while ( oled.nextPage() );
}


void u8g2_prepare(void) {
  oled.setFont(u8g2_font_6x10_tf);
  oled.setFontRefHeightExtendedText();
  oled.setDrawColor(1);
  oled.setFontPosTop();
  oled.setFontDirection(0);
}


void update_pos_vel(int particle_num, int acc_x, int acc_y, int acc_z) {
  int acc[3] = {0, 0, 0};
  acc[0] = acc_x;
  acc[1] = acc_y;
  acc[2] = acc_z;
  for (int axis = 0; axis < 3; axis ++) {
    //if(axis>1){
    #ifdef ADD_ACCEL
    particle_vel[particle_num][axis] += acc[axis];
    #endif
    
    //}
    particle_pos[particle_num][axis] += particle_vel[particle_num][axis];
    if (particle_pos[particle_num][axis] > (axis_max[axis]-boundary[axis]) || particle_pos[particle_num][axis] < 0) {
      if (particle_pos[particle_num][axis] > axis_max[axis]) {
        particle_pos[particle_num][axis] = axis_max[axis] - 100;
      }
      if (particle_pos[particle_num][axis] < 0) {
        particle_pos[particle_num][axis] = 100;
      }
      #ifdef ADD_ACCEL
      if (particle_vel[particle_num][axis] < 0) {
        particle_vel[particle_num][axis] += 10;
      }
      else {
        particle_vel[particle_num][axis] -= 10;
      }
      #endif
      particle_vel[particle_num][axis] *= -1;
      //tick_sound();
    }
  }
}

void draw_particles(int particle_num, int display_num) {
  int pixel[3] = {0, 0, 0};
  for (int axis = 0; axis < 3; axis ++) {
    pixel[axis] = float(particle_pos[particle_num][axis]) / 100;
    if (axis == 2) {
      pixel[axis] /= 10;
    }
    if (pixel[axis] > axis_max[axis]) {
      pixel[axis] = axis_max[axis];
    }
    if (pixel[axis] < 0) {
      pixel[axis] = 0;
    }
  }
  if (pixel[2] == (9-display_num)) {
    oled.drawDisc(pixel[0], pixel[1], 1, U8G2_DRAW_ALL);
  }
//  Serial.print(pixel[0]);
//  Serial.print(",");
//  Serial.print(pixel[1]);
//  Serial.print(",");
//  Serial.print(pixel[2]);
//  Serial.print(",");
//  Serial.print(particle_vel[particle_num][0]);
//  Serial.print(",");
//  Serial.print(particle_vel[particle_num][1]);
//  Serial.print(",");
//  Serial.println(particle_vel[particle_num][2]);

}



void tick_sound(){
  //tone(PIEZO, 50, 2);
  //digitalWrite(PIEZO, LOW);
}
