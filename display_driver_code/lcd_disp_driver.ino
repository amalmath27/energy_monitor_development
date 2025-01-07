/**
 * LCD Driver for HT16C23/HT16C23G 
 * 
 * This code implements an I2C-like protocol for controlling a custom LCD display.
 * It handles various display sections including kWh, timer, current, voltage, and phase readings.
 */

// Pin Definitions
#define SCLPIN 3    
#define SDAPIN 2    

// Timing Constants
#define TIME_1 40000    // Primary delay constant (microseconds)
#define TIME_2 30000    // Secondary delay constant (microseconds)

// Display Pattern Arrays
const uint8_t MASK2[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
const uint8_t MASK[] = {0xE, 0x48, 0xD6, 0xDA, 0x78, 0xBA, 0xBE, 0xC8, 0xFE, 0xFA};

// For 0 to 9 representation 
const uint8_t ZERO_NINE_DISP_ARR[] = {0xEF, 0x48, 0xD6, 0xDA, 0x78, 0xBA, 0xBE, 0xC8, 0xFF, 0xFA};

// Buffer for display data
uint8_t l_data[50];

// Address mapping for display segments
const uint8_t Z_ADDR[] = {
    46, 7, 46, 6, 46, 5, 46, 4, 46, 3,
    33, 0, 34, 0, 36, 0, 37, 0, 38, 0, 39, 0, 23, 0,
    24, 0, 25, 0, 26, 0, 28, 0, 30, 0, 29, 0, 40, 0,
    40, 7, 40, 5, 40, 6, 41, 5, 41, 6, 41, 7, 40, 4, 41, 4,
    40, 1, 40, 2, 40, 3, 41, 2, 41, 3, 41, 1,
    42, 7, 42, 5, 42, 6, 43, 5, 43, 6, 43, 7, 42, 4, 43, 4,
    42, 1, 42, 2, 42, 3, 43, 2, 43, 3, 43, 1,
    44, 7, 44, 5, 44, 6, 45, 5, 45, 6, 45, 7, 44, 4, 45, 4,
    44, 1, 44, 2, 44, 3, 45, 2, 45, 3, 45, 1,
    4, 0, 21, 0, 20, 0, 19, 0, 18, 0, 17, 0, 16, 0, 15, 0,
    13, 0, 12, 0, 11, 0, 7, 0, 22, 0, 14, 0
};

/**
 * I2C Protocol Implementation Functions
 */

// Short delay for I2C timing
void delayShort() {
    delayMicroseconds(200);
}

// Generate I2C start condition
void i2cStart() {
    digitalWrite(SDAPIN, HIGH);
    digitalWrite(SCLPIN, HIGH);
    delayShort();
    digitalWrite(SDAPIN, LOW);
    delayShort();
    digitalWrite(SCLPIN, LOW);
}

// Generate I2C stop condition
void i2cStop() {
    digitalWrite(SDAPIN, LOW);
    digitalWrite(SCLPIN, HIGH);
    delayShort();
    digitalWrite(SDAPIN, HIGH);
    delayShort();
}

// Read I2C acknowledgment
bool i2cAck() {
    pinMode(SDAPIN, INPUT);
    digitalWrite(SCLPIN, HIGH);
    delayShort();
    bool ack = digitalRead(SDAPIN);
    digitalWrite(SCLPIN, LOW);
    pinMode(SDAPIN, OUTPUT);
    return ack;
}

// Write a single bit to I2C bus
void i2cWriteBit(bool bit) {
    digitalWrite(SDAPIN, bit);
    delayShort();
    digitalWrite(SCLPIN, HIGH);
    delayShort();
    digitalWrite(SCLPIN, LOW);
}

// Write a byte to I2C bus
void i2cWriteByte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        i2cWriteBit((byte & 0x80) != 0);
        byte <<= 1;
    }
}

/**
 * LCD Control Functions
 */

// Write command to LCD
void f_wc(uint8_t command, uint8_t data) {
    i2cStart();
    i2cWriteByte(0x7C);
    i2cAck();
    i2cWriteByte(command);
    i2cAck();
    i2cWriteByte(data);
    i2cAck();
    i2cStop();
}

