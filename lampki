const int ledPin1       = 3;
const int ledPin2       = 5;
const int buttonPin     = A3;    // używaj A3 jeśli działasz na Uno

int mode               = 0;      // tryb 0–3
bool ledState          = LOW;
unsigned long lastBlink   = 0;
unsigned long lastDebounce = 0;
bool buttonLocked       = false; // blokada do czasu zwolnienia

const unsigned long debounceDelay = 50;  // ms
const unsigned int intervals[]    = { 0, 500, 200, 100 };
const char* modeNames[] = {
  "0 – Wszystko wylaczone",
  "1 – Mruganie co 500 ms",
  "2 – Mruganie co 200 ms",
  "3 – Mruganie co 100 ms"
};

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(modeNames[mode]);
}

void loop() {
  unsigned long now = millis();
  bool reading = digitalRead(buttonPin);

  // 1. Debounce i blokada do zwolnienia
  if (reading == LOW && !buttonLocked && (now - lastDebounce) > debounceDelay) {
    // wykryte jednorazowe naciśnięcie
    buttonLocked   = true;
    lastDebounce   = now;
    mode           = (mode + 1) % 4;
    Serial.println(modeNames[mode]);
  }
  // 2. Po zwolnieniu przycisku odblokowujemy
  if (reading == HIGH && buttonLocked) {
    buttonLocked   = false;
    lastDebounce   = now;  // reset debounce
  }

  // 3. Realizacja trybów
  if (mode == 0) {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    return;
  }

  unsigned int interval = intervals[mode];
  if (now - lastBlink >= interval) {
    lastBlink = now;
    ledState  = !ledState;
    digitalWrite(ledPin1, ledState);
    digitalWrite(ledPin2, ledState);
  }
}
