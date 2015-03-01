#include "osObjects.h"
#include "sonar.h"

#include "servo.h"
#include "motors.h"

osThreadId tid_zumoAI;           // ZumoAI thread id

uint16_t EnemyDistance;
int32_t EnemyAngle;

typedef enum {
  FIND_ENEMY,
  TURN_TOWARD_ENEMY,
  RAM
} Phase_t;

/**
  @brief Thread ZumoAI initialization
*/
int Init_ZumoAI(void) {
  tid_zumoAI = osThreadCreate(osThread(zumoAI), NULL);
  if(!tid_zumoAI) return(-1);
  SendMessage("ZumoAI: ZumoAI online.\n"); 
  return(0);
}

/**
  @brief Thread ZumoAI termination function
*/
void Kill_ZumoAI(void){
  osThreadTerminate(tid_zumoAI);      // terminate thread
  tid_zumoAI = 0;                     // thread id
  driveStop();
  ServoChangeMode(MANUAL);
  ServoMoveByDegree(0);
  
  SendMessage("ZumoAI: Process terminated.\n");
}

/**
  @brief Finding enemy phase
  This function checks if the servo is locked onto something.
  If sonar is not locked, make full sweep and rotate. 
  Wait for sonar lock.
  If there is lock, break the loop and save enemy position.
*/
Phase_t FindEnemy(void){
osEvent Evt;
  SendMessage("ZumoAI: Searching for enemy.\n");
  while(ServoState != LOCKED){
    
  
    /* Wait for sonar lock */
    osSignalClear(tid_zumoAI,SIG_ENEMY_LOCK_ON);
    osSignalClear(tid_zumoAI,SIG_SWEEP_COMPLETE);
    
    Evt = osSignalWait(0,osWaitForever);
    if(Evt.value.signals & SIG_ENEMY_LOCK_ON){
      break;
    }
    if(Evt.value.signals & SIG_SWEEP_COMPLETE){
      osDelay(20);
      rotate(120);
      SendMessage("ZumoAI: Enemy not found. Rotating.\n");
    }
  }
  EnemyDistance = lastDistance;
  EnemyAngle = ServoPosition;   
  return TURN_TOWARD_ENEMY;
}
/**
  @brief Turning toward the enemy
  If the enemy is found, rotate towards it.
*/
Phase_t TurnTowardEnemy(void){
  SendMessage("ZumoAI: Enemy found!\n");
  
  if(EnemyAngle > 15 || EnemyAngle < -15){
    SendMessage("ZumoAI: Enemy at %d. Rotating...\n",EnemyAngle);
    osDelay(20);
    rotate(EnemyAngle);
    ServoSweepDir = (EnemyAngle>0) ? SWEEP_LEFT : SWEEP_RIGHT;
    
    osSignalClear(tid_zumoAI,SIG_MOVE_COMPLETE);
    osSignalWait(SIG_MOVE_COMPLETE,osWaitForever);
    
    if (ServoState != LOCKED){
      SendMessage("ZumoAI: Enemy lost after rotation.\n");
      return FIND_ENEMY;
    }
  }
  return RAM;
}
/** 
  @brief Ram the enemy.
  Ram enemy by driving forward for given distace.
  Update distance toward enemy every 2 seconds if lock is not lost.
 */
Phase_t Ram(void){
  osEvent Evt;
  while(1){
    osDelay(20);
    drive(40,FORWARD,lastDistance);
		//drive(100,FORWARD,0); // uncomment this line to ram instead of stoping in front of the enemy. Comment out the line above.
    SendMessage("ZumoAI: Rammming!\n");
    osSignalClear(tid_zumoAI,SIG_ENEMY_LOCK_LOST);
    Evt = osSignalWait(SIG_ENEMY_LOCK_LOST,2000);
    if (Evt.status != osEventTimeout){
      return FIND_ENEMY;
    }
    EnemyDistance = lastDistance;
  }
}
/** 
  @brief ZumoAI thread main loop
*/
void zumoAI(void const *argument){
  Phase_t ZumoNextPhase = FIND_ENEMY;
  ServoChangeSweepMode(SCAN_AND_LOCK);
  ServoChangeMode(SWEEP);
	//osDelay(5000);
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