// Write data to LCD address
void f_wd1(uint8_t addr, uint8_t data) {
    i2cStart();
    i2cWriteByte(0x7C);
    i2cAck();
    i2cWriteByte(0x80);
    i2cAck();
    i2cWriteByte(addr);
    i2cAck();
    i2cWriteByte(data);
    i2cAck();
    i2cStop();
}

// Fill LCD with specified value
void f_fill0(uint8_t value) {
    for (uint8_t i = 0; i < 47; i++) {
        f_wd1(i, value);
    }
}

// Display animation helper function
void f_hhx(uint8_t t) {
    // Initialize display buffer
    for (uint8_t i = 0; i < 40; i++) {
        l_data[i] = MASK[8];
    }
    for (uint8_t i = 40; i < 47; i++) {
        l_data[i] = 0x00;
    }
    
    // Update display segments
    for (uint8_t i = 0; i < t + 1; i += 2) {
        l_data[Z_ADDR[i]] |= MASK2[Z_ADDR[i + 1]];
    }
    
    // Write to display
    for (uint8_t i = 0; i < 47; i++) {
        f_wd1(i, l_data[i]);
    }
}

// Run display animation
void f_hh() {
    for (uint8_t i = 0; i < 150; i += 2) {
        f_hhx(i);
        delayMicroseconds(TIME_2);
    }
}

// Initialize LCD
void lcdReset() {
    f_wc(0x82, 0x03);    // 1/8 duty 1/4 bias
    f_wc(0x84, 0x03);
    f_wc(0x86, 0x00);    // 80Hz refresh rate
    f_wc(0x88, 0x10);    // Blinking 2Hz (01), off (00)
    f_wc(0x8A, 0x10);
}

/**
 * Display Section Functions
 */

// Display kWh section
void kwh_section_disp(uint32_t val) {
    for (int i = 0; i < 5; i++) {
        uint8_t temp_val = val % 10;
        val /= 10;
        f_wd1(i, ZERO_NINE_DISP_ARR[temp_val]);
        Serial.println(temp_val);    // Debug output
    }
}

// Display timer section (minutes and seconds)
void timer_section_disp(uint8_t min, uint8_t sec) {
    // Display seconds
    uint8_t temp_val = sec % 10;
    f_wd1(0x05, ZERO_NINE_DISP_ARR[temp_val]);
    temp_val = sec / 10;
    f_wd1(0x06, ZERO_NINE_DISP_ARR[temp_val]);

    // Display minutes with colon
    temp_val = min % 10;
    f_wd1(0x07, (ZERO_NINE_DISP_ARR[temp_val] | 0x01));
    temp_val = min / 10;
    f_wd1(0x08, ZERO_NINE_DISP_ARR[temp_val]);
}

// Display S section values
void s_section_disp(uint32_t val) {
    for (int i = 0x09; i <= 0x0E; i++) {
        uint8_t temp_val = val % 10;
        val /= 10;
        f_wd1(i, ZERO_NINE_DISP_ARR[temp_val]);
    }
}

// Display flow liters section
void flow_ltrs_section_disp(uint32_t val) {
    for (int i = 0x0F; i <= 0x16; i++) {
        uint8_t temp_val = val % 10;
        val /= 10;
        f_wd1(i, ZERO_NINE_DISP_ARR[temp_val]);
    }
}

// Display output current section
void op_curr_section_disp(uint8_t current, uint8_t fraction) {
    // Display current value
    uint8_t temp_val = current % 10;
    f_wd1(0x17, ZERO_NINE_DISP_ARR[temp_val] | 0x01);   // Enable OP CURR indicator
    
    temp_val = current / 10;
    f_wd1(0x18, ZERO_NINE_DISP_ARR[temp_val] | 0x01);   // Enable decimal point
    
    // Display fraction
    temp_val = fraction % 10;
    f_wd1(0x19, ZERO_NINE_DISP_ARR[temp_val] | 0x01);   // Enable Amp indicator
}

