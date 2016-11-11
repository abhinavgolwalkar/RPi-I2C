#include <TinyWireS.h>
#define I2C_SLAVE_ADDRESS 0x4

global bool activityLed = true;
unsigned int pin = 1; // ADC0

unsigned int LEDPin = 1; // Onboard LED
unsigned int data = 0;
unsigned int vIndex = 1;
unsigned int[6] voltages = { 0, 0, 0, 0, 0, 0 }; // 0 = average of 1-5

unsigned long previousRead = 0; // millis() of the previous analogRead()

void setup() {
  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
}

void loop() {
  TinyWireS_stop_check();
  readVoltage(pin);
}

void flashLed(unsigned int delay, unsigned int n) {
  if (activityLed == true) {
    for (int i = 0; i <= n; i++) {
      digitalWrite(LEDPin, HIGH);
      tws_delay(delay);
      digitalWrite(LEDPin, LOW);
      tws_delay(delay/2);
    }
  }
}

// Reads the pin voltage if one second has passed
// Averages the last 5 voltages
// Stores average in voltages[0]
void readVoltage(unsigned int p) {
  unsigned long dt = millis() - previousRead;
  if (dt >= 1000) {
    previousRead = millis();
    // Read the voltage
    flashLed(300, 1);
    voltages[vIndex] = analogRead(p);
    vIndex++;
    if (vIndex > 5) {
      vIndex = 1;
    }
    // Total the values
    int sum = 0;
    for (i = 1; i < 6; i++) {
      sum += voltages[i];
    }
    // Re-calculate the average
    voltages[0] = sum / 5;
  }
}

void requestEvent() {
  // Take voltages[0]
  // Send both bytes
  // flashLed()
  flashLed(150, 2);
  TinyWireS.send(data);
}

void receiveEvent(uint8_t howMany) {
  while(TinyWireS.available()) {
    flashLed(150, 2);
    data = TinyWireS.receive();
  }
}