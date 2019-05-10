#ifndef __TIMER5_H
#define __TIMER5_H
#include "sys.h"
extern uint8_t js_accel_x_status[5];
extern uint8_t js_accel_y_status[5];
extern uint8_t js_accel_z_status[5];

extern uint8_t js_gyro_x_status[5];
extern uint8_t js_gyro_y_status[5];
extern uint8_t js_gyro_z_status[5];

extern uint8_t js_mag_x_status[5];
extern uint8_t js_mag_y_status[5];
extern uint8_t js_mag_z_status[5];
extern short mag_x_middle ;
extern short mag_y_middle ;


void TIM5_Init(u16 arr,u16 psc);

#endif
