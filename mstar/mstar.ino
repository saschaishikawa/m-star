#define PIN_G 3
#define PIN_R 5
#define PIN_B 6

#define PIN_BTN 2
#define DEBOUNCE_DELAY 100

bool VERBOSE = false;

// Button states 
volatile int currentStateBTN;
int lastFlickerableStateBTN; // Reliable state
int lastSteadyStateBTN;
int lastDebounceTimeBTN;
int menuCount = 5;
int currentMenuIndex = 0;

int r = 0, g = 0, b = 0;
int fadeSpeed;
int fadeStep;

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);

  fadeSpeed = 0;
  fadeStep = 1;
  currentStateBTN = 0;

  // Attach interrupt to button pin
  pinMode(PIN_BTN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN), checkButtonPress, CHANGE);

  // Set up RGB pins
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_B, OUTPUT);
}

void loop() {
  runRGB();
}

/*
 * Fade LEDs through various colors. Break cycle on interrupt.
 */
void cycleRGB(int fadeSpeed=1, int fadeStep=5) {

  // Blue to purple
  for (r = 0; r <= 255; r=r+fadeStep) { 
    if (currentStateBTN == HIGH) { return; }
    analogWrite(PIN_R, r);
    delay(fadeSpeed);
  } 

  // Purple to red
  for (b = 255; b > 0; b=b-fadeStep) { 
    if (currentStateBTN == HIGH) { return; }
    analogWrite(PIN_B, b);
    delay(fadeSpeed);
  }

  // Red to yellow
  for (g = 0; g < 256; g=g+fadeStep) { 
    if (currentStateBTN == HIGH) { return; }
    analogWrite(PIN_G, g);
    delay(fadeSpeed);
  }

  // Yellow to green
  for (r = 255; r > 0; r=r-fadeStep) { 
    if (currentStateBTN == HIGH) { return; }
    analogWrite(PIN_R, r);
    delay(fadeSpeed);
  }

  // Green to teal
  for (b = 0; b < 256; b=b+fadeStep) { 
    if (currentStateBTN == HIGH) { return; }
    analogWrite(PIN_B, b);
    delay(fadeSpeed);
  }

  // Teal to blue
  for (g = 255; g > 0; g=g-fadeStep) { 
    if (currentStateBTN == HIGH) { return; }
    analogWrite(PIN_G, g);
    delay(fadeSpeed);
  } 
}

/*
 * Run color modes based on current selection
 */
void runRGB() {
  switch(currentMenuIndex) {
    case 0:
      solidColor(255, 60, 0);
      break;
    case 1:
      cycleRGB(20, 1);
      break;
    case 2:
      cycleRGB(10, 1);
      break;
    case 3:
      cycleRGB(1, 1);
      break;
    case 4:
      cycleRGB(1, 5);
      break;
    default:    
      cycleRGB(20, 1);
  }
}

/*
 * Display warm yellow color
 */
void solidColor(int r, int g, int b) {
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}
 

/*
 * Handle button interrupts
 */
void checkButtonPress() {
  Serial.print('.');

  // Read current button state
  currentStateBTN = digitalRead(PIN_BTN);
  
  // Potential button press (could be noise)
  if (currentStateBTN != lastFlickerableStateBTN) {
    lastDebounceTimeBTN = millis();
    lastFlickerableStateBTN = currentStateBTN;
  }

  // Debounce button presses
  if ((millis() - lastDebounceTimeBTN) > DEBOUNCE_DELAY) {
    if (lastSteadyStateBTN == HIGH && currentStateBTN == LOW) {
      if (VERBOSE) {
        Serial.println("The button is pressed");
      }
    } else if (lastSteadyStateBTN == LOW && currentStateBTN == HIGH) {
      cycleMenus();
      if (VERBOSE) {
        Serial.println("The button is released");
      }
    }
    lastSteadyStateBTN = currentStateBTN;
  }
}

/*
 * Cycle through menus when pressing button
 */
void cycleMenus() {
  currentMenuIndex++;
  if (currentMenuIndex >= menuCount) {
    currentMenuIndex = 0; // Reset to beginning
  }

  if (VERBOSE) {
    Serial.print("Current Menu: ");
    Serial.println(currentMenuIndex);
  }
}
