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
const int stirPin = A1;
const int phPin = A0;

const int verticalPulsesLimit = 1000;
const int stirTime = 10000;

const float voltagePHmiddle = 2.5;
const float voltagePHlower = 3.0;
const float voltagePHupper = 2.1;
float PH_step_lower = (voltagePHmiddle - voltagePHlower) / (6.86 - 4.01);
float PH_step_upper = (voltagePHmiddle - voltagePHupper) / (6.86 - 9.18);
const int measuringInterval = 1000;
const int numberOfMeasurements = 10;

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
const int stateStirring = 1000;

const int actIdle = 0;
const int actMoveLeft = 100;
const int actMoveRight = 200;
const int actMoveDown = 500;
const int actMoveUp = 600;
const int actPippetteDown = 700;
const int actPippetteUp = 800;
const int actStir = 1000;

float measureRawValue() {
  int measure = analogRead(phPin);
  double voltage = 5 / 1024.0 * measure;
  float Po;
  if (voltage >= voltagePHmiddle) {
    Po = 7 + ((2.5 - voltage) / PH_step_lower);
  } else if (voltage <= voltagePHmiddle) {
    Po = 7 + ((2.5 - voltage) / PH_step_upper);
  }
  return (Po);
}

class StateMachine {
  public:
    int currentState = stateInit;
    int nextState = stateUpperLeft;
    int numberOfVerticalPulses;
    int numberOfAddOperations;
    bool leftEndStopState;
    bool rightEndStopState;
    bool upperEndStopState;
    bool lowerEndStopState;
    bool lowerPippetteEndStopState;
    bool upperPippetteEndStopState;
    bool stirring;
    bool processingOfData;
    unsigned long stirringStartTime;
    unsigned long lastMeasuringTime;
    int performedMeasurements;
    float phArray[10];
    String prevSerialData;
    String serialData;


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
      String serialPacket;
      if (Serial.available()){
        serialPacket = Serial.readString();
        if ((serialPacket != prevSerialData) && (serialPacket.length() > 1)){
          prevSerialData = serialData;
          serialData = serialPacket;
        }
      }
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
        numberOfVerticalPulses ++;
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
    void stir() {
      unsigned long currentTime = millis();
      float ph = 0;
      if ((stirring != true) && (processingOfData != true)) {
        digitalWrite(stirPin, HIGH);
        stirringStartTime = currentTime;
        numberOfAddOperations += 1;
        ph = measureRawValue();
        //Serial.println(ph);
        lastMeasuringTime = currentTime;
        performedMeasurements += 1;
        phArray[performedMeasurements - 1] = ph;
        stirring = true;
      } else if ((stirring == true) && ((currentTime - stirringStartTime) <= stirTime)) {
        digitalWrite(stirPin, HIGH);
        if (((currentTime - lastMeasuringTime) >= measuringInterval) && (performedMeasurements < numberOfMeasurements)) {
          ph = measureRawValue();
          //Serial.println(ph);
          lastMeasuringTime = currentTime;
          performedMeasurements += 1;
          phArray[performedMeasurements - 1] = ph;
        }
      } else if ((stirring == true) && ((currentTime - stirringStartTime) >= stirTime)) {
        digitalWrite(stirPin, LOW);
        processingOfData = true;
        performedMeasurements = 0;
        stirringStartTime = 0;
      }
      if (processingOfData == true) {
        float backup = 0;
        float sumOfAllElements = 0;
        float average = 0;
        for (int i = 0; i < 9; i++) {
          for (int j = i + 1; j < 10; j++) {
            if (phArray[i] > phArray[j]) {
              backup = phArray[i];
              phArray[j] = phArray[i];
              phArray[i] = backup;
            }
          }
        }
        for (int i = 1; i < 9; i++) {
          sumOfAllElements += phArray[i];
        }
        average = sumOfAllElements / 8;
        String logString = "Adds:" + String(numberOfAddOperations) + ";Ph:" + String(average);
        Serial.println(logString);
        stirring = false;
        processingOfData = false;
      }
    }

