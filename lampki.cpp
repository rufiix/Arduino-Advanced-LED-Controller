// ---- PINY ----
const int ledPins[]             = {3, 5};
const int buttonPin             = A3;

// ---- PROGI JASNOŚCI ----
const int minBrt                = 120;
const int maxBrt                = 255;

// ---- TRYBY ----
// 0 – wszystko wyłączone
// 1–3 – mruganie obu LED (500, 200, 100 ms)
// 4 – naprzemienne świecenie: LED1 → LED2
// 5 – światła antykolizyjne (Strobe lights)
// 6 – bardzo szybkie miganie obu LED
int mode                        = 0;
const unsigned int intervals[]  = {0, 500, 200, 100};
const char* modeNames[]         = {
  "0 – Wszystko wyłączone",
  "1 – Mruganie co 500 ms",
  "2 – Mruganie co 200 ms",
  "3 – Mruganie co 100 ms",
  "4 – Naprzemienne LED1/LED2",
  "5 – Światła antykolizyjne",
  "6 – Bardzo szybkie miganie"
};

// ---- DEBOUNCE PRZYCISKU ----
bool    lastReading      = HIGH;
bool    buttonState      = HIGH;
unsigned long lastDebounce = 0;
const unsigned long debounceDelay = 50;

// ---- DETEKCJA DŁUGIEGO PRZYTRZYMANIA ----
unsigned long pressTime      = 0;
const unsigned long longPressTime = 3000;
bool    adjusting            = false;

// ---- REGULACJA JASNOŚCI ----
int     brightness           = maxBrt;
int     adjustDir            = -1;      // -1 = maleje, +1 = rośnie
const unsigned long adjustInterval = 30; // ms między zmianami jasności
unsigned long lastAdjust     = 0;

// ---- MRUGANIE & SEKWENCJE ----
bool    ledOn                = false;
unsigned long lastBlink      = 0;

// ---- STROBE (Tryb 5) ----
// krótkie impulsy sekwencyjnie na LED1 (tył) i LED2 (skrzydło)
const unsigned long strobeOn   = 50;   // ms stanu HIGH
const unsigned long strobeOff  = 100;  // ms przerwy
unsigned long lastStrobe       = 0;
int strobeStage                = 0;    // 0: LED1 on, 1: off, 2: LED2 on, 3: off

void setup() {
  for (int i = 0; i < 2; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(modeNames[mode]);
}

void loop() {
  unsigned long now = millis();
  bool reading      = digitalRead(buttonPin);

  // 1. Debounce
  if (reading != lastReading) {
    lastDebounce = now;
  }
  if (now - lastDebounce > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        pressTime = now;
        adjusting = false;
      } else {
        unsigned long held = now - pressTime;
        if (held < longPressTime && !adjusting) {
          mode = (mode + 1) % 7;
          Serial.println(modeNames[mode]);
        } else if (adjusting) {
          Serial.print("Final brightness = ");
          Serial.println(brightness);
        }
      }
    }

    // Wejście w tryb regulacji jasności po długim przytrzymaniu
    if (buttonState == LOW
        && !adjusting
        && (now - pressTime >= longPressTime)) {
      adjusting  = true;
      lastAdjust = now;
      adjustDir = (brightness <= minBrt) ? +1 :
                  (brightness >= maxBrt) ? -1 :
                  -1;
      Serial.println("Start regulacji jasności");
    }
  }
  lastReading = reading;

  // 2. Regulacja jasności (triangle wave)
  if (adjusting && buttonState == LOW) {
    if (now - lastAdjust >= adjustInterval) {
      lastAdjust = now;
      brightness += adjustDir;
      brightness = constrain(brightness, minBrt, maxBrt);
      if (brightness == minBrt || brightness == maxBrt) {
        adjustDir = -adjustDir;
      }
      Serial.print("Brt=");
      Serial.print(brightness);
      Serial.print(" Dir=");
      Serial.println(adjustDir);
    }
    analogWrite(ledPins[0], brightness);
    analogWrite(ledPins[1], brightness);
    return;
  }

  // 3. Tryby podstawowe i dodatkowe
  switch (mode) {
    case 0:  // wszystko wyłączone
      analogWrite(ledPins[0], 0);
      analogWrite(ledPins[1], 0);
      break;

    case 1: // mruganie obu LED co 500 ms
    case 2: // mruganie obu LED co 200 ms
    case 3: { // mruganie obu LED co 100 ms
      unsigned int interval = intervals[mode];
      if (now - lastBlink >= interval) {
        lastBlink = now;
        ledOn     = !ledOn;
        int level = ledOn ? brightness : 0;
        analogWrite(ledPins[0], level);
        analogWrite(ledPins[1], level);
      }
      break;
    }

    case 4: { // naprzemienne świecenie LED1 → LED2 (500 ms)
      const unsigned long altInterval = 500;
      if (now - lastBlink >= altInterval) {
        lastBlink = now;
        ledOn     = !ledOn;
      }
      analogWrite(ledPins[0], ledOn ? brightness : 0);
      analogWrite(ledPins[1], ledOn ? 0 : brightness);
      break;
    }

    case 5: { // światła antykolizyjne (Strobe lights)
      unsigned long interval = (strobeStage % 2 == 0) ? strobeOn : strobeOff;
      if (now - lastStrobe >= interval) {
        lastStrobe = now;
        strobeStage = (strobeStage + 1) % 4;
      }
      switch (strobeStage) {
        case 0: // LED1 on (tył)
          analogWrite(ledPins[0], brightness);
          analogWrite(ledPins[1], 0);
          break;
        case 1: // off
          analogWrite(ledPins[0], 0);
          analogWrite(ledPins[1], 0);
          break;
        case 2: // LED2 on (skrzydło)
          analogWrite(ledPins[0], 0);
          analogWrite(ledPins[1], brightness);
          break;
        case 3: // off
          analogWrite(ledPins[0], 0);
          analogWrite(ledPins[1], 0);
          break;
      }
      break;
    }

    case 6: { // bardzo szybkie miganie obu LED (50 ms)
      const unsigned long fastInterval = 50;
      if (now - lastBlink >= fastInterval) {
        lastBlink = now;
        ledOn     = !ledOn;
      }
      int lvl = ledOn ? brightness : 0;
      analogWrite(ledPins[0], lvl);
      analogWrite(ledPins[1], lvl);
      break;
    }
  }
}
