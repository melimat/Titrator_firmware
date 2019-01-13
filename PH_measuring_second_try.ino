const float voltagePHmiddle = 2.5;
const float voltagePHlower = 3.0;
const float voltagePHupper = 2.1;

float PH_step_lower = (voltagePHmiddle - voltagePHlower) / (6.86 - 4.01);
float PH_step_upper = (voltagePHmiddle - voltagePHupper) / (6.86 - 9.18);

const int ph_pin = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int measure = analogRead(ph_pin);
  Serial.print("Measure: ");
  Serial.print(measure);

  double voltage = 5 / 1024.0 * measure;
  Serial.print("\tVoltage: ");
  Serial.print(voltage, 3);
  float Po;
  // PH_probe = PH7 - ((voltage@PH7 - voltage@probe) / PH_step)
  if (voltage >= voltagePHmiddle){
    Po = 7 + ((2.5 - voltage) / PH_step_lower);
    
  } else if (voltage <= voltagePHmiddle){
    Po = 7 + ((2.5 - voltage) / PH_step_upper);
  }
  
  Serial.print("\tPH: ");
  Serial.print(Po, 3);

  Serial.println("");
  delay(2000);
}
