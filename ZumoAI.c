#include "osObjects.h"
#include "sonar.h"
#include "servo.h"
#include "motors.h"

osThreadId tid_zumoAI;

uint16_t EnemyDistance;
int32_t EnemyAngle;

typedef enum {
  FIND_ENEMY,
  TURN_TOWARD_ENEMY,
  RAM
} Phase_t;



int Init_ZumoAI(void) {
  tid_zumoAI = osThreadCreate(osThread(zumoAI), NULL);
  if(!tid_zumoAI) return(-1);
  SendMessage("ZumoAI: ZumoAI online.\n"); 
  return(0);
}

void Kill_ZumoAI(void){
  osThreadTerminate(tid_zumoAI);
  tid_zumoAI = 0;
  driveStop();
  ServoChangeMode(MANUAL);
  ServoMoveByDegree(0);
  
  SendMessage("ZumoAI: Process terminated.\n");
}



Phase_t FindEnemy(void){
osEvent Evt; 
  while(ServoState != LOCKED){
    SendMessage("ZumoAI: Searching for enemy.\n");
  
    /* Wait for sonar lock */
    osSignalClear(tid_zumoAI,SIG_ENEMY_LOCK_ON);
    osSignalClear(tid_zumoAI,SIG_SWEEP_COMPLETE);
    
    Evt = osSignalWait(0,osWaitForever);
    if(Evt.value.signals & SIG_ENEMY_LOCK_ON){
      EnemyDistance = lastDistance;
      EnemyAngle = ServoPosition; 
      return TURN_TOWARD_ENEMY;
    }
    if(Evt.value.signals & SIG_SWEEP_COMPLETE){
      osDelay(20);
      rotate(90);
    }
  } 
  return TURN_TOWARD_ENEMY;
}

Phase_t TurnTowardEnemy(void){
  SendMessage("ZumoAI: Enemy found!\n");
  
  if(EnemyAngle > 15 || EnemyAngle < -15){
    SendMessage("ZumoAI: Enemy at %d. Rotating...\n",EnemyAngle);
    osDelay(20);
    rotate(EnemyAngle);
    ServoSweepDir = (EnemyAngle>0) ? SWEEP_LEFT : SWEEP_RIGHT;
    
    osSignalClear(tid_zumoAI,SIG_MOVE_COMPLETE);
    osSignalWait(SIG_MOVE_COMPLETE,osWaitForever);
//    
//    if (ServoState != LOCKED){
//      SendMessage("ZumoAI: Enemy lost after rotation.\n");
//      return FIND_ENEMY;
//    }
  }
  return RAM;
}

Phase_t Ram(void){
  osEvent Evt;
  while(1){
    osDelay(20);
    drive(40,FORWARD,lastDistance);
    SendMessage("ZumoAI: Rammming!\n");
    osSignalClear(tid_zumoAI,SIG_ENEMY_LOCK_LOST);
    Evt = osSignalWait(SIG_ENEMY_LOCK_LOST,2000);
    if (Evt.status != osEventTimeout){
      return FIND_ENEMY;
    }
    EnemyDistance = lastDistance;
  }
}

void zumoAI(void const *argument){
  Phase_t ZumoNextPhase = FIND_ENEMY;
  ServoChangeSweepMode(SCAN_AND_LOCK);
  ServoChangeMode(SWEEP);

  /* Main Loop */
  while(1){
    switch(ZumoNextPhase){
      case FIND_ENEMY:
        ZumoNextPhase = FindEnemy();
        break;
      case TURN_TOWARD_ENEMY:
        ZumoNextPhase = TurnTowardEnemy();
        break;
      case RAM:
        ZumoNextPhase = Ram();
        break;
    }       
  }
}

