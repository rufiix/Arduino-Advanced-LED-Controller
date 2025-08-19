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
// 5 – "światła w samolocie" (4-krokowy wzór)
// 6 – bardzo szybkie miganie obu LED
int mode                        = 0;
const unsigned int intervals[]  = {0, 500, 200, 100};
const char* modeNames[]         = {
  "0 – Wszystko wyłączone",
  "1 – Mruganie co 500 ms",
  "2 – Mruganie co 200 ms",
  "3 – Mruganie co 100 ms",
  "4 – Naprzemienne LED1/LED2",
  "5 – Swiatelka w samolocie",
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

// dla trybu 5 (światła w samolocie)
const unsigned long planeInterval = 200; // ms
unsigned long lastPlaneStep       = 0;
const int planeSteps = 4;
byte planeSeq[planeSteps][2] = {
  {1, 0},  // tylko LED1
  {0, 1},  // tylko LED2
  {1, 1},  // oba LED
  {0, 0}   // oba wyłączone
};
int planeIndex = 0;

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
        // przycisk w dół → start potencjalnego długiego przytrzymania
        pressTime = now;
        adjusting = false;
      } else {
        // przycisk puszczony
        unsigned long held = now - pressTime;
        if (held < longPressTime && !adjusting) {
          // krótkie kliknięcie → zmiana trybu
          mode = (mode + 1) % 7;
          Serial.println(modeNames[mode]);
        } else if (adjusting) {
          // zakończenie regulacji jasności
          Serial.print("Final brightness = ");
          Serial.println(brightness);
        }
      }
    }

    // 2. Wejście w tryb regulacji jasności po 3 s
    if (buttonState == LOW
        && !adjusting
        && (now - pressTime >= longPressTime)) {
      adjusting  = true;
      lastAdjust = now;
      // wymuś poprawny kierunek startu
      adjustDir = (brightness <= minBrt) ? +1 :
                  (brightness >= maxBrt) ? -1 :
                  -1;
      Serial.println("Start regulacji jasności");
    }
  }
  lastReading = reading;

  // 3. Regulacja jasności (triangle wave między 120 a 255)
  if (adjusting && buttonState == LOW) {
    if (now - lastAdjust >= adjustInterval) {
      lastAdjust = now;
      brightness += adjustDir;
      // utrzymaj w minBrt…maxBrt
      brightness = constrain(brightness, minBrt, maxBrt);
      // odwróć kierunek na progach
      if (brightness == minBrt || brightness == maxBrt) {
        adjustDir = -adjustDir;
      }
      Serial.print("Brt=");
      Serial.print(brightness);
      Serial.print(" Dir=");
      Serial.println(adjustDir);
    }
    // ciągłe świecenie podczas regulacji
    analogWrite(ledPins[0], brightness);
    analogWrite(ledPins[1], brightness);
    return;  // pomiń mruganie/sekwencje
  }

  // 4. Tryby podstawowe i dodatkowe
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
        ledOn     = !ledOn;  // używamy ledOn jako przełącznik
      }
      analogWrite(ledPins[0], ledOn ? brightness : 0);
      analogWrite(ledPins[1], ledOn ? 0 : brightness);
      break;
    }

    case 5: { // "światła w samolocie" – 4-krokowa sekwencja
      if (now - lastPlaneStep >= planeInterval) {
        lastPlaneStep = now;
        planeIndex = (planeIndex + 1) % planeSteps;
      }
      analogWrite(ledPins[0], planeSeq[planeIndex][0] ? brightness : 0);
      analogWrite(ledPins[1], planeSeq[planeIndex][1] ? brightness : 0);
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