    int nextAction() {
      //      String verticalEndStops = "UpperEndStop: " + String(upperEndStopState) + " ; LowerEndStop: " + String(lowerEndStopState) + "\n";
      //      String horizontalEndStops = "LeftEndStop: " + String(leftEndStopState) + " ;  RightEndStop: " + String(rightEndStopState) + "\n";
      //      String pippetteEndStops = "UpperPipEndStop: " + String(upperPippetteEndStopState) + " ; lowerPippetteEndStop: " + String(lowerPippetteEndStopState) + "\n";
      //      String states = "Current state: " + String(currentState) + " ; Next state: " + String(nextState) + "\n";
      //      String verticalPulses = "Amount of verPulses: " + String(numberOfVerticalPulses) + "\n";
      //      String logString = verticalEndStops + horizontalEndStops + pippetteEndStops + verticalPulses + states + "\n";
      //      Serial.println(logString);
      if (serialData == "STOP\r\n"){
        return actIdle;
      }

      if ((currentState == stateInit) && (serialData == "START\r\n" )){
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
            numberOfVerticalPulses = 0;
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
        } else if ((nextState == stateLowerRight) && (currentState != stateLowerRight) && (upperPippetteEndStopState == LOW)) {
          currentState = stateLowerRight;
          nextState = stateUpperRight;
          return actMoveUp;
        } else {
          if ((lowerPippetteEndStopState == LOW) && (nextState == statePippetteLow)) {
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
          } else if (leftEndStopState == HIGH) {
            nextState = stateLowerRight;
          }
          return actIdle;
        }
      }
      if (currentState == stateUpperRight) {
        if ((nextState == stateLowerRight) && (currentState != stateLowerRight) && (lowerEndStopState == HIGH) && (numberOfVerticalPulses < verticalPulsesLimit)) {
          return actMoveDown;
        } else if ((nextState == stateUpperLeft) && (currentState != stateUpperLeft) && (leftEndStopState == HIGH)) {
          return actMoveLeft;
        } else if ((nextState == stateStirring) && (currentState != stateStirring) && (upperEndStopState == LOW)) {
          currentState = stateStirring;
          nextState = stateUpperRight;
          return actStir;
        } else {
          if (leftEndStopState == LOW) {
            currentState = stateUpperLeft;
            nextState = stateLowerLeft;
            return actMoveDown;
          } else if ((lowerEndStopState == LOW) || (numberOfVerticalPulses >= verticalPulsesLimit)) {
            numberOfVerticalPulses = 0;
            currentState = stateLowerRight;
            nextState = statePippetteUp;
            return actIdle;
          }
        }
      }
      if (currentState == stateStirring) {
        if ((stirring == true) && (nextState == stateUpperRight) && (currentState != stateUpperRight)) {
          return actStir;
        } else if ((stirring == false) && (nextState == stateUpperRight) && (currentState != stateUpperRight)) {
          currentState = stateUpperRight;
          nextState = stateUpperLeft;
          return actMoveLeft;
        }
      }
      if (currentState == stateLowerRight) {
        if ((nextState == statePippetteUp) && (currentState != statePippetteUp) && (upperEndStopState == HIGH)) {
          currentState = statePippetteUp;
          nextState = statePippetteLow;
          return actPippetteDown;
        } else if ((nextState == stateUpperRight) && (currentState != stateUpperRight) && (upperEndStopState == HIGH)) {
          return actMoveUp;
        } else {
          if ((upperEndStopState == LOW) && (nextState == stateUpperRight)) {
            currentState = stateUpperRight;
            nextState = stateStirring;
            return actStir;
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
  pinMode(stirPin, OUTPUT);
  pinMode(phPin, INPUT);
  Serial.begin(9600);

}

void loop() {
  stateEngine.readInput();
  int nextAction = stateEngine.nextAction();
  //
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
    case (actStir):
      stateEngine.stir();
      break;
    default:
      0;
  }
}
