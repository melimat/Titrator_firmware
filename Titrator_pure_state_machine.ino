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
const int stateUpperRight = 200;
const int stateLowerRight = 500;
const int stateUpperLeft = 600;
const int stateLowerLeft = 700;

const int actIdle = 0;
const int actMoveLeft = 100;
const int actMoveRight = 200;
const int actMoveDown = 500;
const int actMoveUp = 600;


class StateMachine {
  public:
    int currentState = stateInit;
    int nextState = stateUpperLeft;
    bool leftEndStopState;
    bool rightEndStopState;
    bool upperEndStopState;
    bool lowerEndStopState;

    StateMachine() {
      leftEndStopState = LOW;
      rightEndStopState = LOW;
      upperEndStopState = LOW;
      lowerEndStopState = LOW;
    }

    void readInput() {
      leftEndStopState = digitalRead(leftEndStop);
      rightEndStopState = digitalRead(rightEndStop);
      upperEndStopState = digitalRead(upperEndStop);
      lowerEndStopState = digitalRead(lowerEndStop);
    }

    void horMovement(int moveDir) {
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

    void verMovement(int moveDir) {
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

    int nextAction() {
      String verticalEndStops = "UpperEndStop: " + String(upperEndStopState) + " ; LowerEndStop: " + String(lowerEndStopState) + "\n";
      String horizontalEndStops = "LeftEndStop: " + String(leftEndStopState) + " ;  RightEndStop: " + String(rightEndStopState) + "\n";
      String states = "Current state: " + String(currentState) + " ; Next state: " + String(nextState) + "\n";
      String logString = verticalEndStops + horizontalEndStops + states + "\n";
      Serial.println(logString);
      
      if (currentState == stateInit) {
        //Serial.println("A");
        if ((nextState == stateUpperLeft) && (currentState != stateUpperLeft) && (leftEndStopState == HIGH)) {
          return actMoveLeft;
        } else {
          currentState = stateUpperLeft;
          nextState = stateLowerLeft;
          return actMoveDown;
        }
      }
      if (currentState == stateUpperLeft) {
        //Serial.println("B");
        if ((nextState == stateLowerLeft) && (currentState != stateLowerLeft) && (leftEndStopState == LOW) && (lowerEndStopState == HIGH)) {
          return actMoveDown;
        } if ((nextState == stateUpperRight) && (currentState != stateUpperRight) && (upperEndStopState == LOW) && (rightEndStopState == HIGH)) {
          return actMoveRight;
        } else {
          if (lowerEndStopState == LOW) {
            currentState = stateLowerLeft;
            nextState = stateUpperLeft;
            return actMoveUp;
          } else if (rightEndStopState == LOW) {
            currentState = stateUpperRight;
            nextState = stateLowerRight;
            return actMoveDown;
          }
        }
      }
      if (currentState == stateLowerLeft) {
        //Serial.println("C");
        if ((nextState == stateUpperLeft) && (currentState != stateUpperLeft) && (leftEndStopState == LOW) && (upperEndStopState == HIGH)) {
          return actMoveUp;
        } else {
          currentState = stateUpperLeft;
          nextState = stateUpperRight;
          return actMoveRight;
        }
      }
      if (currentState == stateUpperRight) {
        //Serial.println("D");
        if ((nextState == stateLowerRight) && (currentState != stateLowerRight) && (rightEndStopState == LOW) && (lowerEndStopState == HIGH)) {
          //Serial.println("1");
          return actMoveDown;
        } else if ((nextState == stateUpperLeft) && (currentState != stateUpperLeft) && (upperEndStopState == LOW) && (leftEndStopState == HIGH)) {
          //Serial.println("2");
          return actMoveLeft;
        } else {
          //Serial.println("3");
          if (lowerEndStopState == LOW) {
            //Serial.println("3.1");
            currentState = stateLowerRight;
            nextState = stateUpperRight;
            return actMoveUp;
          } else if (leftEndStopState == LOW) {
            //Serial.println("3.2");
            currentState = stateUpperLeft;
            nextState = stateLowerLeft;
            return actMoveDown;
          }
        }
      }
      if (currentState == stateLowerRight) {
        //Serial.println("E");
        if ((nextState == stateUpperRight) && (currentState != stateUpperRight) && (rightEndStopState == LOW) && (upperEndStopState == HIGH)) {
          return actMoveUp;
        } else {
          currentState = stateUpperRight;
          nextState = stateUpperLeft;
          return actMoveLeft;
        }
      }
    }
};

StateMachine stateEngine;

void setup() {
  pinMode(leftEndStop, INPUT);
  pinMode(rightEndStop, INPUT);
  pinMode(horDirPin, OUTPUT);
  pinMode(horStepPin, OUTPUT);
  pinMode(upperEndStop, INPUT);
  pinMode(lowerEndStop, INPUT);
  pinMode(verDirPin, OUTPUT);
  pinMode(verStepPin, OUTPUT);
  Serial.begin(250000);

}

void loop() {
  stateEngine.readInput();
  int nextAction = stateEngine.nextAction();
  String nextActionLog = "Next action: " + String(nextAction);
  Serial.println(nextActionLog);
  switch (nextAction) {
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
