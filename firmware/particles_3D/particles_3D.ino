
#include <math.h>

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//U8G2_SSD1306_64X48_ER_F_4W_SW_SPI oled(U8G2_R0, 13, 12, 9, 10, 11);
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI oled(U8G2_R0, /* cs=*/ A0, /* dc=*/ 11, /* reset=*/ 13);

#define num_particles 10
#define X_MAX 12800
#define Y_MAX 6400
#define Z_MAX 10000
#define VEL_MAX 25500

int axis_max[3] = {12800, 6400, 10000};

int particle_pos[10][3] =
{ {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

int prev_partical_pos[10][3] =
{ {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

int particle_vel[10][3] =
{ {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};


void setup() {
  for (int i = 0; i < num_particles; i ++) {
    int x_vel = random(-255, 255);
    int y_vel = random(-255, 255);
    int z_vel = random(-255, 255);
    particle_vel[i][0] = x_vel;
    particle_vel[i][1] = y_vel;
    particle_vel[i][2] = z_vel;
  }
  Serial.begin(115200);
  oled.begin();
  oled.setFlipMode(0);
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("===================");
//  for (int i = 0; i < num_particles; i++) {
//    update_pos_vel(i);
//    draw_particles(i);
//  }
//  Serial.println("===================");
  oled.firstPage();
  do {
    for (int i = 0; i < num_particles; i++) {
      update_pos_vel(i);
      draw_particles(i);
    }
  } while ( oled.nextPage() );
}

void update_pos_vel(int particle_num) {
  for (int axis = 0; axis < 3; axis ++) {
    particle_pos[particle_num][axis] += particle_vel[particle_num][axis];
    if (particle_pos[particle_num][axis] > axis_max[axis] || particle_pos[particle_num][axis] < 0) {
      particle_vel[particle_num][axis] *= -1;
    }
  }
}

void draw_particles(int particle_num) {
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

  oled.drawCircle(pixel[0], pixel[1], pixel[2]+1, U8G2_DRAW_ALL);
  Serial.print(pixel[0]);
  Serial.print(",");
  Serial.print(pixel[1]);
  Serial.print(",");
  Serial.print(pixel[2]);
  Serial.print(",");
  Serial.print(particle_vel[particle_num][0]);
  Serial.print(",");
  Serial.print(particle_vel[particle_num][1]);
  Serial.print(",");
  Serial.println(particle_vel[particle_num][2]);

}
