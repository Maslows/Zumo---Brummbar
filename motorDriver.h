#include "MKL46Z4.h"



//////////////////////////////////////// Definitions and settings ////////////////////////////////////////



#define FORWARD 0
#define REVERSE 1

#define RIGHT_PHASE_PIN (1uL<<9)
#define LEFT_PHASE_PIN (1uL<<13)

#define V_MOD 1023 // 1023 to ma duzy
#define PRIORITY_INTERRUPT_DRIVE 2

// distance of one teeth in 0.1 mm -> set 50 = 5 mm
// approximation error is 0.04 % -> 1 mm on 10 m
#define ONE_TEETH_DISTANCE 50


// select encoders for functionalities
// 3 left engine encoder
// 5 right engine encoder
// BE CAREFUL! FOR FUNCTIONALITIES TO WORK PROPERLY ONE ENCODER CANNOT BE USED TWICE.

// 'drive way and turning' functionality
#define DRIVE_AND_TURN_COUNTING_ENCODER 3
// 'count distance' functionality
#define DISTANCE_COUNTING_ENCODER 5


// default turning speed
#define DEFAULT_TURNING_SPEED 40

extern uint8_t speed;

//////////////////////////////////////// Functions ////////////////////////////////////////



// driver initialization function

void motorDriverInit(void);


// timer's interrupt handler

void TPM0_IRQnHandler(void);


// "set" functions for direct, low-level control

void setLeftTrackDir(int direction);
void setLeftTrackSpeed(int speed);

void setRightTrackDir(int direction);
void setRightTrackSpeed(int speed);

void setTracksDir(int leftDir, int rightDir);
void setTracksSpeed(int leftSpeed, int rightSpeed);


// "drive" functions for indirect, high-level control

void driveForward(int speed);
void driveReverse(int speed);
void driveStop(void);

void driveForwardDist(int speed, int distance);
void driveReverseDist(int speed, int distance);

void driveAccelerate(int howMuch);

// "turn" functions for turning; using compass and encoder

void turnLeftC(int degrees);
void turnRightC(int degrees);

void turnLeftE(int degrees);
void turnRightE(int degrees);


// encoder control functions

void countDistanceEnable(void);
void countDistanceDisable(void);
uint32_t getDistance(void);


// wait tool

void wait4(void);
