// Define pins
#define PIN_39 39  // sensor out
#define PIN_16 16   // relay

void setup() {
    
    pinMode(PIN_16, OUTPUT);
    pinMode(PIN_39, OUTPUT);
}

void loop() {
    // Can only control GPIO 16
    digitalWrite(PIN_16, LOW);
    delay(1000);
    digitalWrite(PIN_39, LOW);
    delay(1000);
}