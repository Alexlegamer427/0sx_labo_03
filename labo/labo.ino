

#include <LCD_I2C.h>
#define BTN_PIN 2

unsigned long currentTime = 0;
bool buttonSwitch = true;
int treshold_upper = 512;
int treshold_under = 497;

LCD_I2C lcd(0x27, 16, 2);
byte numDa[8] = { B11100, B10000, B11100, B00111, B11101, B00111, B00101, B00111 };
int Led_pin = 8;
int temp;

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  pinMode(Led_pin, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  lcd.print("Grenier");
  lcd.setCursor(0, 1);
  lcd.createChar(0, numDa);
  lcd.setCursor(15, 1);
  lcd.write(0);
  delay(3000);
  lcd.clear();
}





void loop() {
  currentTime = millis();
  static bool ledState = false;
  static int xCurrentValue = 0;
  static int yCurrentValue = 0;
  bool transition = readButton();



  if (transition) {
    if (!buttonSwitch) {
      buttonSwitch = true;

    } else if (buttonSwitch) {
      buttonSwitch = false;
    }
  }

  switch (buttonSwitch) {


    case 1:
      joystick_LCD_printing(xCurrentValue, yCurrentValue);
      break;

    default:
      thermisistor(ledState);
      break;
  }


  print_Da(currentTime, xCurrentValue, yCurrentValue, ledState);
}




bool readButton() {
  static int etatPrecedent = HIGH;
  static int etat = HIGH;
  const int delai = 50;
  static unsigned long dernierChangement = 0;

  int etatPresent = digitalRead(BTN_PIN);

  if (etatPresent != etatPrecedent) {
    dernierChangement = millis();
  }

  if ((millis() - dernierChangement) > delai && etatPresent != etat) {
    etat = etatPresent;

    if (etat == LOW) {

      return true;
    }
  }

  etatPrecedent = etatPresent;
  return false;
}


void print_Da(unsigned long currentTime, int xCurrentValue, int yCurrentValue, bool ledState) {

  static unsigned long lastTime = 0;
  const int rate = 100;

  if (currentTime - lastTime >= rate) {
    lastTime = currentTime;
    lcd.clear();

    Serial.print("etd:6308958,");
    Serial.print("");
    Serial.print(",X:");
    Serial.print(xCurrentValue);
    Serial.print("Y:");
    Serial.print(yCurrentValue);
    Serial.print(",sys:");
    Serial.println(ledState);
  }
}



bool thermisistor(bool ledState) {
  int ThermistorPin = A0;
  int Vo;            // Voltage à la sortie
  float R1 = 10000;  // Résistance
  float logR2, R2, T, Tc;
  float c1 = 1.129148e-03, c2 = 2.34125e-04, c3 = 8.76741e-08;

  const int upperScale = 25;
  const int downScale = 24;

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(5, 0);
  lcd.print(Tc);
  lcd.setCursor(10, 0);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("AC:");

  if (Tc < downScale) {
    digitalWrite(Led_pin, LOW);
    lcd.setCursor(3, 1);
    lcd.print("0");
    return false;
  } else if (Tc > upperScale) {
    digitalWrite(Led_pin, HIGH);
    lcd.setCursor(3, 1);
    lcd.print("1");
    return true;
  }
}

void joystick_LCD_printing(int& xCurrentValue, int& yCurrentValue) {

  int max_speed = 125;
  int max_opposite_speed = -125;
  int max_opposite_backing = 25;
  int max_backing = -25;

  xCurrentValue = analogRead(A1);
  yCurrentValue = analogRead(A2);

  int angle_x = map(xCurrentValue, 0, 1023, -90, 90);
  int y_temp_value = 0;


  lcd.setCursor(0, 0);
  lcd.print("Vitesse:");
  lcd.setCursor(9, 0);

  if (yCurrentValue < treshold_under) {

    y_temp_value = map(yCurrentValue, 0, 1023, max_speed, max_opposite_speed);

    lcd.print(y_temp_value);
  } else if (yCurrentValue > treshold_upper) {
    y_temp_value = map(yCurrentValue, 0, 1023, max_opposite_backing, max_backing);
    lcd.print(y_temp_value);

  } else {
    lcd.print("0");
  }
  lcd.setCursor(12, 0);
  lcd.print("km/h");
  lcd.setCursor(0, 1);
  lcd.print("Direction:");
  lcd.setCursor(11, 1);
  if (angle_x > 0) {

    lcd.print("D");

  } else if (angle_x < 0) {

    lcd.print("G");

  } else {
    lcd.print("C");
  }
}
