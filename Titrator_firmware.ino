const int stepPinHorizontal = 3;
const int dirPinHorizontal = 4;
const int endStopLeft = 5;
const int endStopRight = 6;

const int stepPinVertical = 9;
const int dirPinVertical = 8;
const int endStopVerticalUpper = 10;
const int endStopVerticalLower = 11;



const int stateInit = 0;
const int stateLeft = 100;
const int stateRight = 200;
const int stateWaitLeft = 300;
const int stateWaitRight = 400;
const int stateUp = 500;
const int stateDown = 600;
const int stateWaitUp = 700;
const int stateWaitDown = 800;

const int directionLeft = 1;
const int directionRight = 2;

const int actIdle = 0;
const int actMoveLeft = 100;
const int actMoveRight = 200;
const int actWaitLeft = 300;
const int actWaitRight = 400;
const int actMoveUp = 500;
const int actMoveDown = 600;
const int actWaitDown = 700;
const int actWaitUp = 800;

void horMovement(int moveDir) {
  if (moveDir == directionLeft) {
    digitalWrite(dirPinHorizontal, HIGH);
  } else {
    digitalWrite(dirPinHorizontal, LOW);
  }
  digitalWrite(stepPinHorizontal, HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPinHorizontal, LOW);
  delayMicroseconds(500);
}

void moveLeft() {
  horMovement(directionLeft);
}

void moveRight() {
  horMovement(directionRight);
}

class StateMachine {
  public:
    bool endStopLeftState;
    bool endStopRightState;
    bool prevEndStopLeftState = HIGH;
    bool prevEndStopRightState = LOW;
    bool isWaiting = false;
    int currentState = stateInit;
    int nextState = stateLeft;
    unsigned long restTimeStart;
    unsigned long restTime;

    int horizontalDirection = 1;

    StateMachine() {
      endStopLeftState = HIGH;
      endStopRightState = HIGH;
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

    void readInput() {
      //Serial.println("Read input");
      endStopLeftState = digitalRead(endStopLeft);
      endStopRightState = digitalRead(endStopRight);
    /*
      if (endStopLeftState == LOW) {
        currentState = stateLeft;
      }
      else if (endStopRightState == LOW) {
        currentState = stateRight;
      }
*/
      String current = endStopLeftState + " - " + endStopRightState;
      //Serial.println(current);
    }

    void dbgstate() {
      Serial.print(currentState);
      Serial.print(" ==== ");
      Serial.print(nextState);  
    }
    
    int nextAction() {
      if (currentState == stateInit) {
        Serial.println("A");
        if ((nextState == stateLeft) && (currentState != stateLeft) && (endStopLeftState == HIGH)) {
          return actMoveLeft;
        } else {
          currentState = stateLeft;
          nextState = stateWaitLeft;
          dbgstate();
          return actIdle;
        }
      }
      if (currentState == stateLeft) { 
        Serial.println("B");
        if ((nextState == stateWaitLeft) && (currentState != stateWaitLeft) && (isWaiting == false)) {
          Serial.println("1");
          return actWaitLeft;
        } else if ((nextState == stateWaitLeft) && (currentState != stateWaitLeft) && (isWaiting == true)) {
          Serial.println("2");
          if (restFinish() == true) {
            Serial.println("3");
            isWaiting = false;
            currentState = stateWaitLeft;
            nextState = stateRight;
            dbgstate();
            return actIdle;
          } else {
            Serial.println("4");
            return actIdle;
          }
        } 
      }

      if (currentState == stateWaitLeft) { 
        Serial.println("C");
        if ((nextState == stateRight) && (currentState != stateRight) && (endStopRightState == HIGH)) {
          return actMoveRight;
        } else {
          currentState = stateRight;
          nextState = stateWaitRight;
          dbgstate();
          return actWaitRight;
        }
      }
      
      if (currentState == stateRight) { 
        Serial.println("D");
        if ((nextState == stateWaitRight) && (currentState != stateWaitRight) && (isWaiting == false)) {
          return actWaitRight;
        } else if ((nextState == stateWaitRight) && (currentState != stateWaitRight) && (isWaiting == true)) {
          if (restFinish() == true) {
            isWaiting = false;
            currentState = stateWaitRight;
            nextState = stateLeft;
            dbgstate();
            return actIdle;
          } else {
            return actIdle;
          }
        }
      }  

      if (currentState == stateWaitRight) {
        Serial.println("E");
        if ((nextState == stateLeft) && (currentState != stateLeft) && (endStopLeftState == HIGH)) {
          return actMoveLeft;
        } else {
          currentState = stateLeft;
          nextState = stateWaitLeft;
          dbgstate();
          return actIdle;
        }
      }

    }
};

StateMachine stateEngine;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(stepPinHorizontal, OUTPUT);
  pinMode(dirPinHorizontal, OUTPUT);
  pinMode(endStopLeft, INPUT);
  pinMode(endStopRight, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  stateEngine.readInput();

  int nextAction = stateEngine.nextAction();
  String state = stateEngine.currentState + " -> " + stateEngine.nextState;
  Serial.print(stateEngine.currentState);
  Serial.print(" -> ");
  Serial.print(stateEngine.nextState);
  Serial.print(" : ");
  Serial.println(nextAction);

  //delay(500);

  switch (nextAction) {
    case (actMoveLeft):
      moveLeft();
      break;
    case (actMoveRight):
      moveRight();
      break;
    case (actWaitLeft):
      stateEngine.restStart(5000);
      break;
    case (actWaitRight):
      stateEngine.restStart(10000);
      break;
    default:
      0;
  }
}


