// Iambic mode B keyer
// Anand Biehl

// Change these vars to configure the keyer
// Words per minute
int wpm = 20;
// Farnsworth speed
int farnsworthSpeed = 15;
// Note frequency. in herz
int note = 600;

// Internal variables
#include <LiquidCrystal.h>
//#include <EEPROM.h>

int Contrast = 110;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned long mspc = 1200 / wpm;
char last = 'a';

int leftPaddle = 9;
int rightPaddle = 7;

int menu = 8;
bool menuState = false;

int cMenu = 13;

int buzzer = 10;

int trimPot = A3;

char nextTone = '0';

int leftPressed = 0;
int rightPressed = 0;
int bothPressed = 0;

int readRight = 5;
int readLeft = 5;

unsigned long duration = 0;

int buzzing = 0;

int br = 0;

// int lc = 0;
int c = 9;

int i = 0;
int j = 0;


void setup() {
  Serial.begin(9600);

  //Contrast = EEPROM.read(0);

  // threshold = EEPROM.read(2);

  digitalWrite(13, LOW);

  // Setup the display
  pinMode(13, OUTPUT);
  analogWrite(6, Contrast);
  lcd.begin(16, 2);

  // Clear and setup LCD
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Speed: ");
  lcd.setCursor(9, 0);
  lcd.print(wpm);
  lcd.print(" WPM");

  lcd.setCursor(2, 1);
  lcd.print("Freq.: ");
  lcd.setCursor(9, 1);
  lcd.print(note);
  lcd.print(" Hz");
  lcd.setCursor(0, 0);
  lcd.print("*");
  lcd.setCursor(0, 1);

  // Init menu toggle
  pinMode(menu, INPUT_PULLUP);
  pinMode(cMenu, INPUT_PULLUP);

  // Initialize the paddle pins
  pinMode(leftPaddle, INPUT_PULLUP);
  pinMode(rightPaddle, INPUT_PULLUP);

  // Init. buzzer & trimpot
  pinMode(buzzer, OUTPUT);
  pinMode(trimPot, INPUT);

  // Print debug info
  // // Serial.print(wpm);
  // // Serial.print(" ");
  // // Serial.println(mspc);
}

