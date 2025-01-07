// Define ADC pins
const int NUM_ADC_PINS = 6;
const int adcPins[NUM_ADC_PINS] = {4, 5, 6, 7, 15, 1};

// Arrays to store channel information
int adcValues[NUM_ADC_PINS];

void setup() {
    Serial.begin(115200);
    
    // Configure ADC resolution (12-bit = 0-4095)
    analogReadResolution(12);
    
    // ESP32 ADC attenuation setting for full range (0-3.3V)
    for(int i = 0; i < NUM_ADC_PINS; i++) {
        analogSetPinAttenuation(adcPins[i], ADC_11db);
    }
}

void loop() {
    // Read and print ADC values for each pin
    Serial.println("\n----- ADC Readings -----");
    
    for(int i = 0; i < NUM_ADC_PINS; i++) {
        adcValues[i] = analogRead(adcPins[i]);
        
        // Print raw ADC value
        Serial.print("GPIO ");
        Serial.print(adcPins[i]);
        Serial.print(" ADC Value: ");
        Serial.print(adcValues[i]);
        
        // Convert to voltage (assuming 3.3V reference)
        float voltage = (adcValues[i] * 3.3) / 4095.0;
        Serial.print(" Voltage: ");
        Serial.print(voltage, 2);
        Serial.println("V");
    }
    
    delay(5000);  // Read every second
}