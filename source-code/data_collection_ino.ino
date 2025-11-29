// Author: Guillermo Perez Guillen

#include <Wire.h>

// Constants
#define SAMPLING_FREQ_HZ    100       // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms)
#define NUM_SAMPLES         100       // 100 samples at 100 Hz is 1 sec window

int analogPin1 = PA3; //A0 MQ-135   
int analogPin2 = PC3; //A2 MQ-7   
int val1 = 0;           
int val2 = 0;           

void setup() {
  // Enable button pin
  pinMode(USER_BTN, INPUT_PULLUP); //BTN pin
  pinMode(LED_BUILTIN, OUTPUT); //LED pin

  // Start serial
  Serial1.setRx(PA10);                       //Redefine required Serial1 RX pin
  Serial1.setTx(PA9);                       //Redefine required Serial1 TX pin    
  Serial1.begin(115200);
}

void loop() {
  uint8_t len = 0;
  uint8_t addr = 0;
  uint8_t i;

  unsigned long timestamp;
  unsigned long start_timestamp;

  // Wait for button press
  while (digitalRead(USER_BTN) == 1);

  // Turn on LED to show we're recording
  digitalWrite(LED_BUILTIN, LOW);

  // Print header
  Serial1.println("timestamp,MQ-135,MQ-7");
 
  // Record samples in buffer
  start_timestamp = millis();
  for (int i = 0; i < NUM_SAMPLES; i++) {

    // Take timestamp so we can hit our target frequency
    timestamp = millis();

    // Read and print values
    Serial1.print(timestamp - start_timestamp);
    Serial1.print(",");
    val1 = analogRead(analogPin1);     // read the input pin1
    Serial1.print(val1);             // debug value MQ-135
    Serial1.print(",");
    val2 = analogRead(analogPin2);     // read the input pin2
    Serial1.println(val2);             // debug value MQ-7
        
    // Wait just long enough for our sampling period
    while (millis() < timestamp + SAMPLING_PERIOD_MS);
  }

  // Print empty line to transmit termination of recording
  Serial1.println();

  // Turn off LED to show we're done
  digitalWrite(LED_BUILTIN, HIGH);

  // Make sure the button has been released for a few milliseconds
  while (digitalRead(USER_BTN) == 0);
  delay(100);
}
