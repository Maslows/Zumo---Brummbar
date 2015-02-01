#ifndef MOTORS_H
#define MOTORS_H

#include "MKL46Z4.h"                    // Device header

#define MOTORS_INTERUPT_PRIORITY 2

/**
  @brief Define distance travelled per encoder event.
*/
#define TEETH_DISTANCE_MM 5

/**
  @brief Define the perimiter of a wheel.
*/
#define M_PI 3.141592f
#define WHEEL_PERIMITER_MM 119 //0.11938052083641214m 
#define DISTANCE_BETWEEN_TRACKS_MM 95  //mm
#define ZUMO_TURN_PERIMITER_MM (M_PI*DISTANCE_BETWEEN_TRACKS_MM)
#define DEFAULT_TURNING_SPEED (30)

#define RIGHT_PHASE_PIN (9)
#define RIGHT_PHASE_PIN_MASK (1 << RIGHT_PHASE_PIN)
#define RIGHT_PHASE_PORT (PORTC)
#define RIGHT_PHASE_FPORT (FPTC)      
                                 
#define LEFT_PHASE_PIN (13)
#define LEFT_PHASE_PIN_MASK (1 << LEFT_PHASE_PIN)
#define LEFT_PHASE_PORT (PORTA)
#define LEFT_PHASE_FPORT (FPTA)

#define LEFT_PWM_PIN (4)
#define LEFT_PWM_PIN_MASK (1 << LEFT_PWM_PIN)
#define LEFT_PWM_PORT (PORTD)

#define RIGHT_PWM_PIN (2)
#define RIGHT_PWM_PIN_MASK (1 << RIGHT_PWM_PIN)
#define RIGHT_PWM_PORT (PORTD)

#define LEFT_A_ENCODER_PIN (6)
#define LEFT_A_ENCODER_MASK (1 << LEFT_A_ENCODER_PIN)
#define LEFT_A_ENCODER_PORT PORTA
#define LEFT_A_ENCODER_FPORT (FPTA)

#define RIGHT_A_ENCODER_PIN (5)
#define RIGHT_A_ENCODER_MASK (1 << RIGHT_A_ENCODER_PIN)
#define RIGHT_A_ENCODER_PORT PORTD
#define RIGHT_A_ENCODER_FPORT (FPTD)

typedef enum {FORWARD,REVERSE} direction_t;
typedef enum {LEFT = 0x1,
              RIGHT = 0x2,
              BOTH = 0x3} track_t;
extern uint8_t speed;


void motors_init(void);

/* Basic controls */
void drive(uint8_t speed, direction_t direction);
void driveReverse(uint8_t speed);
void driveStop(void);
void turnLeft(void);
void turnRight(void);

/* Functions requiring encoders */
void rotate(int angle);


#endif