// Display output voltage section
void OP_volt_section_disp(uint16_t val) {
    // Display units digit with V indicator
    uint8_t temp_val = val % 10;
    f_wd1(0x1C, ZERO_NINE_DISP_ARR[temp_val] | 0x01);
    
    // Display tens digit
    val /= 10;
    temp_val = val % 10;
    f_wd1(0x1B, ZERO_NINE_DISP_ARR[temp_val]);
    
    // Display hundreds digit
    val /= 10;
    temp_val = val % 10;
    f_wd1(0x1A, ZERO_NINE_DISP_ARR[temp_val]);
}

// Display power factor section
void PF_section_disp(uint8_t val) {
    uint8_t temp_val = val % 10;
    f_wd1(0x1E, ZERO_NINE_DISP_ARR[temp_val] | 0x01);   // Enable indicator
    
    temp_val = val / 10;
    f_wd1(0x1D, ZERO_NINE_DISP_ARR[temp_val] | 0x01);
}

// Display phase 1 section
void ph1_section_disp(uint16_t val) {
    f_wd1(0x2E, 0xF);    // Enable phase 1 indicator
    
    for (int i = 0x21; i >= 0x1F; i--) {
        uint8_t temp_val = val % 10;
        val /= 10;
        f_wd1(i, ZERO_NINE_DISP_ARR[temp_val] | 0x01);
    }
}

// Display phase 2 section
void ph2_section_disp(uint16_t val) {
    for (int i = 0x24; i >= 0x22; i--) {
        uint8_t temp_val = val % 10;
        val /= 10;
        f_wd1(i, ZERO_NINE_DISP_ARR[temp_val] | 0x01);
    }
}

// Display phase 3 section with WiFi indicator
void ph3_section_disp(uint16_t val, bool wifi) {
    uint8_t temp_val = val % 10;
    f_wd1(0x27, ZERO_NINE_DISP_ARR[temp_val] | (wifi ? 0x01 : 0x00));
    
    val /= 10;
    temp_val = val % 10;
    f_wd1(0x26, ZERO_NINE_DISP_ARR[temp_val] | 0x01);
    
    val /= 10;
    temp_val = val % 10;
    f_wd1(0x25, ZERO_NINE_DISP_ARR[temp_val] | 0x01);
}

// pending - do not use
// Display timer on/off section
void timer_on_off_section_disp() {
    // This section combines with S section and Flow Ltrs Section
    for (uint8_t addr = 0x0B; addr <= 0x10; addr++) { // pending
        f_wd1(addr, 0xFF);
    }
}

// pending - do not use
// Display relay on/off section
void relay_on_off_section_disp() {
    // This section combines with Flow Ltrs Section
    for (uint8_t addr = 0x11; addr <= 0x15; addr++) {  
        f_wd1(addr, 0xFF);
    }
}

/**
 * Setup and Main Loop
 */

void setup() {
    // Initialize pins
    pinMode(SCLPIN, OUTPUT);
    pinMode(SDAPIN, OUTPUT);
    digitalWrite(SCLPIN, LOW);
    digitalWrite(SDAPIN, LOW);

    // Initialize LCD
    lcdReset();
   
}

void loop() {
    lcdReset();
    f_fill0(0x00);

    // Example display sequence
    kwh_section_disp(0);
    delay(1000);
    
    timer_section_disp(25, 40);
    delay(1000);
    
    s_section_disp(21301);
    delay(1000);
    
    flow_ltrs_section_disp(1115623);
    delay(1000);
    
    op_curr_section_disp(12, 5);
    OP_volt_section_disp(1);
    PF_section_disp(21);
    
    ph1_section_disp(123);
    ph2_section_disp(123);
    ph3_section_disp(123, 1);    // WiFi enabled

    // Additional display updates
    
     f_wd1(0x28,0xFF);
     f_wd1(0x29,0xFF);
     f_wd1(0x2A,0xFF);
     f_wd1(0x2B,0xFF);

     f_wd1(0x2C,0xFF);
     f_wd1(0x2D,0xFF);

      
     f_wd1(0x2F,0xFF);  // i think not used 

    while(1){

    }

}
