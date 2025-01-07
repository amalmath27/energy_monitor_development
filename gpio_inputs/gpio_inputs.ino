// Define pins
#define PIN_8  8   //reset
#define PIN_46 46  //dnc 
#define PIN_9  9  // inc 
#define PIN_10 10  // func 
#define PIN_14 14  // esc

#define PIN_42 42  // wifi en
#define PIN_40 40  // sensor_en
#define PIN_41 41  // timer_en


void setup() {
    Serial.begin(115200);
    
    // Configure pins
    pinMode(PIN_8,  INPUT_PULLUP);
    pinMode(PIN_46, INPUT_PULLUP);
    pinMode(PIN_9,  INPUT_PULLUP);
    pinMode(PIN_10, INPUT_PULLUP);
    pinMode(PIN_14, INPUT_PULLUP);

    pinMode(PIN_42, INPUT_PULLUP);
    pinMode(PIN_40, INPUT_PULLUP);
    pinMode(PIN_41, INPUT_PULLUP);
}

void loop() {
    // Check each pin individually
    if(digitalRead(PIN_8) == LOW) {
        Serial.println("GPIO 8 is pressed! - Reset ");
    }
    
    if(digitalRead(PIN_46) == LOW) {
        Serial.println("GPIO 46 is pressed! - DNC");
    }
    
    if(digitalRead(PIN_9) == LOW) {
        Serial.println("GPIO 9 is pressed! - INC");
    }
    
    if(digitalRead(PIN_10) == LOW) {
        Serial.println("GPIO 10 is pressed! - FUNC");
    }
    
    if(digitalRead(PIN_14) == LOW) {
        Serial.println("GPIO 14 is pressed! - ESC");
    }

    if(digitalRead(PIN_42) == LOW) {
        Serial.println("GPIO 42 is pressed! - wifi en ");
    }

    if(digitalRead(PIN_40) == LOW) {
        Serial.println("GPIO 40 is pressed! - sensor en");
    }

    if(digitalRead(PIN_41) == LOW) {
        Serial.println("GPIO 41 is pressed! - timer en");
    }


    
    delay(300);
}