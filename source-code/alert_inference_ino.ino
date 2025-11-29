// Author: Guillermo Perez Guillen 

#include <smart-alert_inferencing.h>
#include <Wire.h>

static uint8_t recv_cmd[8] = {};

// Settings
#define THRESHOLD           0.8       // Threshold for performing action 0.8

// Constants
#define SAMPLING_FREQ_HZ    100       // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms) 1000
#define NUM_CHANNELS        EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME // 2 channels
#define NUM_READINGS        EI_CLASSIFIER_RAW_SAMPLE_COUNT      // 100 readings
#define NUM_CLASSES         EI_CLASSIFIER_LABEL_COUNT           // 3 classes

#define led1 D10 //PA4
#define led2 D11 //PA7

// Means and standard deviations from our dataset curation
static const float means[] = {0.4686, -0.4075, 8.3669, 0.0717, 4.7533, -9.9816};
static const float std_devs[] = {2.9989, 7.0776, 6.8269, 60.9333, 101.3666, 109.0392};

int analogPin1 = PA3; //A0    
int analogPin2 = PC3; //A2    
int val1 = 0;           
int val2 = 0;           

float test0 = 0.0;
float test1 = 0.0;
float test2 = 0.0;

float myArray[3];

void setup() {  
  // Enable button pin
  pinMode(USER_BTN, INPUT_PULLUP); //BTN pin
  pinMode(LED_BUILTIN, OUTPUT); //LED pin

  pinMode(led1, OUTPUT); 
  pinMode(led2, OUTPUT);
    
  // Start serial
  Serial1.setRx(PA10);                       //Redefine required Serial1 RX pin
  Serial1.setTx(PA9);                       //Redefine required Serial1 TX pin    
  Serial1.begin(115200);
}

void loop() {  
  uint8_t len = 0;
  uint8_t addr = 0;
  uint8_t i;
  //uint32_t val = 0;
  
  unsigned long timestamp;
  ei_impulse_result_t result;
  int err;
  float input_buf[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
  signal_t signal;

  // Wait for button press
  while (digitalRead(USER_BTN) == 1);

  // Turn on LED to show we're recording
  digitalWrite(LED_BUILTIN, LOW);

  // Record samples in buffer
  for (int i = 0; i < NUM_READINGS; i++) {

    // Take timestamp so we can hit our target frequency
    timestamp = millis();

    // Get raw readings from the gas sensor  
    val1 = analogRead(analogPin1);
    int mq135 = val1;
    val2 = analogRead(analogPin2);
    int mq7 = val2;

    // Perform standardization on each reading
    // Use the values from means[] and std_devs[]
    mq135 = (mq135 - means[0]) / std_devs[0];
    mq7 = (mq7 - means[1]) / std_devs[1];

    // Fill input_buf with the standardized readings. Recall tha the order
    // is [mq135, mq7 ...]
    input_buf[(NUM_CHANNELS * i) + 0] = mq135;
    input_buf[(NUM_CHANNELS * i) + 1] = mq7;

    // Wait just long enough for our sampling period
    while (millis() < timestamp + SAMPLING_PERIOD_MS);
  }

  // Turn off LED to show we're done recording
  digitalWrite(LED_BUILTIN, HIGH);

  // Turn the raw buffer into a signal for inference
  err = numpy::signal_from_buffer(input_buf, 
                                  EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, 
                                  &signal);
  if (err != 0) {
    Serial1.print("ERROR: Failed to create signal from buffer: ");
    Serial1.println(err);
    return;
  }

  // Run the impulse
  err = run_classifier(&signal, &result, false);
  if (err != 0) {
    Serial1.print("ERROR: Failed to run classifier: ");
    Serial1.println(err);
    return;
  }

  // Print the results
  Serial1.println("Predictions");
  for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    Serial1.print("  ");
    Serial1.print(result.classification[i].label);
    Serial1.print(": ");
    Serial1.println(result.classification[i].value);
    myArray[i] = (result.classification[i].value);
  }
    
  if (myArray[0] > 0.65) {         //ALCOHOL
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    delay(500);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);    
  }
  if (myArray[1] > 0.65) {         //GAS
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  if (myArray[2] > 0.65) {         //SMOKE
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    delay(500);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  delay(500);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  else {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  
  // Make sure the button has been released for a few milliseconds
  while (digitalRead(USER_BTN) == 0);
  delay(100);
}
