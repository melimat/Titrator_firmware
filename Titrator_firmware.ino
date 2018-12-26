const int stepPinHorizontal = 3;
const int dirPinHorizontal = 4;
const int endStopLeft = 5;
const int endStopRight = 6;

const int stepPinVertical = 9;
const int dirPinVertical = 8;
const int endStopVerticalUpper = 11;
const int endStopVerticalLower = 10;



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
const int directionDown = 3;
const int directionUp = 4;

const int actIdle = 0;
const int actMoveLeft = 100;
const int actMoveRight = 200;
const int actWaitLeft = 300;
const int actWaitRight = 400;
const int actMoveUp = 500;
const int actMoveDown = 600;
const int actWaitDown = 700;
const int actWaitUp = 800;

void horMovement(int moveDir, int dirPin, int stepPin) {
  if ((moveDir == directionLeft) || (moveDir == directionUp)) {
    digitalWrite(dirPin, HIGH);
  } else {
    digitalWrite(dirPin, LOW);
  }
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(500);
}

void moveLeft() {
  horMovement(directionLeft, dirPinHorizontal, stepPinHorizontal);
}

void moveRight() {
  horMovement(directionRight, dirPinHorizontal, stepPinHorizontal);
}

void moveDown() {
  horMovement(directionDown, dirPinVertical, stepPinVertical);
}

void moveUp() {
  horMovement(directionUp, dirPinVertical, stepPinVertical);
}

class StateMachine {
  public:
    bool endStopLeftState;
    bool endStopRightState;
    bool endStopVerticalUpperState;
    bool endStopVerticalLowerState;
    bool prevEndStopLeftState = HIGH;
    bool prevEndStopRightState = LOW;
    bool prevEndStopVerticalUpperState = HIGH;
    bool prevEndStopVerticalLowerState = LOW;
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
      endStopVerticalUpperState = digitalRead(endStopVerticalUpper);
      endStopVerticalLowerState = digitalRead(endStopVerticalLower);
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
            nextState = stateDown;
            dbgstate();
            return actIdle;
          } else {
            return actIdle;
          }
        }
      }

      if (currentState == stateWaitRight) {
        Serial.println("E");
        if ((nextState == stateDown) && (currentState != stateDown) && (endStopVerticalUpperState == HIGH)) {
          return actMoveDown;
        } else {
          Serial.println("nic");
          currentState = stateDown;
          nextState = stateWaitDown;
          dbgstate();
          return actWaitDown;
        }
      }

      if (currentState == stateDown) {
        Serial.println("F");
        if ((nextState == stateWaitDown) && (currentState != stateWaitDown) && (isWaiting == false)) {
          Serial.println("1");
          return actWaitDown;
        } else if ((nextState == stateWaitDown) && (currentState != stateWaitDown) && (isWaiting == true)) {
          Serial.println("2");
          if (restFinish() == true) {
            Serial.println("3");
            isWaiting = false;
            currentState = stateWaitDown;
            nextState = stateUp;
            dbgstate();
            return actIdle;
          } else {
            Serial.println("4");
            return actIdle;
          }
        }
      }
      if (currentState == stateWaitDown) {
        Serial.println("G");
        if ((nextState == stateUp) && (currentState != stateUp) && (endStopVerticalLowerState == HIGH)) {
          return actMoveUp;
          Serial.println("1");
        } else {
          Serial.println("2");
          currentState = stateUp;
          nextState = stateWaitUp;
          dbgstate();
          return actWaitUp;;
        }
      }
      if (currentState == stateUp) {
        Serial.println("H");
        if ((nextState == stateWaitUp) && (currentState != stateWaitUp) && (isWaiting == false)) {
          Serial.println("1");
          return actWaitUp;
      } else if ((nextState == stateWaitUp) && (currentState != stateWaitUp) && (isWaiting == true)) {
        Serial.println("2");
        if (restFinish() == true) {
            isWaiting == false;
            currentState = stateWaitUp;
            nextState = stateLeft;
            dbgstate();
            return actIdle;
          } else {
            Serial.println("3");
            return actIdle;
          }
        }
      }
      if (currentState == stateWaitUp){
        if ((nextState == stateLeft) && (currentState != stateLeft) && (endStopVerticalUpperState = HIGH)){
          return actMoveLeft;
        } else {
          currentState == stateLeft;
          nextState == stateWaitLeft;
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
  pinMode(stepPinVertical, OUTPUT);
  pinMode(dirPinVertical, OUTPUT);
  pinMode(endStopVerticalUpper, INPUT);
  pinMode(endStopVerticalLower, INPUT);
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
    case (actMoveDown):
      moveDown();
      break;
    case (actWaitDown):
      stateEngine.restStart(5000);
      break;
    case (actMoveUp):
      moveUp();
      break;
    case (actWaitUp):
      stateEngine.restStart(5000);
      break;
    case (actWaitRight):
      stateEngine.restStart(10000);
      break;
    default:
      0;
  }
}


