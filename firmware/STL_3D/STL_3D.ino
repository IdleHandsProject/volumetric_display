#include <Arduino.h>
#include <math.h>
#include "meshes/mesh_cube.h"
#include "type.h"


#include <U8g2lib.h>
#define PRES             16384
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif





int displays [10] = {A0, A1, A2, A3, A4, A5, 10, 9, 6, 5};

//U8G2_SSD1306_64X48_ER_F_4W_SW_SPI oled(U8G2_R0, 13, 12, 9, 10, 11);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled(U8G2_R0, /* cs=*/ 4, /* dc=*/ 11, /* reset=*/ 13);

#define num_particles 10
#define X_MAX 12800
#define Y_MAX 6400
#define Z_MAX 10000
#define VEL_MAX 25500

#define PIEZO 4

int axis_max[3] = {12700, 5600, 9000};
int boundary[3] = {300,300,0};





// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================



void setup() {
  
  for (int display_pin = 0; display_pin < 10; display_pin++) {
    pinMode (displays[display_pin], OUTPUT);
    digitalWrite(displays[display_pin], LOW);
  }
  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(115200);
  oled.begin();
  oled.setFlipMode(0);
  u8g2_prepare();
}

void loop() {
  for (int display_num = 0; display_num < 10; display_num++) {
    digitalWrite(displays[display_num], LOW);
    oled.clearBuffer();

    for (int i = 0; i < TRICOUNT; i++) {
        if (faces[i][2] == display_num){
          oled.drawDisc(faces[i][0]*5, faces[i][1]*5, 1, U8G2_DRAW_ALL);
        }
    }
    oled.sendBuffer();
    //delay(40);
    //oled.clear();
    digitalWrite(displays[display_num], HIGH);

  }
}


void u8g2_prepare(void) {
  oled.setFont(u8g2_font_6x10_tf);
  oled.setFontRefHeightExtendedText();
  oled.setDrawColor(1);
  oled.setFontPosTop();
  oled.setFontDirection(0);
}
