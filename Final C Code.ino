#include <Keypad.h> // Include the Keypad library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define HX710B_DOUT 13  // Data Output pin for PB
#define HX710B_SCK  10  // Clock pin

#define PatmDOUT A2  // Data Output pin for Patm
#define PatmSCK  A3  // Clock pin

// Keypad configuration for numerical keypad (4x3 matrix)
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'} // '*' for confirm, '#' for cancel
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3};    // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Button pins
const int POWER_BUTTON_PIN = 2;    // Pin for the "power" button
const int ENTER_BUTTON_PIN = 11;    // Pin for the "enter" button
const int CALCULATE_BUTTON_PIN = 12; // Pin for the "calculate" button

// Function prototypes
float surfaceTensionCalc(float P0, float P1, float d);
float PBmaxCalc(float PBarr[], int size);
void readPressureSensor(float &P0, float PBarr[], int size);
float getDFromKeypad();

// Constants
const int ARRAY_SIZE = 200; // Size of PBarr array
const int SAMPLE_INTERVAL = 100; // Sampling interval in milliseconds

// Linear scaling constants
const float offsetRawData = 8388608;
const float pressureMaxRawData = 16777216;
const float atmPressurePa = 101325;

// Global variables
float d = 0; // Density (will be input by the user)

// Variables to store potentiometer readings
float P0;
float PBarr[ARRAY_SIZE];

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(HX710B_DOUT, INPUT);
  pinMode(HX710B_SCK, OUTPUT);

  pinMode(PatmDOUT, INPUT);
  pinMode(PatmSCK, OUTPUT);
  // Initialize analog input pins
  //pinMode(POT_P0_PIN, INPUT);
  //pinMode(POT_PB_PIN, INPUT);

  // Initialize button pins
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENTER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CALCULATE_BUTTON_PIN, INPUT_PULLUP);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press Power Btn");

}

void loop() {
  while (digitalRead(POWER_BUTTON_PIN) == HIGH);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter d: ");

  d = getDFromKeypad();
  
  lcd.setCursor(0, 1);
  lcd.print("d = ");
  lcd.print(d);

  while (digitalRead(ENTER_BUTTON_PIN) == HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reading Sensor...");

  readPressureSensor(P0, PBarr, ARRAY_SIZE);

  while (digitalRead(CALCULATE_BUTTON_PIN) == HIGH);

  float P1 = PBmaxCalc(PBarr, ARRAY_SIZE);

  float T = surfaceTensionCalc(P0, P1, d);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Surface Tension: ");
  lcd.setCursor(0, 1);
  lcd.print(T);
  delay(3000);
}


float surfaceTensionCalc(float P0, float P1, float d) {
  const float h0 = 0.07;
  const float r = 0.0004;
  const float g = 9.81;
  float h = h0 + r;
  float P2 = P0 + h * d * g;
  float T = (P1 - P2) * r / 2;
  return T;
}

float PBmaxCalc(float PBarr[], int size) {
  float PBmax = 0;
  for (int i = 0; i < size; i++) {
    if (PBmax <= PBarr[i]) {
      PBmax = PBarr[i];
    }
  }
  return PBmax;
}

void readPressureSensor(float &P0, float PBarr[], int size) {
    // Read the initial pressure (P0)
    long rawP0 = readPatm(); 
  
    P0 = mapFloat(rawP0, offsetRawData, pressureMaxRawData); // Scale to pascal guage
    //delay(1000);
  
    // Read multiple pressure values into PBarr[]
    for (int i = 0; i < size; i++) {
        long rawPB = readHX710B();
        PBarr[i] = mapFloat(rawPB, offsetRawData, pressureMaxRawData); // Scale to pascal guage
        //delay(100);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PB: ");
        lcd.setCursor(0, 1);
        lcd.print(PBarr[i] );
        delay(SAMPLE_INTERVAL);
    }
}

// Function to read a 24-bit value from the HX710B
long readHX710B() {
    long count = 0;

    // Wait until the sensor is ready (DOUT goes LOW)
    while (digitalRead(HX710B_DOUT) == HIGH);

    // Read 24-bit pressure data
    for (int i = 0; i < 24; i++) {
        digitalWrite(HX710B_SCK, HIGH);
        delayMicroseconds(1);
        count = (count << 1) | digitalRead(HX710B_DOUT);
        digitalWrite(HX710B_SCK, LOW);
        delayMicroseconds(1);
    }

    // Apply the gain setting (one extra clock pulse)
    digitalWrite(HX710B_SCK, HIGH);
    delayMicroseconds(1);
    digitalWrite(HX710B_SCK, LOW);
    delayMicroseconds(1);
    
    return count ^ 0x800000;  // Convert to signed value
}

long readPatm() {
    long result = 0;
    // Wait until the sensor is ready (DOUT goes LOW)
    while (digitalRead(PatmDOUT) == HIGH);

    // Read 24-bit pressure data
    for (int i = 0; i < 24; i++) {
        digitalWrite(PatmSCK, HIGH);
        delayMicroseconds(1);
        result = (result << 1) | digitalRead(HX710B_DOUT);
        digitalWrite(PatmSCK, LOW);
        delayMicroseconds(1);
    }

    // Apply the gain setting (one extra clock pulse)
    digitalWrite(PatmSCK, HIGH);
    delayMicroseconds(1);
    digitalWrite(PatmSCK, LOW);
    delayMicroseconds(1);
    
    return result ^ 0x800000;  // Convert to signed value
}

// Linear scaling function
float mapFloat(long x, long offset, long maxRawData) {
    return ((float)(x - offset) / (float)(maxRawData - offset) * 40000.0);
}

float getDFromKeypad() {
  String inputString = "";
  lcd.setCursor(0, 1);

  while (digitalRead(ENTER_BUTTON_PIN) == HIGH) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') { // '*' is the confirm key
        break;
      } else if (key >= '0' && key <= '9') { // Only allow numeric input
        inputString += key;
        lcd.print(key);
      }
    }
  }

  return inputString.toFloat(); // Convert the input string to a float
}
