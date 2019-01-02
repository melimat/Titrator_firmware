const int leftEndStop = 5;
const int rightEndStop = 6;
const int horDirPin = 4;
const int horStepPin = 3;
const int upperEndStop = 11;
const int lowerEndStop = 10;
const int verDirPin = 8;
const int verStepPin = 9;
const int upperPippetteEndStop = 7;
const int lowerPippetteEndStop = 2;
const int pipDirPin = 12;
const int pipStepPin = 13;

const int dirLeft = 100;
const int dirRight = 200;
const int dirDown = 300;
const int dirUp = 400;

const int stateInit = 0;
const int stateUpperRight = 200;
const int stateLowerRight = 500;
const int stateUpperLeft = 600;
const int stateLowerLeft = 700;
const int statePippetteLow = 800;
const int statePippetteUp = 900;

const int actIdle = 0;
const int actMoveLeft = 100;
const int actMoveRight = 200;
const int actMoveDown = 500;
const int actMoveUp = 600;
const int actPippetteDown = 700;
const int actPippetteUp = 800;


class StateMachine {
  public:
    int currentState = stateInit;
    int nextState = stateUpperLeft;
    bool leftEndStopState;
    bool rightEndStopState;
    bool upperEndStopState;
    bool lowerEndStopState;
    bool lowerPippetteEndStopState;
    bool upperPippetteEndStopState;


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
      lowerPippetteEndStopState = digitalRead(lowerPippetteEndStop);
      upperPippetteEndStopState = digitalRead(upperPippetteEndStop);
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