void loop() {
  delay(50);

  // Read Paddles
  int readLeft = digitalRead(leftPaddle);
  int readRight = digitalRead(rightPaddle);
  unsigned long cMillis = millis();

  // Serial.print(readLeft);
  // Serial.print(" ");  
  // Serial.println(readRight);

  // if (readLeft >= threshold){
  //   readLeft = 0;
  // } else {
  //   readLeft = 1;
  // }

  // if (readRight >= threshold){
  //   readRight = 0;
  // } else {
  //   readRight = 1;
  // }


  // Read menu switch & trimPot
  int readMenu = digitalRead(menu);
  int wpmAdj = map(analogRead(trimPot), 1, 1023, 1, 50);
  int noteAdj = map(analogRead(trimPot), 2, 1023, 100, 999);

  if (br == 0) {
    switch (c) {
      case (0):
        //  // Serial.println("BOTH");
        if (last == 'L') {
          Serial.println("LEFT BOTH");
          if ((cMillis - duration) >= (mspc)) {
            noTone(buzzer);
            br = 1;
            duration = millis();
            buzzing = 0;
            // lc = c;
            last = 'R';
          }
        } else if (last == 'R') {
          if ((cMillis - duration) >= (mspc * 3)) {
          Serial.println("RIGHT BOTH");
            noTone(buzzer);
            br = 1;
            duration = millis();
            buzzing = 0;
            last = 'L';
          }
        }

        break;

      case (1):
        if ((cMillis - duration) >= (mspc)) {
          Serial.println("Left");
          noTone(buzzer);
          leftPressed = 0;
          br = 1;
          duration = millis();
          buzzing = 0;
          last = 'L';
        }
        break;

      case (2):
        if ((cMillis - duration) >= (mspc * 3)) {
          Serial.println("Right");
          noTone(buzzer);
          rightPressed = 0;
          br = 1;
          duration = millis();
          buzzing = 0;
          last = 'R';
        }
        break;
    }
  } else if ((cMillis - duration) >= (mspc)) {
    br = 0;
  }


  // // Serial.println(analogRead(trimPot));

  // Menu handling
  if (readMenu == 0) {
    // Cursor management
    lcd.setCursor(0, menuState);
    lcd.print(" ");
    menuState = !menuState;
    lcd.setCursor(0, menuState);
    lcd.print("*");
    // lcd.setCursor(0, !menuState);
    delay(250);
  }

  if (menuState == false && wpm != wpmAdj) {
    // If the cursor is set to WPM adjustment, and there is an adjustment to be made, adjust
    wpm = wpmAdj;
    mspc = 1200 / wpm;
    lcd.setCursor(9, menuState);

    // Handles using single digit characters
    if (wpm >= 10) {
      lcd.print(wpm);
    } else {
      lcd.print("0");
      lcd.print(wpm);
    }

    // Set cursor to the wrong menu line to prevent rapid refreshing problem
    lcd.setCursor(0, !menuState);

  } else if (menuState == true && note != noteAdj) {
    // Same as above but for tone adjustment
    note = noteAdj;
    lcd.setCursor(9, menuState);
    lcd.print(note);

    lcd.setCursor(0, !menuState);
  }

  // The contrast adjustment menu
  if (digitalRead(cMenu) == 0) { // TODO: change so that it can be operated on the menu button


    // Button checking variable
    int bb = 1;

    // Delay so that user can unpress button
    delay(500);

    // if (leftPressed == 1){
    //   lcd.clear();
    //   lcd.print("Sensitivity");

    //   while (true){
    //     threshold = touchRead(leftPaddle);
    //     lcd.setCursor(2,1);
    //     lcd.print("                              ");
    //     lcd.setCursor(2,1);
    //     lcd.print(threshold);
    //     delay(50);

    //     bb = digitalRead(cMenu);
    //       if (bb == 0) {
    //         break;
    //       }
    //   }

    //   //EEPROM.write(2, threshold);
    // } else {
        // Prepare the LCD to show the contrast and related vars
        lcd.clear();
        lcd.print("Contrast");
        lcd.setCursor(2, 1);
        lcd.print(map(Contrast, 0, 256, 0, 100));
        lcd.print("%");

        while (true) {
        // Read values from trimPot and map them to display contrast percentages
        Contrast = map(analogRead(trimPot), 1, 1023, 0, 253);
        analogWrite(6, Contrast);
        lcd.setCursor(2, 1);
        lcd.print(map(Contrast, 0, 253, 0, 99));
        lcd.print("%");

        // // Serial.println(map(Contrast, 0, 253, 0, 99));

        // Check button
        bb = digitalRead(cMenu);
        if (bb == 0) {
          break;
        }
      //}

      //EEPROM.write(0, Contrast);
    }

    delay(250);

    // Clear and setup LCD
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Speed: ");

    if (wpm >= 10) {
      lcd.print(wpm);
    } else {
      lcd.print("0");
      lcd.print(wpm);
    }

    lcd.setCursor(2, 1);
    lcd.print("Freq.: ");
    lcd.setCursor(9, 1);
    lcd.print(note);

    lcd.setCursor(0, menuState);
    lcd.print("*");
    lcd.setCursor(15, 0);
  }

  // if (!bothPressed ) { // && !leftPressed && !rightPressed
    if (readLeft == 0 && readRight == 0) {
      bothPressed = 1;
      //  // Serial.println("b");
    } else if (readRight == 0) {
      rightPressed = 1;
      // lc = 2;
      //  // Serial.println("r");
    } else if (readLeft == 0) {
      leftPressed = 1;
      // lc = 1;
      //  // Serial.print("l");
    }
  // }

  if ((bothPressed || leftPressed || rightPressed) && !br && !buzzing) {
    tone(buzzer, note);
    duration = millis();
    buzzing = 1;

    if (bothPressed) {
      // lc = c;
      c = 0;
    } else if (leftPressed) {
      // lc = c;
      c = 1;
    } else {
      // lc = c;
      c = 2;
    }

    bothPressed = 0;
    leftPressed = 0;
    rightPressed = 0;
  }
}
