#include <LCD-I2C.h>
#include <Wire.h>
#include <Arduino.h>

const int yellowB = 10;
const int blueB = 11;
const int redB = 12;
const int greenB = 13;

const int replayB = A0;
const int upB = A1;
const int downB = A2;

const int buzzer = 2;

const int ledGreen = 3;
const int ledRed = 4;
const int ledBlue = 5;
const int ledYellow = 6;

const int dataPin = 8;
const int clockPin = 9;
const int strobePin = 7;

int level = 1;
char user_input[10];
unsigned int pos = 0;
int difficulty = 1;
int liveDifficulty = 1;
int length;
String level_pattern;

bool waitInput = false;

const int debounceDelay = 500;
unsigned long lastDebounceTime = 0;
char lastButtonState = '\0';

LCD_I2C lcd(0x27, 16, 2);

void centerText(const char* text, int length, char* buffer) {
    int start = (16 - length) / 2;
    for (int i = 0; i < start; i++) {
        buffer[i] = ' ';
    }
    strncpy(buffer + start, text, length);
    for (int i = start + length; i < 16; i++) {
        buffer[i] = ' ';
    }
    buffer[16] = '\0';
}

void showLcdInfo(const char *text) {
  lcd.clear();
  char line1[17] = {0};
  char line2[17] = {0};
  char buffer1[17] = {0};
  char buffer2[17] = {0};

  String strText(text);
  String words[10];
  int wordCount = 0;
  int pos = 0;
  String delimiter = " ";

  while ((pos = strText.indexOf(delimiter)) != -1) {
      words[wordCount++] = strText.substring(0, pos);
      strText.remove(0, pos + delimiter.length());
  }
  words[wordCount++] = strText;

  if (wordCount == 1) {
      centerText(words[0].c_str(), words[0].length(), line1);
  } else if (wordCount == 2) {
      centerText(words[0].c_str(), words[0].length(), line1);
      centerText(words[1].c_str(), words[1].length(), line2);
  } else if (wordCount == 3) {
      snprintf(buffer1, sizeof(buffer1), "%s %s", words[0].c_str(), words[1].c_str());
      centerText(buffer1, strlen(buffer1), line1);
      centerText(words[2].c_str(), words[2].length(), line2);
  } else if (wordCount >= 4) {
      int length1 = 0;
      int length2 = 0;
      int i = 0;

      while (i < wordCount) {
          if (length1 + words[i].length() + (length1 > 0 ? 1 : 0) <= 16) {
              if (length1 > 0) {
                  strcat(buffer1, " ");
                  length1++;
              }
              strcat(buffer1, words[i].c_str());
              length1 += words[i].length();
          } else {
              break;
          }
          i++;
      }

      while (i < wordCount) {
          if (length2 + words[i].length() + (length2 > 0 ? 1 : 0) <= 16) {
              if (length2 > 0) {
                  strcat(buffer2, " ");
                  length2++;
              }
              strcat(buffer2, words[i].c_str());
              length2 += words[i].length();
          }
          i++;
      }

      centerText(buffer1, length1, line1);
      centerText(buffer2, length2, line2);
  }

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}


byte pin[] = {7, 8, 9, 10, 11, 12, 13};
const byte digitMap[10] = {
  0b00111111, // 0
  0b00001100, // 1
  0b01011011, // 2
  0b01011110, // 3
  0b01101100, // 4
  0b01110110, // 5
  0b01110111, // 6
  0b00011100, // 7
  0b01111111, // 8
  0b01111110  // 9
};

void playBuzzer(char option) {
  switch(option) {
    case 'g': {
      tone(buzzer, 4000);
      break;
    }

    case 'r': {
      tone(buzzer, 3000);
      break;
    }

    case 'b': {
      tone(buzzer, 2000);
      break;
    }

    case 'y': {
      tone(buzzer, 1000);
      break;
    }
  }
}

String get_level(int difficulty) {
  char availableChars[] = {'r', 'g', 'b', 'y'};
  String result = "";

  length = 5 + (difficulty - 1) * 2;

  for (int i = 0; i < length; ++i) {
    int randomIdx = random(0, 4);
    result += availableChars[randomIdx];
  }

  return result;
}

void playLevel(String pattern) {
  for (int i = 0; i < length; i++) {
    playBuzzer(pattern[i]);
    switch (pattern[i]) {
      case 'g': // green
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledYellow, LOW);
        break;
      case 'r': // red
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledYellow, LOW);
        break;
      case 'b': // blue
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlue, HIGH);
        digitalWrite(ledYellow, LOW);
        break;
      case 'y': // yellow
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledYellow, HIGH);
        break;
      default:
        break;
    }

    int delayLevel = 700;
    if(liveDifficulty == 2) {
      delayLevel -= 200;
    }
    else if (liveDifficulty == 3) {
      delayLevel -= 400;
    }

    Serial.println(liveDifficulty);
    Serial.println(delayLevel);
    
    delay(delayLevel);
    noTone(buzzer);

    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledYellow, LOW);
    delay(100);
  }
}

