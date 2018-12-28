const int leftEndStop = 5;
const int rightEndStop = 6;
const int horDirPin = 4;
const int horStepPin = 3;
const int upperEndStop = 11;
const int lowerEndStop = 10;
const int verDirPin = 8; 
const int verStepPin = 9;

const int dirLeft = 100;
const int dirRight = 200;
const int dirDown = 300;
const int dirUp = 400;

const int stateInit = 0;
const int stateLeft = 100;
const int stateRight = 200;
const int stateWaitLeft = 300;
const int stateWaitRight = 400;
const int stateDown = 500;
const int stateUp = 600;

const int actIdle = 0;
const int actMoveLeft = 100;
const int actMoveRight = 200;
const int actWaitRight = 300;
const int actWaitLeft = 400;
const int actMoveDown = 500;
const int actMoveUp = 600;


class StateMachine {

  public:
    int currentState = stateInit;
    int nextState = stateLeft;
    int prevState;
    bool leftEndStopState;
    bool rightEndStopState;
    bool upperEndStopState;
    bool lowerEndStopState;

    // Variables for waiting
    bool isWaiting = false;
    unsigned long restTimeStart;
    unsigned long restTime;

    StateMachine() {
      leftEndStopState = LOW;
      leftEndStopState = LOW;
    }

    void readInput() {
      leftEndStopState = digitalRead(leftEndStop);
      rightEndStopState = digitalRead(rightEndStop);
      upperEndStopState = digitalRead(upperEndStop);
      lowerEndStopState = digitalRead(lowerEndStop);
    }

    void horMovement(int moveDir){
      if (moveDir == dirLeft) {
        digitalWrite(horDirPin, HIGH);
      } else {
        digitalWrite(horDirPin, LOW);
      }
      if ((moveDir == dirLeft) && (leftEndStopState == LOW)) {
        return;
      } else if ((moveDir == dirRight) && (rightEndStopState == LOW)) {
        return;
      } else {
        digitalWrite(horStepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(horStepPin, LOW);
        delayMicroseconds(500);
      }
    }
    void verMovement(int moveDir){
      if (moveDir == dirDown) {
        digitalWrite(verDirPin, HIGH);
      } else {
        digitalWrite(verDirPin, LOW);
      }
      if ((moveDir == dirDown) && (lowerEndStopState == LOW)) {
        return;
      } else if ((moveDir == dirUp) && (upperEndStopState == LOW)) {
        return;
      } else {
        digitalWrite(verStepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(verStepPin, LOW);
        delayMicroseconds(500);
      }
    }

    void restStart(int waitTime) {
      restTimeStart = millis();
      isWaiting = true;
      restTime = waitTime;
    }

    boolean restFinish() {
      unsigned long timeDiff = millis() - restTimeStart;
      Serial.println(timeDiff);
      if (timeDiff > restTime) {
        return true;
      } else {
        return false;
      }
    }

    int nextAction() {
      if (currentState == stateInit){
        Serial.println("A");
        if ((nextState == stateLeft) && (currentState != stateLeft) && (leftEndStopState == HIGH)){
          return actMoveLeft;
        } else {
          prevState = currentState;
          currentState = stateLeft;
          nextState = stateRight;
          return actMoveRight;
        }
      }
      if (currentState == stateLeft){
        Serial.println("B");
        if ((nextState == stateRight) && (currentState != stateRight) && (leftEndStopState == HIGH)){
          return actMoveLeft;
        } else if((nextState == stateDown) && (currentState != stateDown) && (leftEndStopState == HIGH)){
          return actMoveLeft;
        } else {
          prevState = currentState;
          if (nextState == stateRight) {
            currentState = stateRight;
            nextState = stateDown;
            return actMoveRight;
          } else if (nextState == stateDown) {
            currentState = stateDown;
            nextState = stateUp;
            return actMoveDown;
          }
        }
      }
      if (currentState == stateRight){
        Serial.println("C");
        if ((nextState == stateDown) && (currentState != stateDown) && (rightEndStopState == HIGH)){
          return actMoveRight;
        } else {
          prevState = currentState;
          currentState = stateDown;
          nextState = stateUp;
          return actMoveDown;
        }
      }
      if (currentState == stateDown){
        Serial.println("D");
        if ((nextState == stateUp) && (currentState != stateUp) && (lowerEndStopState == HIGH)){
          return actMoveDown;
        } else {
          if (prevState == stateRight){
            nextState = stateLeft;
          } else if (prevState == stateLeft){
            nextState = stateRight;
          }
          prevState = currentState;
          currentState = stateUp;
          return actMoveUp;
        }
      }
      if (currentState == stateUp){
        Serial.println("E");
        if (((nextState == stateLeft) || (nextState == stateRight)) && ((currentState != stateLeft) || (currentState != stateRight)) && (upperEndStopState == HIGH)){
          return actMoveUp;
        } else {
          prevState = currentState;
          if (nextState == stateLeft){
            currentState = stateLeft;
            nextState = stateDown;
            return actMoveLeft;
          } else if (nextState == stateRight);
            currentState = stateRight;
            nextState = stateDown;
            return actMoveRight;
        }
      }
    }
};
void setup(){
  pinMode(leftEndStop, INPUT);
  pinMode(rightEndStop, INPUT);
  pinMode(horDirPin, OUTPUT);
  pinMode(horStepPin, OUTPUT);
  pinMode(upperEndStop, INPUT);
  pinMode(lowerEndStop, INPUT);
  pinMode(verDirPin, OUTPUT);
  pinMode(verStepPin, OUTPUT);
  Serial.begin(115200);
}

StateMachine stateEngine;

void loop(){
  stateEngine.readInput();
  int nextAction = stateEngine.nextAction();
  Serial.println(nextAction);
  switch(nextAction){
    case (actMoveLeft):
      stateEngine.horMovement(dirLeft);
      break;
    case (actMoveRight):
      stateEngine.horMovement(dirRight);
      break;
    case (actMoveDown):
      stateEngine.verMovement(dirDown);
      break;
    case (actMoveUp):
      stateEngine.verMovement(dirUp);
      break; 
    default:
      0;
  }
}