    void pipMovement (int moveDir) {
      if (moveDir == dirUp) {
        digitalWrite(pipDirPin, HIGH);
      } else {
        digitalWrite(pipDirPin, LOW);
      }
      if ((moveDir == dirDown) && (lowerPippetteEndStopState == LOW)) {
        return;
      } else if ((moveDir == dirUp) && (upperPippetteEndStopState == LOW)) {
        return;
      } else {
        digitalWrite(pipStepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(pipStepPin, LOW);
        delayMicroseconds(500);
      }
    }

    int nextAction() {
      String verticalEndStops = "UpperEndStop: " + String(upperEndStopState) + " ; LowerEndStop: " + String(lowerEndStopState) + "\n";
      String horizontalEndStops = "LeftEndStop: " + String(leftEndStopState) + " ;  RightEndStop: " + String(rightEndStopState) + "\n";
      String pippetteEndStops = "UpperPipEndStop: " + String(upperPippetteEndStopState) + " ; lowerPippetteEndStop: " + String(lowerPippetteEndStopState) + "\n";
      String states = "Current state: " + String(currentState) + " ; Next state: " + String(nextState) + "\n";
      String logString = verticalEndStops + horizontalEndStops + pippetteEndStops + states + "\n";
      Serial.println(logString);

      if (currentState == stateInit) {
        if ((nextState == stateUpperLeft) && (currentState != stateUpperLeft) && (leftEndStopState == HIGH)) {
          return actMoveLeft;
        } else {
          currentState = stateUpperLeft;
          nextState = stateLowerLeft;
          return actMoveDown;
        }
      }
      if (currentState == stateUpperLeft) {
        if ((nextState == stateLowerLeft) && (currentState != stateLowerLeft) && (leftEndStopState == LOW) && (lowerEndStopState == HIGH)) {
          return actMoveDown;
        } if ((nextState == stateUpperRight) && (currentState != stateUpperRight) && (upperEndStopState == LOW) && (rightEndStopState == HIGH)) {
          return actMoveRight;
        } else {
          if (lowerEndStopState == LOW) {
            currentState = stateLowerLeft;
            nextState = statePippetteUp;
            return actIdle;
          } else if (rightEndStopState == LOW) {
            currentState = stateUpperRight;
            nextState = stateLowerRight;
            return actMoveDown;
          }
        }
      }
      if (currentState == stateLowerLeft) {
        if ((nextState == statePippetteUp) && (currentState != statePippetteUp) && (leftEndStopState == LOW)) {
          currentState = statePippetteUp;
          nextState = statePippetteLow;
          return actPippetteDown;
        } else if ((nextState == stateUpperLeft) && (currentState != stateUpperLeft) && (upperEndStopState == HIGH)) {
          return actMoveUp;
        } else {
          if (upperEndStopState == LOW) {
            currentState = stateUpperLeft;
            nextState = stateUpperRight;
            return actMoveLeft;
          }
        }
      }
      if (currentState == statePippetteUp) {
        if ((nextState == statePippetteLow) && (currentState != statePippetteLow) && (leftEndStopState == LOW) && (lowerPippetteEndStopState == HIGH)) {
          return actPippetteDown;
        } else if ((nextState == stateLowerLeft) && (currentState != stateLowerLeft) && (upperPippetteEndStopState == LOW) && (lowerEndStopState == LOW)) {
          currentState = stateLowerLeft;
          nextState = stateUpperLeft;
          return actMoveUp;
        } else if ((nextState == stateLowerRight) && (currentState != stateLowerRight) && (upperPippetteEndStopState == LOW) && (lowerEndStopState == LOW)) {
          currentState = stateLowerRight;
          nextState = stateUpperRight;
          return actMoveUp;
        } else {
          if (lowerPippetteEndStopState == LOW) {
            currentState = statePippetteLow;
            nextState = statePippetteUp;
            return actPippetteUp;
          } else if (leftEndStopState == LOW) {
            currentState = stateUpperLeft;
            nextState = stateLowerLeft;
            return actMoveDown;
          }
        }
      }
      if (currentState == statePippetteLow) {
        if ((nextState == statePippetteUp) && (currentState != statePippetteUp) && (upperPippetteEndStopState == HIGH)) {
          return actPippetteUp;
        } else {
          currentState = statePippetteUp;
          if (leftEndStopState == LOW) {
            nextState = stateLowerLeft;
          } else if (rightEndStopState == LOW) {
            nextState = stateLowerRight;
          }
          return actIdle;
        }
      }
      if (currentState == stateUpperRight) {
        if ((nextState == stateLowerRight) && (currentState != stateLowerRight) && (lowerEndStopState == HIGH)) {
          return actMoveDown;
        } else if ((nextState == stateUpperLeft) && (currentState != stateUpperLeft) && (leftEndStopState == HIGH)) {
          return actMoveLeft;
        } else {
          if (leftEndStopState == LOW) {
            currentState = stateUpperLeft;
            nextState = stateLowerLeft;
            return actMoveDown;
          } else if (lowerEndStopState == LOW) {
            currentState = stateLowerRight;
            nextState = statePippetteUp;
            return actIdle;
          }
        }
      }
      if (currentState == stateLowerRight) {
        if ((nextState == statePippetteUp) && (currentState != statePippetteUp) && (rightEndStopState == LOW) && (upperEndStopState == HIGH)) {
          currentState = statePippetteUp;
          nextState = statePippetteLow;
          return actPippetteDown;
        } else if ((nextState == stateUpperRight) && (currentState != stateUpperRight) && (upperEndStopState == HIGH)) {
          return actMoveUp;
        } else {
          if (upperEndStopState == LOW) {
            currentState = stateUpperRight;
            nextState = stateUpperLeft;
            return actMoveLeft;
          }
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
  pinMode(upperPippetteEndStop, INPUT);
  pinMode(lowerPippetteEndStop, INPUT);
  pinMode(pipStepPin, OUTPUT);
  pinMode(pipDirPin, OUTPUT);
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
    case (actPippetteUp):
      stateEngine.pipMovement(dirUp);
      break;
    case (actPippetteDown):
      stateEngine.pipMovement(dirDown);
      break;
    default:
      0;
  }
}