void succes() {
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledYellow , LOW);
  delay(50);

  int notes[] = {1500, 0}; // Ton mai înalt pentru bucurie
  int durations[] = {300, 200};

  for(int j = 0; j < 3; j++) {
    for(int i = 0; i < 2; i++) {
      if(notes[i] != 0) {
        tone(buzzer, notes[i]);
      }
      
      switch(i) {
        case 0:
          digitalWrite(ledGreen, HIGH);
          digitalWrite(ledBlue, HIGH);
          break;
        case 1:
          digitalWrite(ledGreen, LOW);
          digitalWrite(ledBlue, LOW);
          break;
        default:
          break;
      }
      
      delay(durations[i]);
      noTone(buzzer);
    }
  }
}

void error() {
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledYellow , LOW);
  delay(50);

  int notes[] = {400, 0}; // Ton mai jos pentru tristețe
  int durations[] = {300, 200};

  for(int j = 0; j < 3; j++) {
    for(int i = 0; i < 2; i++) {
      if(notes[i] != 0) {
        tone(buzzer, notes[i]);
      }
      
      switch(i) {
        case 0:
          digitalWrite(ledRed, HIGH);
          digitalWrite(ledYellow , HIGH);
          break;
        case 1:
          digitalWrite(ledRed, LOW);
          digitalWrite(ledYellow , LOW);
          break;
        default:
          break;
      }
      
      delay(durations[i]);
      noTone(buzzer);
    }
  }
}

void setup() {
  pinMode(yellowB, INPUT);
  pinMode(blueB, INPUT);
  pinMode(redB, INPUT);
  pinMode(greenB, INPUT);

  pinMode(replayB, INPUT);
  pinMode(upB, INPUT);
  pinMode(downB, INPUT);

  pinMode(ledGreen, OUTPUT);    //Define LED pins
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(buzzer, OUTPUT); 
  Serial.begin(9600);           //Serial monitor used to determine limit values

  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(strobePin, OUTPUT);

  lcd.begin();
  lcd.display();
  lcd.backlight();
  showLcdInfo("START GAME!");

  displayDigit(1);

  bool play = true;
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledYellow, LOW);

  noTone(buzzer);
}

bool levelPassed(String level_pattern, char *user_input) {
  for (int i = 0; i < length; ++i) {
    if (level_pattern[i] != user_input[i]) {
      return false;
    }
  }

  return true;
}

void displayDigit(int digit) {
  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digitMap[digit]);
  digitalWrite(strobePin, HIGH);
}

void get_input()
{
  int yState = LOW;
  int bState = LOW;
  int rState = LOW;
  int gState = LOW;

  int reading;
  unsigned long currentMillis = millis();

  yState = digitalRead(yellowB);
  bState = digitalRead(blueB);
  rState = digitalRead(redB);
  gState = digitalRead(greenB);

  if (currentMillis - lastDebounceTime >= debounceDelay) {
    lastButtonState = '\0';
  }

  if (yState == HIGH && 'y' != lastButtonState)
  {
    lastDebounceTime = currentMillis;
    lastButtonState = 'y';
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, LOW); 
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledYellow, HIGH);
    user_input[pos] = 'y';
    pos++;
    playBuzzer('y');
    Serial.println("YELLOW BUTTON");
  }
  else if (bState == HIGH && 'b' != lastButtonState)
  {
    lastDebounceTime = currentMillis;
    lastButtonState = 'b';
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledBlue, HIGH);
    digitalWrite(ledYellow, LOW);
    user_input[pos] = 'b';
    pos++;
    playBuzzer('b');
    Serial.println("BLUE BUTTON");
  }
  else if (rState == HIGH && 'r' != lastButtonState)
  {
    lastDebounceTime = currentMillis;
    lastButtonState = 'r';
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledYellow, LOW);
    user_input[pos] = 'r';
    pos++;
    playBuzzer('r');
    Serial.println("RED BUTTON");
  }
  else if (gState == HIGH && 'g' != lastButtonState)
  {
    lastDebounceTime = currentMillis;
    lastButtonState = 'g';
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledYellow, LOW);
    user_input[pos] = 'g';
    pos++;
    playBuzzer('g');
    Serial.println("GREEN BUTTON");
  }

  delay(350);
  noTone(buzzer);
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledYellow, LOW);
}
// 233 234   463 465 491 492
void loop() {

  if (digitalRead(upB) == HIGH) {
    if (difficulty < 3) {
      difficulty++;
      displayDigit(difficulty);
      showLcdInfo("Difficulty++");
    }
  }

  if (digitalRead(downB) == HIGH) {
    if (difficulty > 1) {
      difficulty--;
      displayDigit(difficulty);
      showLcdInfo("Difficulty--");
    }
  }

  if(digitalRead(replayB) == HIGH) {
    playLevel(level_pattern);
    pos = 0;
    showLcdInfo("Replay!");
  }

  if (!waitInput) {
    Serial.println("no wait");
    level_pattern = get_level(difficulty);
    playLevel(level_pattern);

    waitInput = true;
  } else {
    Serial.println("wait");
    String message = "Level: " + String(level);
    showLcdInfo(message.c_str());
    Serial.println(lastButtonState);
    get_input();

    if (pos == length) {
      pos = 0;

      if (levelPassed(level_pattern, user_input)) {
        showLcdInfo("Congratulations! Passed!");
        liveDifficulty = difficulty;
        level++;
        waitInput = false;
        succes();
      } else {
        showLcdInfo("You Lost! Try Again!");
        error();
        playLevel(level_pattern);
      }
    }
  }
}