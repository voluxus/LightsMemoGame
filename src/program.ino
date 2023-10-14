#include <LiquidCrystal.h> // biblioteka LCD
#include <LinkedList.h> // lista w C
#define LCD1 8
#define LCD2 9
#define LCD3 10
#define LCD4 11
#define LCD5 12
#define LCD6 13

#define LIGHT1 3
#define LIGHT2 5

#define BUTTON1 4
#define BUTTON2 6

#define BUZZ 2

LiquidCrystal lcd(LCD1, LCD2, LCD3, LCD4, LCD5, LCD6);
LinkedList<bool> seq = LinkedList<bool>();
const int DEL_TIME = 300;
unsigned long TIME;
int points = 0;
int TOP1 = 0;
int TOP2 = 0;
int TOP3 = 0;

void Show()
{
  // play the sequence
  int listSize = seq.size();
  bool light;
  for (int i=0; i<listSize; i++)
  {
    light = seq.get(i);
    digitalWrite(BUZZ, HIGH);
    if (light == 0)
    {
       digitalWrite(LIGHT1, HIGH);
    }
    else
    {
       digitalWrite(LIGHT2, HIGH);
    }
    delay(DEL_TIME);
    digitalWrite(BUZZ, LOW);
    digitalWrite(LIGHT1, LOW);
    digitalWrite(LIGHT2, LOW);
    delay(DEL_TIME);
  }
}

void Add()
{
  lcd.clear();
  lcd.noBlink();
  // make a sound that we're appending a new number and add it
  for (int i=0; i<3; i++)
  {
    digitalWrite(BUZZ, HIGH);
    delay(DEL_TIME/4);
    digitalWrite(BUZZ, LOW);
    delay(DEL_TIME/4);
  }
  seq.add(random(2));
}

void Loose()
{
  points = seq.size() - 1;
  seq.clear();
  
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Miss! Score: ");
  lcd.print(points);
  lcd.setCursor(0, 1);
  lcd.print("TOP3: ");

  if (millis() - TIME > 30UL * 24 * 3600 * 1000) // if >30 days
  {
    // reset
    TOP1 = points;
    TOP2 = 0;
    TOP3 = 0;
    TIME = millis();
  }
  if (points > TOP1)
  {
    int temp = TOP2;
    TOP2 = TOP1;
    TOP3 = temp;
    TOP1 = points;
  }
  else if (points > TOP2)
  {
    TOP3 = TOP2;
    TOP2 = points;
  }
  else if (points > TOP3)
  {
    TOP3 = points;
  }
  
  lcd.print(TOP1);
  lcd.print(" ");
  lcd.print(TOP2);
  lcd.print(" ");
  lcd.print(TOP3);
  
  digitalWrite(BUZZ, HIGH);
  for (int i=0; i<3; i++)
  {
    for (int i=0; i<2; i++)
    {
      digitalWrite(LIGHT1, HIGH);
      digitalWrite(LIGHT2, HIGH);
      delay(0.5 * DEL_TIME);
      digitalWrite(LIGHT1, LOW);
      digitalWrite(LIGHT2, LOW);
      delay(0.5 * DEL_TIME);
    }
  }
  digitalWrite(BUZZ, LOW);

  unsigned long myTime = millis();
  // 15 s wait or button press
  while (digitalRead(BUTTON1) == HIGH && digitalRead(BUTTON2) == HIGH 
    && millis() - myTime < 15 * 1000)
  {
    delay(200);
  }
}

void Await()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Memory game!");
  lcd.setCursor(0, 1);
  lcd.print("Press a button.");
  lcd.blink();
  int fill = 0;
  int change = 5;
  while (digitalRead(BUTTON1) == HIGH && digitalRead(BUTTON2) == HIGH)
  {
    analogWrite(LIGHT1, fill);
    analogWrite(LIGHT2, fill);
    if (fill + change > 255 || fill + change < 0)
      change *= -1;
    fill += change;
    delay(50);
  }
  analogWrite(LIGHT1, 0);
  analogWrite(LIGHT2, 0);
  lcd.noBlink();
  lcd.clear();
}

bool Gather() // returns false any time the sequence is wrong
{
  lcd.setCursor(0, 0);
  lcd.print("Waiting for your");
  lcd.setCursor(0, 1);
  lcd.print("repetition!");
  lcd.blink();

  unsigned long myTime;
  int listSize = seq.size();
  bool BUT1;
  bool BUT2;
  for (int i = 0; i < listSize; i++)
  {
    myTime = millis();
    do
    {
      BUT1 = digitalRead(BUTTON1);
      BUT2 = digitalRead(BUTTON2);
      if (millis() - myTime > 15 * 1000)
        return false; // timeout 15 s
      delay(50);
    } while (BUT1 == HIGH && BUT2 == HIGH); // aquire one option

    if (BUT1 == LOW) // left is pressed
    {
      digitalWrite(LIGHT1, HIGH);
      delay(200);
      digitalWrite(LIGHT1, LOW);
      if (seq.get(i) != 0)
       return false;
    }
    else if (BUT2 == LOW) // right is pressed
    {   
      digitalWrite(LIGHT2, HIGH);
      delay(200);
      digitalWrite(LIGHT2, LOW);
      if (seq.get(i) != 1)
       return false;
    }
    delay(200);
  }
  return true; // sequence correct
}

void setup() {
  // put your setup code here, to run once:
  TIME = millis();
  randomSeed(analogRead(A0)); // random nums
  lcd.begin(16, 2);
  pinMode(LIGHT1, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Await();
  delay(DEL_TIME);
  do
  {
    delay(DEL_TIME);
    Add();
    delay(DEL_TIME);
    Show();
  } while (Gather());
  Loose();
  delay(200);
}
